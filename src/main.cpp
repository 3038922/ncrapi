
#include "main.hpp"

void opcontrol()
{
    userDisplay->createOpObj();
    uint32_t lastTime = pros::millis();
    uint32_t nowTime = pros::millis();
    while (true)
    {
        userDisplay->loopTime = pros::millis() - lastTime;
        lastTime = pros::millis();
        if (userDisplay->loopTime > userDisplay->maxLoopTime)
            userDisplay->maxLoopTime = userDisplay->loopTime;
        if (userDisplay->loopTime < userDisplay->minLoopTime)
            userDisplay->minLoopTime = userDisplay->loopTime;
        //demo for nancy
        if (sysData->isOPcontrol)
        {
            chassis->arcade(joy1, ANALOG_LEFT_Y, ANALOG_RIGHT_X); //底盘遥控
            lift->joyControl(joy1, DIGITAL_L1, DIGITAL_L2);       //升降
            cap->joyControl(joy1, DIGITAL_R1, DIGITAL_R2);        //盘子夹
            //demo for nancy
        }
        else
            sysData->stopAllObj();
        pros::Task::delay_until(&nowTime, 10);
    }
}
