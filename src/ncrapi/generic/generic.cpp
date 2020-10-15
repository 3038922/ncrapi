#include "ncrapi/generic/generic.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/system/sysUser.hpp"

namespace ncrapi {

Generic::Generic(const std::string &name, const json &pragma) : _name(name)
{

    for (auto &[key, value] : pragma["马达"].items())
    {
        _motorEncNow.push_back(0.0);
        _motorEncLast.push_back(0.0);
        _motorList.push_back({key, value});
    }
    _holdVal = clamp<int>(pragma["参数"]["悬停值"].get<int>(), 0, 20, "悬停值");
    pros::delay(100);
    _nums = _motorList.size();
    if (_nums == 0)
        logger->error({"部件类马达数量不能为0"});
    _gearing = _motorList.begin()->getGearSpeed();
    sysData->addObj(this);
    resetEnc();
    logger->info({"部件:", _name, " 构造成功"});
}
int Generic::getHoldingVal()
{
    return _holdVal;
}
int Generic::getState()
{
    return _state;
}
void Generic::init(const int pwm)
{
    logger->info({_name, " 校准中"});
    size_t count = 0;
    while (count < 5 || isSafeMode())
    {
        Generic::set(pwm);
        if (_encAvgNow == _encAvgLast)
            count++;
        else
            count = 0;
        pros::delay(10);
    }
    Generic::set(0);
    resetEnc();
    _isInit = true;
    logger->info({_name, " 校准完毕"});
}

void Generic::set(const int pwm)
{
    _encAvgLast = _encAvgNow; //在外面调取的话 要把这个写前面
    _openLoopVal = clamp(pwm, -127, 127);
    if ((getCurrent() >= static_cast<int32_t>(2300 / _nums)) && std::abs(_openLoopVal) > _holdVal * 2)
    {
        _safeModeFlags++;
        if (_safeModeFlags > 30)
            _isSafeMode = true;
    }
    else
    {
        _safeModeFlags = 0;
        _isSafeMode = false;
    }
    if (_isSafeMode)
        _openLoopVal = _holdVal * _holdingFlag;
    for (auto &it : _motorList)
        it.move(_openLoopVal);
    _encAvgNow = getEnc();
}
void Generic::set(const uint32_t nums, const int pwm)
{
    _motorEncLast[nums] = _motorEncNow[nums]; //在外面调取的话 要把这个写前面
    _openLoopVal = clamp(pwm, -127, 127);
    if ((getCurrent(nums) >= 2300) && std::abs(_openLoopVal) > _holdVal * 2)
    {
        _safeModeFlags++;
        if (_safeModeFlags > 30)
            _isSafeMode = true;
    }
    else
    {
        _safeModeFlags = 0;
        _isSafeMode = false;
    }
    if (_isSafeMode)
        _openLoopVal = _holdVal * _holdingFlag;
    _motorList[nums].move(_openLoopVal);
    _motorEncNow[nums] = getEnc(nums);
}
void Generic::moveVoltage(const double vol)
{
    _openLoopVal = static_cast<int>(vol);
    if (isSafeMode())
        _openLoopVal = _holdVal * _holdingFlag;
    for (auto &it : _motorList)
        it.move_voltage(static_cast<int32_t>(_openLoopVal * 100));
}
void Generic::stop()
{
    for (auto &it : _motorList)
        it.move(0);
}
void Generic::moveAbsolute(const double position, const std::int32_t velocity)
{
    setMode(2);
    for (auto &it : _motorList)
        it.move_absolute(position, velocity);
}

void Generic::moveRelative(const double position, const std::int32_t velocity)
{
    setMode(2);
    for (auto &it : _motorList)
        it.move_relative(position, velocity);
}
void Generic::setTarget(const int target)
{
    setMode(2);
    _target = target;
}
void Generic::moveVelocity(const std::int32_t velocity)
{
    for (auto &it : _motorList)
        it.move_velocity(velocity);
}
void Generic::holding()
{
    if (_mode == 1)
    {
        set(127);
        _holdingFlag = _mode;
    }
    else if (_mode == -1)
    {
        set(-127);
        _holdingFlag = _mode;
    }
    else if (_mode == 0)
        set(_holdVal * _holdingFlag);

    else if (_mode == -2)
        set(clamp(static_cast<int>(_target - _encAvgNow), -127, 127));
    else
        moveVelocity(0);
}

void Generic::setMode(const int flag)
{
    _mode = flag;
}
int Generic::getMode()
{
    return _mode;
}

void Generic::joyControl(std::shared_ptr<pros::Controller> joy, pros::controller_digital_e_t up, pros::controller_digital_e_t down)
{
    if (_thisJoy == nullptr)
        _thisJoy = joy; //赋地址
    if (joy->get_digital(up))
    {
        set(127);
        setMode(0); //打开悬停状态
        _holdingFlag = 1;
    }
    else if (joy->get_digital(down))
    {
        set(-127);
        setMode(0); //打开悬停状态
        _holdingFlag = -1;
    }
    else
        holding();
}
void Generic::resetEnc()
{
    for (auto &it : _motorList)
        it.tare_position();
}

void Generic::setEnc(const double pos)
{
    for (auto &it : _motorList)
        it.set_zero_position(pos);
}
int Generic::getOpenLoopVal()
{
    return _openLoopVal;
}

double Generic::getEnc()
{
    double temp = 0;
    for (auto &it : _motorList)
        temp += it.get_position();
    return temp / _nums;
}
double Generic::getEnc(const uint32_t nums)
{
    return _motorList[nums].get_position();
}
double Generic::getSpeed()
{
    double temp = 0;
    for (auto &it : _motorList)
        temp += it.get_actual_velocity();
    return temp / _nums;
}
double Generic::getPower()
{
    double temp = 0;
    for (auto &it : _motorList)
        temp += it.get_power();
    return temp / _nums;
}
int32_t Generic::getCurrent()
{
    int32_t temp = 0;
    for (auto &it : _motorList)
        temp += it.get_current_draw();
    return temp / _nums;
}
int32_t Generic::getCurrent(const uint32_t nums)
{
    return _motorList[nums].get_current_draw();
}
int32_t Generic::getVoltage()
{
    int32_t temp = 0;
    for (auto &it : _motorList)
        temp += it.get_voltage();
    return temp / _nums;
}
double Generic::getEfficiency()
{
    double temp = 0;
    for (auto &it : _motorList)
        temp += it.get_efficiency();
    return temp / _nums;
}
double Generic::getTorque()
{
    double temp = 0;
    for (auto &it : _motorList)
        temp += it.get_torque();
    return temp / _nums;
}

double Generic::getTemperature()
{
    double temp = 0;
    for (auto &it : _motorList)
        temp += it.get_temperature();
    return temp / _nums;
}
bool Generic::isSafeMode()
{
    return _isSafeMode;
}
void Generic::resetAllSensors()
{
    resetEnc();
}

void Generic::setBrakeMode(pros::motor_brake_mode_e_t mode)
{
    logger->info({"设置:", _name, " 马达制动模式", std::to_string(mode)});
    for (auto &it : _motorList)
        it.set_brake_mode(mode);
}
void Generic::showSensor()
{
    userDisplay->ostr << _name << ":编码器:" << getEnc() << "温度:" << getTemperature()
                      << "电压:" << getVoltage() << "电流:" << getCurrent() << std::endl;
}
const std::string Generic::showName() const
{
    return _name;
}
void Generic::showDetailedInfo()
{
    userDisplay->ostr << _name << "pwm:" << _openLoopVal << "\n"
                      << "编码器:" << getEnc() << "速度:" << getSpeed() << "\n"
                      << "温度:" << getTemperature() << "电压:" << getVoltage() << "电流:" << getCurrent() << std::endl;
}
const std::vector<Motor> Generic::getMotorInfo() const
{
    return _motorList;
}
} // namespace ncrapi