#include "ncrapi/generic/roulette.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/system/sysUser.hpp"
#include "ncrapi/util/util.hpp"

namespace ncrapi {

Roulette::Roulette(const json &pragma, std::shared_ptr<Generic> shooter) : Generic("吸吐", pragma), _shooter(shooter)
{
    pros::delay(100);
    _holdingFlag = 1;
    logger->info({"自定义吸吐构造完毕"});
}
void Roulette::joyControl(std::shared_ptr<pros::Controller> joy, pros::controller_digital_e_t up, pros::controller_digital_e_t down)
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
    {
        if (_shooter != nullptr)
            _shooter->setMode(0); //打开悬停状态   ;
        holding();
    }
}
void Roulette::joyControl(std::shared_ptr<pros::Controller> joy, pros::controller_digital_e_t up, pros::controller_digital_e_t down, pros::controller_digital_e_t eatShit)
{
    if (_thisJoy == nullptr)
        _thisJoy = joy; //赋地址

    if (joy->get_digital(up) && joy->get_digital(down))
    {
        if (_shooter != nullptr)
            _shooter->setMode(5); //拉屎
    }
    else if (joy->get_digital(eatShit))
    {
        if (_shooter != nullptr)
            _shooter->setMode(6); //吃屎
    }
    else if (joy->get_digital(up))
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
    {
        if (_shooter != nullptr)
            _shooter->setMode(0); //打开悬停状态   ;
        holding();
    }
}

} // namespace ncrapi
