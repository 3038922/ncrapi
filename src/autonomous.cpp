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

    chassis->forwardRelative(1080, 100);         //机器人以100的速度 前进轮子转3圈
    lift->moveRelative(200, 100);                //升到重置前位置的200 位置速度100
    chassis->rotateReative(720 * autoFlag, 100); //机器人以100的速度旋转 右轮子转3圈.注意误差 注意autoFlag 这样代码可以省一半
}
