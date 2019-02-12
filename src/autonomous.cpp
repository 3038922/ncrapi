/**
 * @Author: 陈昱安
 * @Date:   2018-09-16T00:20:58+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   yan
 * @Last modified time: 2018-10-16T12:51:39+08:00
 */

#include "main.hpp"
/**
 * 自动赛
 */

void autonomous()
{
    userDisplay->delTasks();
    userDisplay->delObjs();
    userDisplay->createUserObj(OBJ_AUTONOMOUS, true, "obj_auto", nullptr, "自动赛进行中");
    int autoFlag = 0;
    //DEMO FOR NANCY
    if (sysData->autoSide == 0) //红方自动
        autoFlag = 1;
    else //蓝方自动
        autoFlag = -1;

    if (!sysData->jsonVal["自动赛"]["前场&后场"]) //前场
    {
        chassis->forwardRelative(-1080, 100);        //后退一个方格
        catapule->moveRelative(200, 100);            //射高旗
        chassis->forwardRelative(1080, 100);         //前进1个方格
        chassis->rotateReative(720 * autoFlag, 100); //右转90
        ballintake->moveRelative(200, 100);          //吸球器启动
        chassis->forwardRelative(-1080, 100);        //后退1.5个方格吸球
        chassis->forwardRelative(1080, 100);         //前进1.5个方格
        chassis->rotateReative(720 * autoFlag, 100); //左转90
        chassis->forwardRelative(1080, 100);         //前进1个方格
        catapule->moveRelative(200, 100);            //射中旗
        chassis->forwardRelative(1080, 100);         //前进0.5个方格 撞低旗

        if (!sysData->jsonVal["自动赛"]["开台&不开台"]) // 开台
        {
            chassis->forwardRelative(-1080, 100);        //后退3.5个方格
            chassis->rotateReative(720 * autoFlag, 100); //左转90
            chassis->forwardRelative(1080, 100);         //前进2.5个方格 停泊
        }
        else // 不开台
        {
            chassis->forwardRelative(-1080, 100); //后退1个方格
        }
    }
    else // 后场
    {
        ballintake->moveRelative(200, 100);             //吸球器启动
        chassis->forwardRelative(-1080, 100);           //后退1.5个方格吸球
        chassis->forwardRelative(1080, 100);            //前进1.5个方格
        chassis->rotateReative(720 * autoFlag, 100);    //左转90
        catapule->moveRelative(200, 100);               //射高旗
        chassis->rotateReative(720 * autoFlag, 100);    //左转90
        if (!sysData->jsonVal["自动赛"]["开台&不开台"]) // 开台
        {
            chassis->forwardRelative(1080, 100); //前进2.5个方格 停泊
        }
        else //不开台
        {
        }
    }

    chassis->forwardRelative(1080, 100);         //机器人以100的速度 前进轮子转3圈
    lift->moveRelative(200, 100);                //升到重置前位置的200 位置速度100
    chassis->rotateReative(720 * autoFlag, 100); //机器人以100的速度旋转 右轮子转3圈.注意误差 注意autoFlag 这样代码可以省一半
}
