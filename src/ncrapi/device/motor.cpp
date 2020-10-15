#include "ncrapi/device/motor.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/system/sysUser.hpp"

namespace ncrapi {
Motor::Motor(const std::string &name, const int &port, const bool &isReverse, const int &gearset) : _name(name),
                                                                                                    pros::Motor(port)
{
    sysData->i2cCheck(port, _name);
    init(clamp<int>(gearset, 0, 2, "马达齿轮"));
}
Motor::Motor(const std::string &name, const json &pragma) : _name(name),
                                                            pros::Motor(pragma["端口"].get<int>(), pragma["正反"].get<bool>())
{
    sysData->i2cCheck(pragma["端口"].get<int>(), _name);
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
    pros::motor_gearset_e_t flag = get_gearing();
    set_encoder_units(pros::E_MOTOR_ENCODER_DEGREES); //设置编码器模式
    if (flag <= 2)
        logger->info({_name, ":", std::to_string(get_port()), " ", std::to_string(is_reversed()), " ", std::to_string(flag), " 构造成功"});
    else
        logger->error({_name, ":", std::to_string(get_port()), "构造失败,请检查马达"});
}
} // namespace ncrapi