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
                 {"升降右", {{"端口", 6}, {"正反", true}, {"齿轮", 1}}},
             }},
            {"参数",
             {
                 {"悬停值", 10},
             }},
        },
    },
    {
        "旋转器",
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
};

extern std::shared_ptr<pros::Controller> joy1;
extern std::shared_ptr<ncrapi::Chassis> chassis;
extern std::shared_ptr<ncrapi::Generic> lift;
extern std::shared_ptr<ncrapi::Generic> flipper;
//demo for nancy