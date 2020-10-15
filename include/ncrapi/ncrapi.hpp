#pragma once
#include "ncrapi/chassis/chassisAutoAiming.hpp"
#include "ncrapi/generic/roulette.hpp"
#include "ncrapi/generic/shooter.hpp"
#include "ncrapi/system/sysUser.hpp"
#include "ncrapi/system/visionData.hpp"

#define PROS_VERSION_MAJOR 3
#define PROS_VERSION_MINOR 3
#define PROS_VERSION_PATCH 0
#define PROS_VERSION_STRING "3.3.0"

#define PROS_ERR (INT32_MAX)
#define PROS_ERR_F (INFINITY)
#define NCR_VERSION_STRING "10.1.2"

//通用部件
extern std::shared_ptr<pros::Controller> joy1;
extern std::shared_ptr<pros::Controller> joy2;
extern std::shared_ptr<ncrapi::ChassisOdom> chassis;     //底盘
extern std::shared_ptr<ncrapi::Roulette> roulette;       //吸吐
extern std::shared_ptr<ncrapi::SeparateShooter> shooter; //分离式发射器

extern pros::Task *autoTask;

static void taskAuto(void *para)
{
    uint32_t now = pros::millis();
    while (true)
    {
        roulette->holding();
        shooter->holding();
        pros::Task::delay_until(&now, 10);
    }
}
/**
  * 用于机器人姿态计算的多线程启动函数
 * @param para NULL
 */
static void taskOdom(void *para)
{
    chassis->odomLoop();
}
void skillAuto(const json &jsonVal, ncrapi::Timer &autoTimer);
void userauto(const json &jsonVal, ncrapi::Timer &autoTimer);
void customTest();