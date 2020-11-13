#pragma once
#include "ncrapi/i18n/chinese.hpp"
#include "ncrapi/system/json.hpp"
const json userData = {
    {I18N_JSON_VER, 0.2},
    {
        I18N_SYSTEM_INFO,
        {
            {I18N_ROBOT_TYPE, "射球机"},
            {I18N_TEAM_NUM, "xxxxA"},
            {I18N_USER, "user"},
        },
    },
    {
        I18N_AUTO,
        {
            {I18N_AUTO "&" I18N_SKILL_AUTO, false},
            {I18N_RED_ALLIANCE "&" I18N_BLUE_ALLIANCE, true},
            {"前场&后场", false},
            {"放中间&不放", false},
            {"放X中&不放", false},
            {"放Y中&不放", false},
            {"启用视觉&不启用视觉", false},
        },
    },
    {
        I18N_VISION_INFO,
        {
            {I18N_VISION_FRONT, {
                                    {I18N_PORT, 17},
                                    {I18N_EXPOSURE, 20},
                                    {I18N_MAX_HIGH, 75},
                                    {I18N_MAX_WIDTH, 65},
                                    {I18N_DATA, 0},
                                    {I18N_COLOR_RANGE, {
                                                           {I18N_RED, {8.8, 10.0, 11.0, 10.0, 10.0, 6.9}},
                                                           {I18N_BLUE, {9.5, 11.0, 11.0, 7.0, 7.1, 11.0}},
                                                           {I18N_GREEN, {11.0, 9.0, 8.0, 8.3, 10.0, 5.0}},
                                                       }},
                                }},
        },
    },
    {
        "底盘",
        {
            {"马达", {
                         {"左前", {{"端口", 1}, {"正反", true}, {"齿轮", 1}}},
                         {"左后", {{"端口", 2}, {"正反", false}, {"齿轮", 1}}},
                         {"右前", {{"端口", 3}, {"正反", false}, {"齿轮", 1}}},
                         {"右后", {{"端口", 4}, {"正反", true}, {"齿轮", 1}}},
                     }},
            {"参数", {
                         {I18N_GEAR_RATIO, 1.0},
                         {"遥控器矫正", 10},
                         {"最大旋转速度", 127},

                     }},
        },
    },
    {
        "发射器",
        {
            {"马达", {
                         {"发射L", {{"端口", 5}, {"正反", false}, {"齿轮", 1}}},
                         {"发射H", {{"端口", 6}, {"正反", true}, {"齿轮", 1}}},
                     }},
            {"参数", {
                         {"悬停值", 5},
                     }},
        },
    },
    {
        "吸吐",
        {
            {"马达", {
                         {"吸吐L", {{"端口", 7}, {"正反", false}, {"齿轮", 1}}},
                         {"吸吐R", {{"端口", 8}, {"正反", true}, {"齿轮", 1}}},
                     }},
            {"参数", {
                         {"悬停值", 0},
                     }},
        },
    },

};
