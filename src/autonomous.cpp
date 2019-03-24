/**
 * @Author: 陈昱安
 * @Date:   2018-09-16T00:20:58+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   yan
 * @Last modified time: 2018-10-16T12:51:39+08:00
 */

#include "main.hpp"
#include <cmath>
/**
 * 自动赛
 */

void autonomous()
{
    ncrapi::Timer autonomousTimer;
    autonomousTimer.placeMark();
    if (pros::competition::get_status() == COMPETITION_CONNECTED)
    {
        userDisplay->delTasks();
        userDisplay->delObjs();
        userDisplay->createUserObj(OBJ_AUTONOMOUS, true, "自动赛", nullptr, "自动赛进行中");
    }
    logger->debug({"自动赛开始:", std::to_string(autonomousTimer.getDtFromMark())});

    int autoFlag = -1;
    //DEMO FOR NANCY
    if (sysData->jsonVal["自动赛"]["红方&蓝方"] == 0) //红方自动
        autoFlag = 1;

    if (!sysData->jsonVal["自动赛"]["前场&后场"]) //前场
    {
        // chassis moving distance in inch
        // distance(inch) /4 (inch wheel diameter)/ pi * 360 degree
        chassis->forwardRelative(-12 * 360 / 4 / M_PI, 100);      //后退一个方格 12inch
        catapule->moveRelative(360, 100);                         //射高旗
        chassis->forwardRelative(12 * 360 / 4 / M_PI, 100);       //前进1个方格
        chassis->rotateReative(-790 * 360 / 900 * autoFlag, 100); //右转90
        ballintake->moveRelative(365 * 5, 100);                   //吸球器启动
        chassis->forwardRelative(-18 * 360 / 4 / M_PI, 100);      //后退1.5个方格吸球
        chassis->forwardRelative(18 * 360 / 4 / M_PI, 100);       //前进1.5个方格
        chassis->rotateReative(790 * 360 / 900 * autoFlag, 100);  //左转90
        chassis->forwardRelative(12 * 360 / 4 / M_PI, 100);       //前进1个方格
        catapule->moveRelative(360, 100);                         //射中旗
        chassis->forwardRelative(6 * 360 / 4 / M_PI, 100);        //前进0.5个方格 撞低旗

        if (!sysData->jsonVal["自动赛"]["开台&不开台"]) // 开台
        {
            chassis->forwardRelative(-42 * 360 / 4 / M_PI, 100);      //后退3.5个方格
            chassis->rotateReative(790 * 360 / 900 * autoFlag, 100);  //左转90
            chassis->forwardRelative(2.5 * 12 * 360 / 4 / M_PI, 100); //前进2.5个方格 停泊
        }
        else // 不开台
        {
            chassis->forwardRelative(-12 * 360 / 4 / M_PI, 100); //后退1个方格
        }
    }
    else // 后场
    {
        ballintake->moveRelative(360 * 5, 100);              //吸球器启动
        chassis->forwardRelative(-18 * 360 / 4 / M_PI, 100); //后退1.5个方格吸球
        chassis->forwardRelative(18 * 360 / 4 / M_PI, 100);  //前进1.5个方格
        chassis->rotateReative(720 * autoFlag, 100);         //左转90
        catapule->moveRelative(360, 100);                    //射高旗
        chassis->rotateReative(720 * autoFlag, 100);         //左转90
        if (!sysData->jsonVal["自动赛"]["开台&不开台"])      // 开台
        {
            chassis->forwardRelative(30, 100); //前进2.5个方格 停泊
        }
        else //不开台
        {
        }
    }

    //chassis->forwardRelative(1080, 100);         //机器人以100的速度 前进轮子转3圈
    //lift->moveRelative(200, 100);                //升到重置前位置的200 位置速度100
    //chassis->rotateReative(720 * autoFlag, 100); //机器人以100的速度旋转 右轮子转3圈.注意误差 注意autoFlag 这样代码可以省一半
    sysData->stopAllObj();
    logger->debug({"自动赛结束:", std::to_string(autonomousTimer.getDtFromMark())});
}
