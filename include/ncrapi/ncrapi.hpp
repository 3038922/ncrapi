#pragma once
#include "ncrapi/advanced/visionData.hpp"
#include "ncrapi/robotParts/chassis.hpp"
#include "ncrapi/robotParts/roulette.hpp"
#include "ncrapi/robotParts/shooter.hpp"
#include "ncrapi/system/ncrSystem.hpp"

#define PROS_VERSION_MAJOR 3
#define PROS_VERSION_MINOR 3
#define PROS_VERSION_PATCH 1
#define PROS_VERSION_STRING "3.3.1"

#define PROS_ERR (INT32_MAX)
#define PROS_ERR_F (INFINITY)
#define NCR_VERSION_STRING "10.2.0"

//通用部件
extern std::shared_ptr<pros::Controller> joy1;
extern std::shared_ptr<pros::Controller> joy2;
extern std::shared_ptr<ncrapi::Generic> roulette; //吸吐
extern std::shared_ptr<ncrapi::Generic> shooter;  //发射器
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
void skillAuto(const json &jsonVal, ncrapi::Timer &autoTimer);
void userauto(const json &jsonVal, ncrapi::Timer &autoTimer);
void customTest();