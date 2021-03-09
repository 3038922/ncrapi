#pragma once
#include "ncrapi/advanced/distanceSystem.hpp"
#include "ncrapi/advanced/visionSystem.hpp"
#include "ncrapi/robotParts/chassis.hpp"
#include "ncrapi/robotParts/shooter.hpp"
#include "ncrapi/system/ncrSystem.hpp"
#include "ncrapi/userDisplay/userDisplay.hpp"
#include "userConfig.hpp"

#define PROS_VERSION_MAJOR 3
#define PROS_VERSION_MINOR 4
#define PROS_VERSION_PATCH 0
#define PROS_VERSION_STRING "3.4.0"

#define PROS_ERR (INT32_MAX)
#define PROS_ERR_F (INFINITY)
#define NCR_VERSION_STRING "10.6.8"
/*******TASK******
里程计 TASK_PRIORITY_DEFAULT+3
自动赛 TASK_PRIORITY_DEFAULT+2
视觉 TASK_PRIORITY_DEFAULT+1
********TASK******/
//通用部件
//通用部件
extern std::shared_ptr<pros::Controller> joy1;
extern std::shared_ptr<pros::Controller> joy2;
extern std::shared_ptr<ncrapi::Generic> roulette; //吸吐
extern std::shared_ptr<ncrapi::Generic> shooter;  //发射器

extern pros::Task *autoTask;
/**
 * @brief 自动赛线程
 * 
 * @param para 传递参数
 */
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
void skillAuto(const json &jsonVal, ncrapi::Timer &autoTimer);
void userauto(const json &jsonVal, ncrapi::Timer &autoTimer);
void customTest();