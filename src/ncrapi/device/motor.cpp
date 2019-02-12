#include "ncrapi/device/motor.hpp"
#include "ncrapi/system/sysBase.hpp"
#include "pros/motors.hpp"
#include <iostream>

namespace ncrapi
{
Motor::Motor(const std::string &name, const int &port, const bool &isRevase, const int &gearset) : _port(port), _name(name),
                                                                                                   pros::Motor(port)
{
    init(clamp<int>(gearset, 0, 2, "马达齿轮"));
}
Motor::Motor(const std::string &name, const json &pragma) : _name(name),
                                                            _port(pragma["端口"].get<int>()),
                                                            pros::Motor(pragma["端口"].get<int>(), pragma["正反"].get<bool>())
{
    init(clamp<int>(pragma["齿轮"].get<int>(), 0, 2, "马达齿轮"));
}

/**
     *获取马达的名字     
     * @return std::string 返回马达的名字 
     */
std::string Motor::getName()
{
    return _name;
}
/**
     *获取马达的端口号     
     * @return std::string 返回马达的名字 
     */
int Motor::getPort()
{
    return _port;
}

/**
     * @获取齿轮中文名字
     * 
     * @return std::string 返回齿轮的中文名字
     */
std::string Motor::getGearName()
{
    size_t flag = get_gearing();
    if (flag == 0)
        return "红齿轮";
    else if (flag == 1)
        return "绿齿轮";
    else if (flag == 2)
        return "蓝齿轮";
    else
        return "齿轮获取错误,请检查马达";
}
/**
     *获取当前齿轮的速度 
     * @return int 范湖齿轮的速度 红齿轮100 绿齿轮200 蓝齿轮600
     */
int Motor::getGearSpeed()
{
    return _speed;
}

void Motor::init(int gear)
{
    if (gear == 0)
    {
        set_gearing(pros::E_MOTOR_GEARSET_36);
        _speed = 100;
    }
    else if (gear == 1)
    {
        set_gearing(pros::E_MOTOR_GEARSET_18);
        _speed = 200;
    }
    else
    {
        set_gearing(pros::E_MOTOR_GEARSET_06);
        _speed = 600;
    }
    size_t flag = get_gearing();
    set_encoder_units(pros::E_MOTOR_ENCODER_DEGREES); //设置编码器模式
    if (flag >= 0 && flag <= 2)
        std::cout << _name << ":" << _port << " " << is_reversed() << " " << flag << " 构造成功" << std::endl;
    else
        sysData->addDebugData({_name, ":", std::to_string(_port), "构造失败,请检查马达"});
}
} // namespace ncrapi