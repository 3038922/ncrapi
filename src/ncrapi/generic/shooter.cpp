#include "ncrapi/generic/shooter.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/system/sysUser.hpp"
#include "ncrapi/util/util.hpp"

namespace ncrapi {
/*********联动式发射************/
Shooter::Shooter(const json &pragma) : Generic("发射器", pragma),
                                       _lineLow("巡线低", pragma["ADI"]),
                                       _lineHigh("巡线高", pragma["ADI"])
{
    pros::delay(100);
    _lineLowVal = clamp<int>(pragma["参数"]["巡线低阈值"].get<int>(), 300, 3000, "巡线低阈值");
    _lineHighVal = clamp<int>(pragma["参数"]["巡线高阈值"].get<int>(), 300, 3000, "巡线高阈值");
    //巡线低
    if (_lineLow.get_value() < _lineLowVal)
        logger->warning({"巡线低数据异常,模拟阈值:", std::to_string(_lineLowVal)});
    else
    {
        logger->info({"设置巡线低阈值:", std::to_string(_lineLowVal)});
    }
    //巡线高
    if (_lineHigh.get_value() < _lineHighVal)
        logger->warning({"巡线高数据异常,模拟阈值:", std::to_string(_lineHighVal)});
    else
    {
        logger->info({"设置巡线高阈值:", std::to_string(_lineHighVal)});
    }
    _holdingFlag = 1;
    logger->info({"联动式发射构造完毕"});
}
void Shooter::joyControl(std::shared_ptr<pros::Controller> joy, pros::controller_digital_e_t up,
                         pros::controller_digital_e_t down, pros::controller_digital_e_t mode)
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
    else if (joy->get_digital_new_press(mode)) //自动吸球模式判断
    {
        if (_isAutoLowMoveBall)
            _isAutoLowMoveBall = false;
        else
            _isAutoLowMoveBall = true;
    }
    else
        holding();
}

void Shooter::setAutoLowMoveBall(bool flag)
{
    _isAutoLowMoveBall = flag;
}

bool Shooter::isAutoLowMoveBall()
{
    return _isAutoLowMoveBall;
}
void Shooter::autoLowMoveBall()
{
    if (_lineLow.get_value() < _lineLowVal && _lineHigh.get_value() > _lineHighVal)
        set(127);
    else
        set(_holdVal * _holdingFlag);
}
void Shooter::holding()
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
        if (_isAutoLowMoveBall)
            autoLowMoveBall();
        else
            set(_holdVal * _holdingFlag);

    else if (_mode == -2)
        set(clamp(static_cast<int>(_target - _encAvgNow), -127, 127));
    else
        moveVelocity(0);
}
void Shooter::showSensor()
{
    userDisplay->ostr << _name << ": 编码器:" << getEnc() << " 温度:" << getTemperature() << " 速度:" << getSpeed() << '\n'
                      << "巡线下: " << _lineLow.get_value() << " 阈值: " << _lineLowVal << " 巡线上: " << _lineHigh.get_value() << " 阈值: " << _lineHighVal << std::endl;
}
void Shooter::showDetailedInfo()
{
    userDisplay->ostr << _name << "pwm:" << _openLoopVal << "\n"
                      << "编码器:" << getEnc() << "速度:" << getSpeed() << "\n"
                      << "温度:" << getTemperature() << "电压:" << getVoltage() << "电流:" << getCurrent() << "\n"
                      << "巡线下: " << _lineLow.get_value() << " 阈值: " << _lineLowVal << "\n"
                      << "巡线上: " << _lineHigh.get_value() << " 阈值: " << _lineHighVal << std::endl;
}

/*********分离式发射************/
SeparateShooter::SeparateShooter(const json &pragma) : Shooter(pragma)
{
    pros::delay(100);
    _holdingFlag = 1;
    logger->info({"分离式发射构造完毕"});
}

void SeparateShooter::joyControl(std::shared_ptr<pros::Controller> joy, pros::controller_digital_e_t up,
                                 pros::controller_digital_e_t down, pros::controller_digital_e_t mode)
{
    if (_thisJoy == nullptr)
        _thisJoy = joy; //赋地址
    if (joy->get_digital(up) && joy->get_digital(down))
    {
        set(127);
        setMode(0); //打开悬停状态
        _holdingFlag = 1;
    }
    else if (joy->get_digital(up))
    {
        set(0, 127); //第一层吸
        setMode(0);  //打开悬停状态
        _holdingFlag = 1;
    }
    else if (joy->get_digital(down))
    {
        set(-127);
        setMode(0); //打开悬停状态
        _holdingFlag = -1;
    }
    else if (joy->get_digital_new_press(mode)) //自动吸球模式判断
    {
        if (_isAutoLowMoveBall)
            _isAutoLowMoveBall = false;
        else
            _isAutoLowMoveBall = true;
    }
    else
        holding();
}
void SeparateShooter::autoLowMoveBall()
{
    if (_lineLow.get_value() < _lineLowVal && _lineHigh.get_value() > _lineHighVal)
        set(0, 127);
    else
        set(_holdVal * _holdingFlag);
}
void SeparateShooter::holding()
{
    if (_mode == 2)
    {
        set(0, 127);
        _holdingFlag = 1;
    }
    else if (_mode == 1)
    {
        set(127);
        _holdingFlag = 1;
    }
    else if (_mode == -1)
    {
        set(-127);
        _holdingFlag = -1;
    }
    else if (_mode == 0)
        if (_isAutoLowMoveBall)
            autoLowMoveBall();
        else
            set(0, _holdVal * _holdingFlag);
    else if (_mode == -2)
        set(clamp(static_cast<int>(_target - _encAvgNow), -127, 127));
    else
        moveVelocity(0);
}
/*********分球式发射************/
DistributionShooter::DistributionShooter(const json &pragma) : Shooter(pragma)
{
    pros::delay(100);
    _holdingFlag = 1;
    _middleMotorVal = clamp<int>(pragma["参数"]["中层马达转速"].get<int>(), -127, 127, "中层马达转速");
    logger->info({"分球式发射构造完毕"});
}
void DistributionShooter::joyControl(std::shared_ptr<pros::Controller> joy,
                                     pros::controller_digital_e_t up, pros::controller_digital_e_t down,
                                     pros::controller_digital_e_t mode, pros::controller_digital_e_t distribution)
{
    if (_thisJoy == nullptr)
        _thisJoy = joy; //赋地址
    if (joy->get_digital(up) && joy->get_digital(down))
    {
        set(127);
        setMode(0); //打开悬停状态
        _holdingFlag = 1;
    }
    else if (joy->get_digital(up))
    {

        if (_motorList.size() > 2) // 判断几马达的吸土
        {
            set(0, 127);
            set(1, 127);
        }
        else
            set(0, 127);
        setMode(0); //打开悬停状态
        _holdingFlag = 1;
    }
    else if (joy->get_digital(down))
    {
        set(-127);
        setMode(0); //打开悬停状态
        _holdingFlag = -1;
    }
    else if (joy->get_digital_new_press(mode)) //自动吸球模式判断
    {
        if (_isAutoLowMoveBall)
            _isAutoLowMoveBall = false;
        else
            _isAutoLowMoveBall = true;
    }
    else
        holding();
}
void DistributionShooter::autoLowMoveBall()
{
    if (_lineLow.get_value() < _lineLowVal && _lineHigh.get_value() > _lineHighVal)
    {
        if (_motorList.size() > 2) // 判断几马达的吸土
        {
            set(0, 127);
            set(1, _middleMotorVal);
            set(2, 127);
        }
        else
            set(127);
    }
    else
        set(_holdVal * _holdingFlag);
}
void DistributionShooter::holding()
{
    if (_mode == 6)
    {
        set(1, 127);
        set(0, -127);
        _holdingFlag = 1;
    }
    else if (_mode == 5)
    {
        set(1, -127);
        set(0, 127);
        _holdingFlag = 1;
    }
    else if (_mode == 4)
    {
        set(2, 127);
        _holdingFlag = 1;
    }
    else if (_mode == 3)
    {
        set(1, 127);
        _holdingFlag = 1;
    }
    else if (_mode == 2)
    {
        set(0, 127);
        _holdingFlag = 1;
    }
    else if (_mode == 1)
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
    {
        if (_isAutoLowMoveBall)
            autoLowMoveBall();
        else
            set(_holdVal * _holdingFlag);
    }

    else if (_mode == -2)
        set(clamp(static_cast<int>(_target - _encAvgNow), -127, 127));
    else
        moveVelocity(0);
}

} // namespace ncrapi
