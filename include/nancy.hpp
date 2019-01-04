#pragma once
#include "ncrapi/ncrapi.hpp"
#include <cstdint>
#include <string>
#include <valarray>

//DEMO FOR NANCY
//用户设置
const std::string robotInfo = "盘子机 Nancy 35216B";
//端口配置
//马达
const uint8_t LF = 1;
const bool LF_REVERSE = 1;
const uint8_t LB = 2;
const bool LB_REVERSE = 1;
const uint8_t RF = 3;
const bool RF_REVERSE = 0;
const uint8_t RB = 4;
const bool RB_REVERSE = 0;

const uint8_t LIFT_LEFT = 5;
const bool LIFT_LEFT_REVERSE = 0;
const uint8_t LIFT_RIGHT = 6;
const bool LIFT_RIGHT_REVERSE = 1;
const uint8_t FLIPPER = 7;
const bool FLIPPER_REVERSE = 0;

const int JOY_THRESHOLD = 10; //遥控器矫正阀值
const int ROTATE_SPEED = 127; //底盘最大旋转速度

extern pros::Controller *joy1;
extern pros::Controller *joy2;
extern ncrapi::Chassis *chassis;
extern ncrapi::Generic *lift;
extern ncrapi::Generic *flipper;
//demo for nancy