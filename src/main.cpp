#include "main.h"
#include "ncrapi/ncrapi.hpp"
#include "userconfig.hpp"
using namespace ncrapi;
//全局变量和类
//全局初始化构造函数
//部件类初始化
std::shared_ptr<pros::Controller> joy1 = nullptr;
std::shared_ptr<pros::Controller> joy2 = nullptr;
std::shared_ptr<ncrapi::Generic> roulette = nullptr; //吸吐
std::shared_ptr<ncrapi::Generic> shooter = nullptr;  //发射器
pros::Task *autoTask = nullptr;
void initialize()
{
    Logger::initLogger();               //logger 单例实现
    UserDisplay::initUserDisplay();     //显示初始化 单例实现
    NcrSystem::initNcrSystem(userData); //系统数据初始化 单例实现

    userDisplay->init(); //设置当前背景颜色
    //遥控器初始化
    joy1 = std::make_shared<pros::Controller>(CONTROLLER_MASTER);  //主遥控器
    joy2 = std::make_shared<pros::Controller>(CONTROLLER_PARTNER); //副遥控器
    // 底盘初始化
    Chassis::initChassis(ncrSys->getJson()[I18N_CHASSIS]);
    shooter = std::make_shared<ncrapi::Generic>("发射器", ncrSys->getJson()["发射器"]);
    roulette = std::make_shared<ncrapi::Generic>("吸吐", ncrSys->getJson()["吸吐"]);
    logger->info("机器人初始化完毕\n");
    userDisplay->terminal->style_p->body.main_color = LV_COLOR_BLACK; //控制台还原为黑色
    lv_obj_del(userDisplay->logoObj);
}

void disabled()
{
    userDisplay->delTasks();
    userDisplay->delObjs();
    userDisplay->createUserObj(OBJ_DISABLED, "obj_disabled", nullptr, "场控关闭状态\n");
    for (auto &it : ncrSys->obj)
        it->setMode(0); //将所有部件都重置为0模式
    if (autoTask != nullptr)
    {
        if (autoTask->get_state() != 4)
        {
            autoTask->remove();
            delete autoTask;
            logger->debug("自动赛线程 自动赛外 删除成功!\n");
        }
        autoTask = nullptr;
    }
}

void competition_initialize() { userDisplay->createCompe(); }

void autonomous()
{
    Timer autonomousTimer;
    autonomousTimer.placeMark();
    if (autoTask == nullptr)
        autoTask = new pros::Task((pros::task_fn_t)taskAuto, nullptr, TASK_PRIORITY_DEFAULT + 2, TASK_STACK_DEPTH_DEFAULT, "task_auto");
    if (pros::competition::get_status() == COMPETITION_CONNECTED)
    {
        userDisplay->delTasks();
        userDisplay->delObjs();
        userDisplay->createUserObj(OBJ_AUTONOMOUS, "自动赛", nullptr, "自动赛进行中");
    }
    logger->debug("自动赛开始:", autonomousTimer.getDtFromMark().convert(millisecond), "\n");
    if (!ncrSys->getJson()["自动赛"]["自动赛&纯自动"])
        userauto(ncrSys->getJson()["自动赛"], autonomousTimer);
    else
        skillAuto(ncrSys->getJson()["自动赛"], autonomousTimer);
    ncrSys->stopAllObj();
    if (autoTask != nullptr) //如果shoot类不为空 则创建一个task
    {
        autoTask->remove();
        delete autoTask;
        autoTask = nullptr;
        logger->debug("自动赛线程 自动赛内 删除成功!\n");
    }
    logger->debug("自动赛结束:", autonomousTimer.getDtFromMark().convert(millisecond), "\n");
}

void opcontrol()
{
    if (autoTask != nullptr)
    {
        if (autoTask->get_state() != 4)
        {
            autoTask->remove();
            delete autoTask;
            logger->debug("自动赛线程 手动赛处 删除成功!\n");
        }
        autoTask = nullptr;
    }
    chassis->setBrakeMode(pros::E_MOTOR_BRAKE_COAST); //放开刹车 防止自动弄错
    userDisplay->createOpObj();
    uint32_t lastTime = pros::millis();
    uint32_t nowTime = pros::millis();
    std::shared_ptr<pros::Controller> joy = joy1;
    ncrapi::Timer joyDisTime;
    joy1 = std::make_shared<pros::Controller>(CONTROLLER_MASTER); //主遥控器
    joy1->print(0, 0, "joy    robot   ball");
    while (true)
    {
        // std::cout << "errorCode:" << strerror(errno) << std::endl; //打印错误码
        userDisplay->loopTime = pros::millis() - lastTime;
        lastTime = pros::millis();
        if (userDisplay->loopTime > userDisplay->maxLoopTime)
            userDisplay->maxLoopTime = userDisplay->loopTime;
        if (userDisplay->loopTime < userDisplay->minLoopTime)
            userDisplay->minLoopTime = userDisplay->loopTime;
        if (ncrSys->getRobotMode() == NORMAL)
        {
            if (joy1->get_digital_new_press(DIGITAL_RIGHT)) //切换单人双人
            {
                if (joy == joy1)
                    joy = joy2;
                else
                    joy = joy1;
                joy->rumble(". ");
            }

            chassis->arcade(joy1, ANALOG_LEFT_Y, ANALOG_RIGHT_X);
            roulette->joyControl(joy, DIGITAL_R1, DIGITAL_R2);
            shooter->joyControl(joy, DIGITAL_L1, DIGITAL_L2);
        }
        else //DEBUG模式
        {
            ncrSys->stopAllObj();
            if (joy1->get_digital_new_press(DIGITAL_A))
                ncrSys->testAction(&autonomous, &customTest, joy1);
        }
        if (joyDisTime.getDtFromMark() >= 100_ms)
        {
            //满电4200 没电的时候3400和 16
            joy->print(1, 0, "%d%%    %.0f%%     %u", joy1->get_battery_capacity(), pros::battery::get_capacity(), opticalSystem->getBallNums());
            joyDisTime.placeMark();
        }
        pros::Task::delay_until(&nowTime, 10);
    }
}