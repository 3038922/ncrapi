#include "ncrapi/chassis/chassis.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/system/sysBase.hpp"
#include "ncrapi/userDisplay/userDisplay.hpp"
#include <algorithm>
#include <cmath>

namespace ncrapi
{

Chassis::Chassis(const json &pragma) : _name("底盘")
{
    for (auto &[key, value] : pragma["马达"].items())
        _motorList.push_back({key, value});

    _joyThreshold = clamp<int>(pragma["参数"]["遥控器矫正"].get<int>(), 0, 20, "遥控器矫正");
    _maxRotateSpd = clamp<int>(pragma["参数"]["最大旋转速度"].get<int>(), 60, 127, "最大旋转速度");
    pros::delay(100);
    _sideNums = _motorList.size() / 2;
    if (_motorList.size() % 2 != 0 || _sideNums == 0)
        logger->error({"底盘类马达数量输入错误!半边马达数量: ", std::to_string(_sideNums)});
    _gearing = _motorList.begin()->getGearSpeed();
    sysData->addObj(this);
    resetEnc();
    setMode(clamp<int>(pragma["参数"]["模式"].get<int>(), 0, 1, "底盘模式"));
    logger->info({"底盘基类构造成功"});
}
void Chassis::set(const int left, const int right)
{
    if (_mode != 0)
        setMode(0);
    _pwm[0] = left;
    _pwm[1] = right;
    for (size_t i = 0; i < _sideNums; i++)
    {
        _motorList[i].move(_pwm[0]);
        _motorList[i + _sideNums].move(_pwm[1]);
    }
    if ((getTemperature(0) >= 50 || getTemperature(1) >= 50) && _timerTemp.getDtFromMark() >= 15000)
    {
        logger->error({"底盘马达过热!"});
        _timerTemp.placeMark();
    }
}
/**
    设定电机的速度。
    *
    *
    *当达到错误状态时，此函数使用以下errno值：
    *EACCES  -另一个资源目前正在尝试访问该端口。
    *
    *\ param left right 新电机速度为 - +  - 100，+ -200或+ -600，具体取决于电机的齿轮组
    *
    *\如果操作成功则返回1或如果操作则返回PROS_ERR
    *失败，设置错误。
     */
void Chassis::moveVelocity(const std::int32_t left, const std::int32_t right)
{
    if (_mode != 1)
        setMode(1);
    for (size_t i = 0; i < _sideNums; i++)
    {
        _motorList[i].move_velocity(left);
        _motorList[i + _sideNums].move_velocity(right);
    }
    if ((getTemperature(0) >= 50 || getTemperature(1) >= 50) && _timerTemp.getDtFromMark() >= 15000)
    {
        logger->error({"底盘马达过热!"});
        _timerTemp.placeMark();
    }
}

void Chassis::moveRelative(const double leftPos, const double rightPos, const std::int32_t velocity)
{
    for (size_t i = 0; i < _sideNums; i++)
    {
        _motorList[i].move_relative(leftPos, velocity);
        _motorList[i + _sideNums].move_relative(rightPos, velocity);
    }
}
/**
     * 普通前进后退 开环控制
     * @param pwm  前进+ 后退- 范围:+-127
     */
void Chassis::forward(const int pwm, const int speedMode[128])
{
    int sign = copySign(pwm);
    set(speedMode[abs(pwm)] * sign, speedMode[abs(pwm)] * sign);
}
/**
     * 使用速度环控制底盘前进后退
     * @param velocity 设定的速度 上限红齿轮+-100 绿齿轮+-200 蓝齿轮+-600
     */
void Chassis::forwardVelocity(const int32_t velocity)
{
    moveVelocity(velocity, velocity);
}

void Chassis::forwardRelative(const double pos, const std::int32_t velocity)
{
    moveRelative(pos, pos, velocity);
}
/**
     * 普通旋转 开环控制
     * @param pwm 左转+ 右转- 范围:+-127
     */
void Chassis::rotate(const int pwm, const int speedMode[128])
{
    int sign = copySign(pwm);
    set(-speedMode[abs(pwm)] * sign, speedMode[abs(pwm)] * sign);
}
/**
     * 使用速度环控制底盘左转右转 左转为+ 右转为-
     * @param velocity 设定的速度 上限红齿轮+-100 绿齿轮+-200 蓝齿轮+-600
     */
void Chassis::rotateVelocity(const int32_t velocity)
{
    moveVelocity(-velocity, velocity);
}
void Chassis::rotateReative(const double pos, const std::int32_t velocity)
{
    moveRelative(pos, -pos, velocity);
}

/**
     * 底盘马达停转
     */
void Chassis::stop()
{
    if (_mode == 0)
        set(0, 0);
    else
        moveVelocity(0, 0);
}
/**
     * 矢量控制 开弧线
     * @param distPwm  前进的力度 前进+ 后退-
     * @param anglePwm 弧线的力度 左开+ 右开-
     * 例:    chassis.driveVector(127, 80);
     */
void Chassis::driveVector(const int distPwm, const int anglePwm, const int speedMode[128]) //开弧线
{
    int left = clamp(distPwm - anglePwm, -_spdRange, _spdRange);
    int right = clamp(distPwm + anglePwm, -_spdRange, _spdRange);
    if (_mode == 0)
        set(speedMode[abs(left)] * copySign(left), speedMode[abs(right)] * copySign(right));
    else
    {
        double flag = _gearing / 127.0;
        moveVelocity(left * flag, right * flag);
    }
}

/**
     * 遥控模块 单摇杆双摇杆都用这个
     * @param verticalVal     前后通道
     * @param horizontalVal    左右通道
     * @param threshold 遥控器矫正阀值
     */
void Chassis::arcade(std::shared_ptr<pros::Controller> joy, pros::controller_analog_e_t verticalVal, pros::controller_analog_e_t horizontalVal, const int speedMode[128])
{

    int x = joy->get_analog(verticalVal);
    int y = -joy->get_analog(horizontalVal);
    if (abs(x) < _joyThreshold)
        x = 0;
    if (abs(y) < _joyThreshold)
        y = 0;
    y = clamp(y, -_maxRotateSpd, _maxRotateSpd);
    driveVector(x, y, speedMode);
}
void Chassis::tank(std::shared_ptr<pros::Controller> joy, pros::controller_analog_e_t left, pros::controller_analog_e_t right, const int threshold)
{
    int32_t l = joy->get_analog(left);
    int32_t r = joy->get_analog(right);
    if (abs(l) < threshold)
        l = 0;
    if (abs(r) < threshold)
        r = 0;
    set(l, r);
}

/**
     * 重置底盘所有马达编码器
     */
void Chassis::resetEnc()
{
    for (auto &it : _motorList)
        it.tare_position();
}

/**
     * 重置底盘相关传感器
     */
void Chassis::resetAllSensors()
{
    resetEnc();
}
/**
 * @获取左边或者右边编码器值
 * @param side 0左边 1右边
 * @return double 返回左边或者右边编码器值
 */
double Chassis::getEnc(const bool side)
{
    size_t i = 0;
    if (side == 1)
        i = _sideNums;
    return _motorList[i].get_position();
}
/**
     * 获取电机当前速度  
     * @param side  0左边 1右边
     * @return double 返回左边或者右边编码器值
     */
double Chassis::getSpeed(const bool side)
{
    size_t i = 0;
    if (side == 1)
        i = _sideNums;
    return _motorList[i].get_actual_velocity();
}
void Chassis::setMode(const int mode)
{
    _mode = mode;
    if (_mode == 0)
    {
        _spdRange = 127;
        _maxRotateSpd = sysData->jsonVal["底盘"]["参数"]["最大旋转速度"].get<int>();
        logger->debug({"底盘切换为开环控制"});
    }
    else
    {
        _spdRange = _gearing;
        _maxRotateSpd = sysData->jsonVal["底盘"]["参数"]["最大旋转速度"].get<int>() * _gearing / 127;
        logger->debug({"底盘切换为V5速度环控制"});
    }
}

/**
 * 获取左边或者右边的温度
 * @param side 左边0 右边1
 * @return const double 返回左边或者右边的温度
 */
const double Chassis::getTemperature(const bool side) const
{
    size_t flag = 0;
    if (side == 1)
        flag = _sideNums;
    double sum = 0;
    for (size_t i = 0; i < _sideNums; i++)
        sum += _motorList[i + flag].get_temperature();
    return sum / _sideNums;
}
/**
 * 获取左边或者右边的电压
 * @param side 左边0 右边1
 * @return int32_t 返回左边或者右边的电压
 */
const int32_t Chassis::getVoltage(const bool side) const
{
    size_t flag = 0;
    if (side == 1)
        flag = _sideNums;
    double sum = 0;
    for (size_t i = 0; i < _sideNums; i++)
        sum += _motorList[i + flag].get_voltage();
    return sum / static_cast<int32_t>(_sideNums); //注意这里有坑
}
const int32_t Chassis::getCurrent(const bool side) const
{
    size_t flag = 0;
    if (side == 1)
        flag = _sideNums;
    double sum = 0;
    for (size_t i = 0; i < _sideNums; i++)
        sum += _motorList[i + flag].get_voltage();
    return sum / static_cast<int32_t>(_sideNums); //注意这里有坑
}

/**
     * 显示传感器数据到屏幕 ostringstream ostr流
     */
void Chassis::showSensor()
{
    userDisplay->ostr << "左底盘: 编码器:" << getEnc(0) << " 温度:" << getTemperature(0) << "电压:" << getVoltage(0) << "电流:" << getCurrent(0) << "\n"
                      << "右底盘: 编码器:" << getEnc(1) << " 温度:" << getTemperature(1) << "电压:" << getVoltage(1) << "电流:" << getCurrent(1) << std::endl;
}
const std::string Chassis::showName() const
{
    return _name;
}
void Chassis::showDetailedInfo()
{
    userDisplay->ostr << "左底盘pwm:" << _pwm[0] << "\n"
                      << "编码器:" << getEnc(0) << "速度:" << getSpeed(0) << "\n"
                      << "温度:" << getTemperature(0) << "电压:" << getVoltage(0) << "电流:" << getCurrent(0) << "\n"
                      << "右底盘pwm:" << _pwm[1] << "\n"
                      << "编码器 : " << getEnc(1) << "速度:" << getSpeed(1) << "\n"
                      << "温度 : " << getTemperature(1) << "电压 : " << getVoltage(1) << "电流 : " << getCurrent(1) << std::endl;
}

} // namespace ncrapi
