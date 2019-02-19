/**
 * @Author: 陈昱安
 * @Date:   2018-09-16T00:20:58+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-10-28T22:54:23+08:00
 */

#include "main.hpp"

void opcontrol()
{
    userDisplay->createOpObj(sysData->robotInfo);
    uint32_t nowTime = pros::millis();
    uint32_t lastTime = pros::millis();

    while (true)
    {
        nowTime = pros::millis();
        userDisplay->loopTime = nowTime - lastTime;
        if (userDisplay->loopTime > userDisplay->maxLoopTime)
            userDisplay->maxLoopTime = userDisplay->loopTime;
        if (userDisplay->loopTime < userDisplay->minLoopTime)
            userDisplay->minLoopTime = userDisplay->loopTime;
        //demo for nancy
        chassis->arcade(joy1, ANALOG_LEFT_Y, ANALOG_RIGHT_X); //底盘遥控
        lift->joyControl(joy1, DIGITAL_L1, DIGITAL_L2);       //升降
        catapule->joyControl(joy1, DIGITAL_A, DIGITAL_B);   //弹射
        ballintake->joyControl(joy1, DIGITAL_X, DIGITAL_Y); //吸吐
        cap->joyControl(joy1, DIGITAL_R1, DIGITAL_R2);        //盘子夹
        //demo for nancy
        lastTime = nowTime;
        pros::c::task_delay_until(&nowTime, 10);
    }
}
