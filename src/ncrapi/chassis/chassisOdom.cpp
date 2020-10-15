#include "ncrapi/chassis/chassisOdom.hpp"
#include "ncrapi/system/sysUser.hpp"
#include "ncrapi/units/QMass.hpp"
#include "ncrapi/userDisplay/userDisplay.hpp"
#include <iomanip>

namespace ncrapi
{

ChassisOdom::ChassisOdom(const json &pragma, const std::array<int, 128> *frspeed, const std::array<int, 128> *routerSpeed, const std::array<int, 128> *translationSpeed)
    : Chassis(pragma, frspeed, routerSpeed, translationSpeed), _speedPid{VelPid("速度pid", pragma["PID参数"]), VelPid("速度pid", pragma["PID参数"])},
      _distancePid("前后pid", pragma["PID参数"]), _anglePid("左右pid", pragma["PID参数"]), _gyroFilter(0.01, 0.02)
{
    _multiplier = clamp<double>(pragma["参数"]["陀螺仪比例"].get<double>(), 0.1, 2.0, "陀螺仪比例");
    _gyroBalance = clamp<double>(pragma["参数"]["陀螺仪平衡"].get<double>(), 0.1, 2.0, "陀螺仪平衡");
    for (auto &it : pragma["ADI"].items())
    {
        std::string tempStr = it.key();
        if (tempStr.find("陀螺仪") != std::string::npos)
        {
            _gyroList.push_back({it.key(), pragma["ADI"]});
            _gyroNums++;
        }
        if (tempStr.find("红盒子") != std::string::npos)
        {
            _redEncList.push_back({it.key(), pragma["ADI"]});
            _isRedEndMode = true;
        }
        else
            _isRedEndMode = false;
    }
    _speedPid[0].reName("速度pid左");
    _speedPid[1].reName("速度pid右");
    //均值滤波
    int total[2] = {0}, gyroMax[2] = {0}, gyroMin[2] = {9999, 9999};
    pros::delay(100);
    for (int i = 0; i < 130; i++)
    {
        size_t count = 0;
        for (auto &it : _gyroList)
        {
            int val = it.get_value();
            if (val > gyroMax[count])
                gyroMax[count] = val;
            if (val < gyroMin[count])
                gyroMin[count] = val;
            total[count] += val;
            count++;
        }
        pros::delay(10);
    }
    for (size_t i = 0; i < _gyroNums; i++)
    {
        if (gyroMax[i] - gyroMin[i] > 5)
            logger->error({"陀螺仪: ", std::to_string(i), " 初始化错误!,请重启机器重新初始化"});
        _gyroBenchmark[i] = total[i] / 130;
        logger->info({"陀螺仪:", std::to_string(i + 1), " 最大值:", std::to_string(gyroMax[i]), "陀螺仪最小值:", std::to_string(gyroMin[i]), "陀螺仪基准值:", std::to_string(_gyroBenchmark[i])});
    }

    logger->info({"里程计底盘类构造成功"});
}
void ChassisOdom::resetEnc()
{
    if (_isRedEndMode)
        for (auto &it : _redEncList)
            it.reset();
    else
        Chassis::resetEnc();
}

double ChassisOdom::getEnc(const bool side)
{
    if (_isRedEndMode)
        return _redEncList[side].get_value();
    else
        return Chassis::getEnc(side);
}

/**
     *重置底盘上所有传感器 
    */
void ChassisOdom::resetAllSensors()
{
    Chassis::resetEnc();
    resetEnc();
    for (size_t i = 0; i < _gyroNums; i++)
        _gyroNow[i] = 0_deg;
    _filterAngle = 0_deg;
}
void ChassisOdom::isStopPid(const bool isStopPid)
{
    _isStopPid = isStopPid;
}

std::pair<QTime, double> ChassisOdom::speedPid(QSpeed leftSpd, QSpeed rightSPd)
{
    isStopPid(false); //不要停止PID
    uint32_t prevWakeTime = pros::millis();
    _speedPid[0].setTarget(leftSpd.convert(mps));  //前馈常数设置跟PID目标值有关 先设置目标 再重置PID
    _speedPid[1].setTarget(rightSPd.convert(mps)); //前馈常数设置跟PID目标值有关 先设置目标 再重置PID
    _speedPid[0].reset();
    _speedPid[1].reset();
    bool atTarget = false;
    const QSpeed atTargetSpeed = 0.01_mps; //速度允许误差 每秒1厘米
    Timer atTargetTimer;
    const QTime timeoutPeriod = 50_ms;
    while (!atTarget)
    {
        set(static_cast<int>(_speedPid[0].step(_encSpeed[0].convert(mps))), static_cast<int>(_speedPid[1].step(_encSpeed[1].convert(mps))));
        if (((leftSpd - _encSpeed[0]).abs() <= atTargetSpeed) && ((rightSPd - _encSpeed[1]).abs() <= atTargetSpeed)) //前后允许误差
            atTargetTimer.placeHardMark();
        else
            atTargetTimer.clearHardMark();
        if (atTargetTimer.getDtFromHardMark() >= timeoutPeriod || _isStopPid)
            atTarget = true;
        pros::Task::delay_until(&prevWakeTime, 10);
    }
    Chassis::stop();
    return std::make_pair(atTargetTimer.getDtFromStart(), _speedPid[0].getError());
}

/**
     * 机器人前后控制FOR PID
     * @param itarget 前后运动的距离
     * return 返回程序运行时间
    */
std::pair<QTime, double> ChassisOdom::forwardPid(QLength itarget)
{
    isStopPid(false); //不要停止PID
    const double startEnc[2] = {_encNow[0], _encNow[1]};
    const QAngle startAngle = _filterAngle;
    uint32_t prevWakeTime = pros::millis();
    _distancePid.setTarget(itarget.convert(millimeter)); //前馈常数设置跟PID目标值有关 先设置目标 再重置PID
    _distancePid.reset();
    //_distancePid.showPidParams();
    _anglePid.setTarget(0);
    _anglePid.reset();
    bool atTarget = false;
    const double atTargetDistance = 4.0; //前后允许误差
    const QSpeed threshold = 0.01_mps;   //速度变化到多慢的时候跳出
    Timer atTargetTimer;
    const QTime timeoutPeriod = 50_ms;
    double tickEnc[2] = {0};
    QAngle angleChange = {0_deg};
    while (!atTarget)
    {
        for (auto i = 0; i < 2; i++)
            tickEnc[i] = _encNow[i] - startEnc[i];
        double distanceElapsed = (tickEnc[0] + tickEnc[1]) / 2.0 * sysData->diameter; //编码器距离
        angleChange = _filterAngle - startAngle;
        driveVector(static_cast<int>(_distancePid.step(distanceElapsed)), static_cast<int>(_anglePid.step(angleChange.convert(degree)))); //开弧线 -负数
        if (fabs(itarget.convert(millimeter) - distanceElapsed) <= atTargetDistance)                                                      //前后允许误差
            atTargetTimer.placeHardMark();
        else if ((_encSpeed[0].abs() + _encSpeed[1].abs()) <= threshold && distanceElapsed != 0) //速度变化到多慢的时候跳出
            atTargetTimer.placeHardMark();
        else
            atTargetTimer.clearHardMark();
        if (atTargetTimer.getDtFromHardMark() >= timeoutPeriod || _isStopPid)
            atTarget = true;
        pros::Task::delay_until(&prevWakeTime, 10);
    }
    Chassis::stop();
    return std::make_pair(atTargetTimer.getDtFromStart(), _distancePid.getError());
}
/**
* 旋转PID 将机器人转动到适当的角度idegTarget。角度行程的单位通常是底盘两侧之间编码器刻度的差异。
* @param idegTarget 左转为+ 右转为-
* return 返回程序运行时间
*/

std::pair<QTime, double> ChassisOdom::rotatePid(QAngle idegTarget)
{
    isStopPid(false); //不要停止PID
    const QAngle startAngle = _filterAngle;
    uint32_t prevWakeTime = pros::millis();
    while (idegTarget > 180_deg)
        idegTarget -= 360_deg;
    while (idegTarget <= -180_deg)
        idegTarget += 360_deg;
    _anglePid.setTarget(idegTarget.convert(degree)); //前馈常数设置跟PID目标值有关 先设置目标 再重置PID
    _anglePid.reset();
    //  _anglePid.showPidParams();
    bool atTarget = false;
    const QAngle atTargetAngle = 0.5_deg;       //允许误差的角度
    const QAngularSpeed threshold = 0.01_radps; //速度变化到多慢的时候跳出
    Timer atTargetTimer;
    const QTime timeoutPeriod = 50_ms;
    QAngle angleChange = {0_deg};
    while (!atTarget)
    {
        angleChange = _filterAngle - startAngle;
        driveVector(0, static_cast<int>(_anglePid.step(angleChange.convert(degree)))); //这里输出有问题啊 似乎是负数
        //logger->info({"gyro:", std::to_string(angleChange), " enc:", std::to_string(encChange)});
        if ((idegTarget - angleChange).abs() <= atTargetAngle) //如果目标角度-当前角度小于等于允许误差角度 则break
            atTargetTimer.placeHardMark();
        else if (_nowAngleSpeed[0].abs() <= threshold && _nowAngleSpeed[1].abs() <= threshold) //如果当前角度-上次角度<=1 则break
            atTargetTimer.placeHardMark();
        else
            atTargetTimer.clearHardMark();
        if (atTargetTimer.getDtFromHardMark() >= timeoutPeriod || _isStopPid)
            atTarget = true;
        pros::Task::delay_until(&prevWakeTime, 10);
    }
    Chassis::stop();
    return std::make_pair(atTargetTimer.getDtFromStart(), _anglePid.getError());
}
/**
         * 开车到现场框架中的点（ix，iy）。如果需要，机器人将首先转向目标点。
         * @param ix 目的地的X坐标
         * @param iy 目的地的Y坐标
         * @param ibackwards 是否倒着开回来
         * @param ioffset 离目的地有多远停止
         */
void ChassisOdom::driveToPoint(QLength ix, QLength iy, const bool ibackwards, const QLength ioffset)
{
    ix = ncrapi::clamp(ix, sysData->robotWidth / 2, sysData->siteSize - (sysData->robotWidth / 2));
    iy = ncrapi::clamp(iy, sysData->robotWidth / 2, sysData->siteSize - (sysData->robotWidth / 2));
    DistanceAndAngle daa = OdomMath::computeDistanceAndAngleToPoint(ix, iy, getState());
    if (ibackwards) //是否倒着开
    {
        daa.theta += 180_deg;
        daa.length = daa.length * -1;
    }
    if (std::abs(daa.theta.convert(degree)) >= 1)
        rotatePid(daa.theta);
    if (std::fabs(daa.length.convert(millimeter) - ioffset.convert(millimeter)) >= _moveThreshold)
        forwardPid(daa.length - ioffset);
}

/**
 * 转到相对于场地的角度
 * @param 转向的角度 相对于XY坐标系内
 */
void ChassisOdom::driveToAngle(const QAngle iangle)
{
    rotatePid(iangle - getState().theta);
}

/**
     * 手动设置当前机器人姿态
     * @param x     X点坐标
     * @param y     Y点坐标
     * @param theta 机器人当前角度
     */
void ChassisOdom::setState(const QLength x, const QLength y, const QAngle theta)
{
    setX(x);
    setY(y);
    setTheta(theta);
}
/**
     * 单独设置X点坐标
     * @param x X点坐标
     */
void ChassisOdom::setX(const QLength x)
{
    _state.x = ncrapi::clamp(x, sysData->robotWidth / 2, sysData->siteSize - (sysData->robotWidth / 2));
    logger->debug({"设置x坐标为:", std::to_string(_state.x.convert(millimeter))});
}
/**
     * 单独设置Y点坐标
     * @param y Y点坐标
     */
void ChassisOdom::setY(const QLength y)
{
    _state.y = ncrapi::clamp(y, sysData->robotWidth / 2, sysData->siteSize - (sysData->robotWidth / 2));
    logger->debug({"设置y坐标为:", std::to_string(_state.y.convert(millimeter))});
}
/**
     * 单独设置机器人当前角度坐标
     * @param theta 机器人相对场地的角度
     */
void ChassisOdom::setTheta(const QAngle theta)
{
    _state.theta = theta;
    logger->debug({"设置theta坐标为:", std::to_string(_state.theta.convert(degree))});
}
QAngle ChassisOdom::getGyro()
{
    int count = 0, tempVal[2] = {0};
    QAngle tickGyro[2] = {0_deg, 0_deg};
    for (auto &it : _gyroList)
    {
        tempVal[count] = it.get_value() - _gyroBenchmark[count];
        tickGyro[count] = QAngle(_gyroFilter.filter(tempVal[count] / 128.0 * _multiplier) * degree);
        _gyroNow[count] += tickGyro[count] - _gyroDriftAvg;
        count++;
    }
    if (_gyroNums > 1)
    {
        if (copySign(tempVal[0]) == copySign(tempVal[1]))
            _isGyroReady = true;
        else
            _isGyroReady = false;
        return (tickGyro[0] + tickGyro[1]) / 2;
    }
    else
        return tickGyro[0];
}

/**
    * 返回机器人的当前姿态
    * @return 返回姿态类型
    */
const OdomState &ChassisOdom::getState()
{
    //logger->debug({"当前坐标为x:", std::to_string(_state.x), " y:", std::to_string(_state.y), " theta:", std::to_string(_state.theta)});
    return _state;
}
/**
     * 显示传感器数据到屏幕 ostringstream ostr流
     */
void ChassisOdom::showSensor()
{
    if (_gyroNums > 1)
        userDisplay->ostr << "陀螺仪L:" << _gyroNow[0].convert(degree) << "陀螺仪R:" << _gyroNow[1].convert(degree) << "编码器角度:" << _encAngle.convert(degree) << "滤波后角度:" << _filterAngle.convert(degree) << "\n";
    else
        userDisplay->ostr
            << "陀螺仪:" << _gyroNow[0].convert(degree) << "编码器角度:" << _encAngle.convert(degree) << "滤波后角度:" << _filterAngle.convert(degree) << "\n";
    userDisplay->ostr << "零飘率:" << std::setprecision(6) << _gyroDriftAvg.convert(degree) << std::setprecision(1) << "模式" << _mode << "\n"
                      << "左底盘: 编码器:" << _encNow[0] << " 温度:" << Chassis::getTemperature(0) << "电压:" << Chassis::getVoltage(0) << "电流:" << Chassis::getCurrent(0) << "\n"
                      << "右底盘: 编码器:" << _encNow[1] << " 温度:" << Chassis::getTemperature(1) << "电压:" << Chassis::getVoltage(1) << "电流:" << Chassis::getCurrent(1) << std::endl;
}
void ChassisOdom::showDetailedInfo()
{
    if (_gyroNums > 1)
        userDisplay->ostr << "陀螺仪L:" << _gyroNow[0].convert(degree) << "陀螺仪R:" << _gyroNow[1].convert(degree) << "编码器角度:" << _encAngle.convert(degree) << "滤波后角度:" << _filterAngle.convert(degree) << "零飘率:" << std::setprecision(6) << _gyroDriftAvg.convert(degree) << std::setprecision(1) << "模式" << _mode << "\n";

    else
        userDisplay->ostr << "陀螺仪:" << _gyroNow[0].convert(degree) << "编码器角度:" << _encAngle.convert(degree) << "滤波后角度:" << _filterAngle.convert(degree) << "零飘率:" << std::setprecision(6) << _gyroDriftAvg.convert(degree) << std::setprecision(1) << "模式" << _mode << "\n";
    userDisplay->ostr << "enc角速度:" << _nowAngleSpeed[0].convert(radps) << "radps 最大加速度:" << _maxAccelAngle[0] << "°/s2 gyro角速度:" << _nowAngleSpeed[1].convert(radps) << "radps 最大加速度:" << _maxAccelAngle[1] << "°/s2\n"
                      << "左底盘pwm:" << Chassis::_pwm[0] << "编码器:" << _encNow[0] << "温度:" << Chassis::getTemperature(0) << "电压:" << Chassis::getVoltage(0) << "电流:" << Chassis::getCurrent(0) << "\n"
                      << "机器人速度" << getRobotSpeed(0).convert(mps) << "mps 最大速度" << _maxEncSpeed[0].convert(mps) << "mps\n"
                      << "电机转速:" << Chassis::getSpeed(0) << "加速度:" << _nowAccelSpeed[0].convert(mps2) << "mps2 最大加速度:" << _maxAccelSpeed[0].convert(mps2) << "mps2\n"
                      << "右底盘pwm:" << Chassis::_pwm[1] << "编码器 : " << _encNow[1] << "温度 : " << Chassis::getTemperature(1) << "电压 : " << Chassis::getVoltage(1) << "电流 : " << Chassis::getCurrent(1) << "\n"
                      << "机器人速度" << getRobotSpeed(1).convert(mps) << "mps 最大速度" << _maxEncSpeed[1].convert(mps) << "mps\n"
                      << "电机转速:" << Chassis::getSpeed(1) << "加速度:" << _nowAccelSpeed[1].convert(mps2) << "mps2 最大加速度:" << _maxAccelSpeed[1].convert(mps2) << "mps2\n"
                      << "odom:x:" << _state.x.convert(millimeter) << " y:" << _state.y.convert(millimeter) << "角度:" << _state.theta.convert(degree) << std::endl;
}
void ChassisOdom::showOdom(bool isColor)
{
    if (!isColor)
    {
        if (_gyroNums > 1)
            userDisplay->ostr << "陀螺仪L:" << _gyroNow[0].convert(degree) << "陀螺仪R:" << _gyroNow[1].convert(degree) << "编码器角度:" << _encAngle.convert(degree) << "滤波后角度:" << _filterAngle.convert(degree) << "\n";
        else
            userDisplay->ostr << "陀螺仪:" << _gyroNow[0].convert(degree) << "编码器角度:" << _encAngle.convert(degree) << "滤波后角度:" << _filterAngle.convert(degree) << "\n";
        userDisplay->ostr << "零飘率:" << std::setprecision(6) << _gyroDriftAvg.convert(degree) << std::setprecision(1) << "模式" << _mode << "\n"
                          << "左编码器:" << _encNow[0] << "右编码器:" << _encNow[1] << "\n"
                          << "x:" << _state.x.convert(millimeter) << " y:" << _state.y.convert(millimeter) << "角度:" << _state.theta.convert(degree) << std::endl;
    }
    else
        userDisplay->ostr << "#FF0000 x:" << _state.x.convert(millimeter) << " y:" << _state.y.convert(millimeter) << "角度:" << _state.theta.convert(degree) << std::endl;
}
void ChassisOdom::resetPid(std::string_view name)
{
    std::string mode;
    if (sysData->jsonVal["底盘"]["参数"]["模式"] == 0)
        mode = "模式0";
    else
        mode = "模式1";
    if (name == "速度pid")
    {
        _speedPid[0].setAllPragmas(sysData->jsonVal["底盘"]["PID参数"][mode]["速度pid"]);
        _speedPid[1].setAllPragmas(sysData->jsonVal["底盘"]["PID参数"][mode]["速度pid"]);
    }

    else if (name == "前后pid")
        _distancePid.setAllPragmas(sysData->jsonVal["底盘"]["PID参数"][mode]["前后pid"]);
    else if (name == "左右pid")
        _anglePid.setAllPragmas(sysData->jsonVal["底盘"]["PID参数"][mode]["左右pid"]);
    else
        logger->error({"pid名字录入错误"});
}
/**
     *  独立线程的机器人姿态测算循环
     */
void ChassisOdom::odomLoop()
{
    std::valarray<double> newEnc(2), tickEnc(2), lastEnc(2);
    QLength mm = 0_mm;
    uint32_t now = pros::millis(); //这里很奇怪 他怎么放在LOOP外面
    Timer gyroCalcTimer;           //陀螺仪积分计时器
    QAngle gyroCalc = 0_deg;
    size_t gyroFlag = 0;                                                                 //用于防止一停车由于惯性没稳定就开始计时的flag
    QLength r = sysData->jsonVal["系统信息"]["车轮直径"].get<double>() / 2 * millimeter; //车轮半径
    QMass robotWeight = sysData->jsonVal["系统信息"]["机器重量"].get<double>() * kg;
    QSpeed lastRobotSpeed[2] = {0_mps, 0_mps};
    QAngularSpeed lastAngleSpeed[2] = {0_radps, 0_radps};
    QAngle angleRead[2] = {0_deg, 0_deg}; //0ENC 1GYRO
    while (true)
    {
        //传感器更新
        for (auto side = 0; side < 2; side++)
            _encNow[side] = getEnc(side);
        newEnc = {_encNow[0], _encNow[1]};
        tickEnc = newEnc - lastEnc;
        angleRead[0] = QAngle(((tickEnc[1] - tickEnc[0])) * sysData->wheelSpacing * degree); //编码器角度
        angleRead[1] = getGyro();
        _encAngle += angleRead[0];
        if ((tickEnc[0] != 0 || tickEnc[1] != 0)) //如果两个轮子都没转 机器动了 不积分角度 记录陀螺仪的累计误差
        {
            if (_isGyroReady)
            {
                if (angleRead[1] >= 0_deg)
                    angleRead[1] = (angleRead[1] - _gyroDriftAvg);
                else
                    angleRead[1] = (angleRead[1] * _gyroBalance - _gyroDriftAvg);
                _filterAngle += angleRead[1];
                _state.theta += angleRead[1];
            }
            gyroCalcTimer.clearHardMark(); //这里也要clean 不然会一直更新
            gyroCalc = 0_deg;
            gyroFlag = 0;
        }
        else
        {
            gyroFlag++;
            if (gyroFlag >= 100)
            {
                gyroCalcTimer.placeHardMark();
                gyroCalc += angleRead[1];
            }
            else
                gyroCalcTimer.clearHardMark();
            if (gyroCalcTimer.getDtFromHardMark() >= 10000_ms)
            {
                _gyroDriftAvg = gyroCalc / static_cast<double>(gyroCalcTimer.getDtFromHardMark().convert(millisecond));
                //logger->debug({"陀螺仪零飘率更新为:", std::to_string(_gyroDriftAvg), " 计算时间:", std::to_string(gyroCalcTimer.getDtFromHardMark())});
                gyroCalcTimer.clearHardMark();
                gyroCalc = 0_deg;
            }
        }

        for (auto side = 0; side < 2; side++)
        {
            _encSpeed[side] = tickEnc[side] * sysData->diameter / 10 * mps;                                            //单位 m/s
            _nowAccelSpeed[side] = QAcceleration(getTorque(side) / (r.convert(millimeter) * robotWeight.convert(kg))); //单位mps2 扭矩方式看着舒服
            //_nowAccelSpeed[side] = (_nowRobotSpeed[side] - lastRobotSpeed[side]); //单位 mm/s2
            _nowAngleSpeed[side] = angleRead[side] / 0.01_s;                                                  //单位°/s
            _nowAccelAngle[side] = _nowAngleSpeed[side].convert(radps) - lastAngleSpeed[side].convert(radps); //单位°/s2

            if (fabs(_encSpeed[side].convert(mps)) > _maxEncSpeed[side].convert(mps))
                _maxEncSpeed[side] = _encSpeed[side];
            if (fabs(_nowAccelSpeed[side].convert(mps2)) > _maxAccelSpeed[side].convert(mps2))
                _maxAccelSpeed[side] = _nowAccelSpeed[side];

            if (fabs(_nowAngleSpeed[side].convert(radps)) > _maxAngleSpeed[side].convert(radps))
                _maxAngleSpeed[side] = _nowAngleSpeed[side];
            if (fabs(_nowAccelAngle[side]) > _maxAccelAngle[side])
                _maxAccelAngle[side] = _nowAccelAngle[side];

            lastRobotSpeed[side] = _encSpeed[side];
            lastAngleSpeed[side] = _nowAngleSpeed[side];
        }

        if (angleRead[1] >= 4_deg)
            logger->error({"陀螺仪单次数值变化大于4,请检查陀螺仪"});
        mm = QLength(((tickEnc[1] + tickEnc[0]) / 2.0) * sysData->diameter * millimeter);
        //这里是弧度 不是角度
        _state.x += mm * std::cos(_state.theta.convert(radian));
        _state.y += mm * std::sin(_state.theta.convert(radian));
        _state.x = ncrapi::clamp(_state.x, sysData->odomMin, sysData->odomMax);
        _state.y = ncrapi::clamp(_state.y, sysData->odomMin, sysData->odomMax);
        lastEnc = newEnc; //注意线程安全问题
        pros::Task::delay_until(&now, 10);
    }
} // namespace ncrapi

} // namespace ncrapi
