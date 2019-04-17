#pragma once
#include "ncrapi/ncrapi.hpp"
#define ROBOT_CAP          //盘子机
#define READ_SD_CARD false //是否从SD卡上读取 false 从这里读取参数 true 从SD卡上读取参数
const json userData = {
    {"json版本号", 1.1},
    {
        "系统信息",
        {
            {"机器人类型", "全能机"},
            {"队伍编号", "35216B"},
            {"用户", "Nancy"},
            {"机器人长度", 457},
            {"机器人宽度", 365},
            {"赛场边长", 3440},
            {"轮间距", 300.0},
            {"车轮直径", 104},
            {"机器重量", 8.47},
        },
    },
    {
        "自动赛",
        {
            {"红方&蓝方", false},
            {"前场&后场", false},
            {"开台&不开台", false},
        },
    },
    {
        "底盘",
        {
            {"马达",
             {
                 {"左前", {{"端口", 7}, {"正反", false}, {"齿轮", 1}}},
                 {"左后", {{"端口", 8}, {"正反", false}, {"齿轮", 1}}},
                 {"右前", {{"端口", 9}, {"正反", true}, {"齿轮", 1}}},
                 {"右后", {{"端口", 10}, {"正反", true}, {"齿轮", 1}}},
             }},
            {"参数",
             {
                 {"遥控器矫正", 10},
                 {"最大旋转速度", 127},
                 {"模式", 0},
             }},
        },
    },
    {
        "弹射",
        {
            {"马达",
             {
                 {"弹射", {{"端口", 19}, {"正反", false}, {"齿轮", 1}}},
             }},
            {"参数",
             {
                 {"悬停值", 15},
             }},
        },
    },
    {
        "升降",
        {
            {"马达",
             {
                 {"升降", {{"端口", 20}, {"正反", false}, {"齿轮", 1}}},
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
                 {"夹子", {{"端口", 12}, {"正反", true}, {"齿轮", 1}}},
             }},
            {"参数",
             {
                 {"悬停值", 8},
             }},
        },
    },
    {
        "吸吐",
        {
            {"马达",
             {
                 {"吸吐", {{"端口", 19}, {"正反", false}, {"齿轮", 1}}},
             }},
            {"参数",
             {
                 {"悬停值", 0},
             }},
        },
    },
};

extern std::shared_ptr<pros::Controller> joy1;
extern std::shared_ptr<ncrapi::Chassis> chassis;
extern std::shared_ptr<ncrapi::Generic> lift;
extern std::shared_ptr<ncrapi::Generic> catapule;   //弹射
extern std::shared_ptr<ncrapi::Generic> ballintake; //吸吐
extern std::shared_ptr<ncrapi::Generic> cap;        //夹子

//demo for nancy
