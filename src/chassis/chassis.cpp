#include "ncrapi/chassis/chassis.hpp"
namespace ncrapi
{

Chassis::Chassis(const std::vector<pros::Motor> &motorList) : _motorList(motorList), _name("底盘")
{
    _sideNums = _motorList.size() / 2;
    if (_sideNums % 2 != 0 || _sideNums == 0)
        std::cerr << "chassis side nums error" << std::endl;
    pros::delay(100);
    resetEnc();
    sysData->addObj(this);
}
void Chassis::set(const int left, const int right)
{
    _pwm[0] = left;
    _pwm[1] = right;
    for (size_t i = 0; i < _sideNums; i++)
        _motorList[i].move(_pwm[0]);
    for (size_t i = _sideNums; i < _motorList.size(); i++)
        _motorList[i].move(_pwm[1]);
}
void Chassis::moveVelocity(const std::int32_t left, const std::int32_t right)
{
    for (size_t i = 0; i < _sideNums; i++)
        _motorList[i].move_velocity(left);
    for (size_t i = _sideNums; i < _motorList.size(); i++)
        _motorList[i].move_velocity(right);
}
void Chassis::moveRelative(const double leftPos, const double rightPos, const std::int32_t velocity)
{
    for (size_t i = 0; i < _sideNums; i++)
        _motorList[i].move_relative(leftPos, velocity);
    for (size_t i = _sideNums; i < _motorList.size(); i++)
        _motorList[i].move_relative(rightPos, velocity);
}
void Chassis::forward(const int pwm)
{
    set(pwm, pwm);
}
void Chassis::forwardVelocity(const int32_t velocity)
{
    moveVelocity(velocity, velocity);
}
void Chassis::forwardRelative(const double pos, const std::int32_t velocity)
{
    moveRelative(pos, pos, velocity);
}
void Chassis::rotate(const int pwm)
{
    set(pwm, -pwm);
}
void Chassis::rotateVelocity(const int32_t velocity)
{
    moveVelocity(velocity, -velocity);
}
void Chassis::rotateReative(const double pos, const std::int32_t velocity)
{
    moveRelative(pos, -pos, velocity);
}
void Chassis::stop()
{
    set(0, 0);
}
void Chassis::driveVector(const int distPwm, const int anglePwm, const int *speedMode) //开弧线
{
    int left = distPwm + anglePwm;
    int right = distPwm - anglePwm;
    int leftSign = static_cast<int>(copysign(1.0, static_cast<float>(left)));
    int rightSign = static_cast<int>(copysign(1.0, static_cast<float>(right)));
    if (abs(left) > 127)
        left = 127 * leftSign;
    if (abs(right) > 127)
        right = 127 * rightSign;
    set(speedMode[abs(left)] * leftSign, speedMode[abs(right)] * rightSign);
}
void Chassis::arcade(pros::Controller *joy, pros::controller_analog_e_t verticalVal, pros::controller_analog_e_t horizontalVal, const int rotateMaxSpeed, const int threshold, const int *speedMode)
{
    int32_t x = joy->get_analog(verticalVal);
    int32_t y = joy->get_analog(horizontalVal);
    if (abs(x) < threshold)
        x = 0;
    if (abs(y) < threshold)
        y = 0;
    else if (abs(y) >= rotateMaxSpeed)
        y = static_cast<int>(copysign(rotateMaxSpeed, static_cast<float>(y)));
    driveVector(x, y, speedMode);
}
void Chassis::tank(pros::Controller *joy, pros::controller_analog_e_t left, pros::controller_analog_e_t right, const int threshold)
{
    int32_t l = joy->get_analog(left);
    int32_t r = joy->get_analog(right);
    if (abs(l) < threshold)
        l = 0;
    if (abs(r) < threshold)
        r = 0;
    set(l, r);
}
void Chassis::resetEnc()
{
    for (auto &it : _motorList)
        it.tare_position();
}
void Chassis::resetAllSensors()
{
    resetEnc();
}
double Chassis::getEnc(const bool side)
{
    size_t i = 0;
    size_t max = _sideNums;
    if (side == 1)
    {
        i = _sideNums;
        max = _motorList.size();
    }
    double sum = 0;
    for (; i < max; i++)
        sum += _motorList[i].get_position();
    return sum / _sideNums;
}
double Chassis::getSpeed(const bool side)
{
    size_t i = 0;
    size_t max = _sideNums;
    if (side == 1)
    {
        i = _sideNums;
        max = _motorList.size();
    }
    double sum = 0;
    for (; i < max; i++)
        sum += _motorList[i].get_actual_velocity();
    return sum / _sideNums;
}
const double Chassis::getTemperature(const bool side) const
{
    size_t i = 0;
    size_t max = _sideNums;
    if (side == 1)
    {
        i = _sideNums;
        max = _motorList.size();
    }
    double sum = 0;
    for (; i < max; i++)
        sum += _motorList[i].get_temperature();
    return sum / _sideNums;
}
const int32_t Chassis::getVoltage(const bool side) const
{
    size_t i = 0;
    size_t max = _sideNums;
    if (side == 1)
    {
        i = _sideNums;
        max = _motorList.size();
    }
    int32_t sum = 0;
    for (; i < max; i++)
        sum += _motorList[i].get_voltage();

    return sum / static_cast<int32_t>(_sideNums); //注意这里有坑
}

const int32_t Chassis::getCurrent(const bool side) const
{
    size_t i = 0;
    size_t max = _sideNums;
    if (side == 1)
    {
        i = _sideNums;
        max = _motorList.size();
    }
    int32_t sum = 0;
    for (; i < max; i++)
        sum += _motorList[i].get_current_draw();
    return sum / static_cast<int32_t>(_sideNums); //注意这里有坑
}

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