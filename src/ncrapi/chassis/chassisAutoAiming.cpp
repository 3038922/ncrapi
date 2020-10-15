#include "ncrapi/chassis/chassisAutoAiming.hpp"
#include "userConfig/robotSet.hpp"

#include "ncrapi/system/sysUser.hpp"
#include "ncrapi/system/visionData.hpp"

namespace ncrapi {
ChassisAutoAiming::ChassisAutoAiming(const json &pragma) : ChassisOdom(pragma), _autoAimingPid("自瞄pid", pragma["PID参数"])
{
    _shootPosVal = clamp<double>(pragma["参数"]["左右矫正比例"].get<double>(), 0.1, 2.0, "左右矫正比例");
    _autoAimingPid.setTarget(0); //前馈常数设置跟PID目标值有关 先设置目标 再重置PID
    logger->info({"自瞄底盘 类构造成功"});
}
/**
     *自瞄算法 
     * @param data 瞄准数据类
     */
void ChassisAutoAiming::autoAimingCalc(const int speedMode[128])
{
    auto data = visionData->getData();
    if (data[0].signature != 0)
    {
        if (_isResetPid)
        {
            _autoAimingPid.reset();
            _isResetPid = false;
        }
        else
        {
            double angleError = (VISION_FOV_WIDTH / 2.0) - (data[0].x_middle_coord * _shootPosVal); //左右误差
                                                                                                    // double distanceError = (VISION_FOV_HEIGHT / 2.0) - (data.y_middle_coord);            //前后误差 主要无法解决现在是高旗还是中旗被识别
            if (fabs(angleError) <= 4)
                stop();
            else
                driveVector(0, static_cast<int>(_autoAimingPid.step(-angleError)), frSpeed);

        } //这里输出有问题啊 似乎是负数
    }
}
void ChassisAutoAiming::arcade(std::shared_ptr<pros::Controller> joy, pros::controller_analog_e_t verticalVal, pros::controller_analog_e_t horizontalVal, pros::controller_digital_e_t autoAimingBTN, const int speedMode[128])
{
    if (joy->get_digital(autoAimingBTN))
        autoAimingCalc(speedMode);
    else
    {
        _isResetPid = true;
        Chassis::arcade(joy, verticalVal, horizontalVal, speedMode);
    }
}
std::pair<QTime, double> ChassisAutoAiming::rotateAutoAiming(QAngle idegTarget)
{
    isStopPid(false); //不要停止PID
    const QAngle startAngle = _filterAngle;
    uint32_t prevWakeTime = pros::millis();
    bool isUseVisionPid = false; //标记是否启用视觉PID 防止重回陀螺仪PID
    while (idegTarget > 180_deg)
        idegTarget -= 360_deg;
    while (idegTarget <= -180_deg)
        idegTarget += 360_deg;
    _anglePid.setTarget(idegTarget.convert(degree)); //前馈常数设置跟PID目标值有关 先设置目标 再重置PID
    _anglePid.reset();
    _autoAimingPid.reset();
    bool atTarget = false;
    const QAngle atTargetAngle = 0.5_deg;        //允许误差的角度
    const QAngularSpeed threshold = 0.005_radps; //速度变化到多慢的时候跳出
    Timer atTargetTimer;
    const QTime timeoutPeriod = 50_ms;
    QAngle angleChange = {0_deg};
    while (!atTarget)
    {
        angleChange = _filterAngle - startAngle;
        auto data = visionData->getData();
        double errorVision = (VISION_FOV_WIDTH / 2.0) - (data[0].x_middle_coord * _shootPosVal);
        if (!isUseVisionPid) //如果距离目标大于5° 或者 没有检测到东西
        {
            if ((idegTarget - angleChange).abs() > 15_deg || data[0].signature == 0)
                driveVector(0, static_cast<int>(_anglePid.step(angleChange.convert(degree))), rotateSpeed); //这里输出有问题啊 似乎是负数
            else
                isUseVisionPid = true;
        }
        else
            driveVector(0, static_cast<int>(_autoAimingPid.step(-errorVision)));         //这里输出有问题啊 似乎是负数
        if ((idegTarget - angleChange).abs() <= atTargetAngle || fabs(errorVision) <= 4) //如果目标角度-当前角度小于等于允许误差角度 则break
            atTargetTimer.placeHardMark();
        else if (_nowAngleSpeed[0].abs() <= threshold && _nowAngleSpeed[1].abs() <= threshold) //如果当前角度-上次角度<=1 则break
            atTargetTimer.placeHardMark();
        else
            atTargetTimer.clearHardMark();
        if (atTargetTimer.getDtFromHardMark() >= timeoutPeriod)
            atTarget = true;
        pros::Task::delay_until(&prevWakeTime, 10);
    }
    Chassis::stop();
    return std::make_pair(atTargetTimer.getDtFromStart(), _autoAimingPid.getError());
}
void ChassisAutoAiming::driveToAngleAutoAiming(const QAngle iangle)
{
    rotateAutoAiming(iangle - getState().theta);
}

/**
     * 重新设定PID参数

     */
void ChassisAutoAiming::resetPid(std::string_view name)
{
    std::string mode;
    if (sysData->jsonVal["底盘"]["参数"]["模式"] == 0)
        mode = "模式0";
    else
        mode = "模式1";
    if (name == "前后pid")
        _distancePid.setAllPragmas(sysData->jsonVal["底盘"]["PID参数"][mode]["前后pid"]);
    else if (name == "左右pid")
        _anglePid.setAllPragmas(sysData->jsonVal["底盘"]["PID参数"][mode]["左右pid"]);
    else if (name == "自瞄pid")
        _autoAimingPid.setAllPragmas(sysData->jsonVal["底盘"]["PID参数"][mode]["自瞄pid"]);
    else
        logger->error({"pid名字录入错误"});
}
} // namespace ncrapi