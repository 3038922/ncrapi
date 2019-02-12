#pragma once
#include "ncrapi/ncrapi.hpp"
#define ROBOT_CAP          //盘子机
#define READ_SD_CARD false //是否从SD卡上读取 false 从这里读取参数 true 从SD卡上读取参数
const json userData = {
    {"json版本号", 1.0},
    {
        "系统信息",
        {
            {"机器人类型", "盘子机"},
            {"队伍编号", "35216B"},
            {"用户", "Nancy"},
        },
    },
    {
      "自动赛",
      {
        {"前场&后场", false},
        {"高旗&中旗", false},
        {"不开台", false},
      },
    },
    {
        "底盘",
        {
            {"马达",
             {
                 {"左前", {{"端口", 1}, {"正反", true}, {"齿轮", 1}}},
                 {"左后", {{"端口", 2}, {"正反", true}, {"齿轮", 1}}},
                 {"右前", {{"端口", 3}, {"正反", false}, {"齿轮", 1}}},
                 {"右后", {{"端口", 4}, {"正反", false}, {"齿轮", 1}}},
             }},
            {"参数",
             {
                 {"遥控器矫正", 10},
                 {"最大旋转速度", 127},
             }},
        },
    },
    {
        "升降",
        {
            {"马达",
             {
                 {"升降左", {{"端口", 5}, {"正反", false}, {"齿轮", 1}}},
                 //{"升降右", {{"端口", 6}, {"正反", true}, {"齿轮", 1}}},
             }},
            {"参数",
             {
                 {"悬停值", 10},
             }},
        },
    },
    {
        "夹子",
        {
            {"马达",
             {
                 {"夹子", {{"端口", 7}, {"正反", false}, {"齿轮", 1}}},
             }},
            {"参数",
             {
                 {"悬停值", 8},
             }},
        },
    },
    {
        "吸球器",
        {
            {"马达",
             {
                 {"吸球器", {{"端口", 8}, {"正反", false}, {"齿轮", 1}}},
             }},
            {"参数",
             {
                 {"悬停值", 8},
             }},
        },
    },
    {
        "射球器",
        {
            {"马达",
             {
                 {"射球器", {{"端口", 9}, {"正反", false}, {"齿轮", 1}}},
             }},
            {"参数",
             {
                 {"悬停值", 8},
             }},
        },
    },
};

extern pros::Controller *joy1;
extern pros::Controller *joy2;
extern ncrapi::Chassis *chassis;
extern ncrapi::Generic *lift;
extern ncrapi::Generic *claw;
extern ncrapi::Generic *ballintake;
extern ncrapi::Generic *catapult;
//demo for nancy
