/**
 * @Author: 陈昱安
 * @Date:   2018-09-16T00:20:57+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-10-28T22:49:50+08:00
 */

#include "main.hpp"
//全局变量和类
std::unique_ptr<ncrapi::Logger> logger;                     //系统日志
std::unique_ptr<ncrapi::SysBase> sysData = nullptr;         //系统数据类
std::unique_ptr<ncrapi::UserDisplay> userDisplay = nullptr; //图像数据类
//全局初始化构造函数
//部件类初始化
//demo for nancy
std::shared_ptr<pros::Controller> joy1 = nullptr;
std::unique_ptr<pros::Task> loggerTask = nullptr;
std::shared_ptr<ncrapi::Chassis> chassis = nullptr;
std::shared_ptr<ncrapi::Generic> lift = nullptr;
std::shared_ptr<ncrapi::Generic> cap = nullptr; //夹子

/**
 * 初始化函数
 */

void initialize()

{

    //系统日志初始化
    logger = std::make_unique<ncrapi::Logger>();
    //显示初始化
    userDisplay = std::make_unique<ncrapi::UserDisplay>();
    //系统初始化
    sysData = std::make_unique<ncrapi::SysBase>(userData);
    userDisplay->init(); //设置当前背景颜色
    //遥控器初始化
    joy1 = std::make_shared<pros::Controller>(CONTROLLER_MASTER); //主遥控器
    //底盘初始化
    chassis = std::make_shared<ncrapi::Chassis>(sysData->jsonVal["底盘"]);
    lift = std::make_shared<ncrapi::Generic>("升降", sysData->jsonVal["升降"]);
    cap = std::make_shared<ncrapi::Generic>("夹子", sysData->jsonVal["夹子"]);
    logger->info({"机器人初始化完毕"});
    loggerTask = std::make_unique<pros::Task>((pros::task_fn_t)taskLogger, nullptr, TASK_PRIORITY_DEFAULT - 3, TASK_STACK_DEPTH_DEFAULT, "task_logger"); //DEBUG线程开始
    userDisplay->terminal->style_p->body.main_color = LV_COLOR_BLACK;                                                                                    //控制台还原为黑色
    lv_obj_del(userDisplay->logoObj);
}
/**
*在初始化initialize()之后运行，并且在连接到场控之前自动运行
*管理系统或VEX竞争交换机。这是为了特定于竞争的初始化例程，例如自主选择器在液晶显示器上。
*此任务将在机器人启用和自动赛或手动赛时开始。
*选择自动赛 设置参数的地方
*经简易场控测试 是先接上场控,然后再开机,才会执行.已经开机再接场控 不会执行
 */
void competition_initialize()
{
    userDisplay->createCompe();
}
/**
 * 场控没开自动赛 没开手动 完全禁止的时候使用的函数
  */
void disabled()
{
    userDisplay->delTasks();
    userDisplay->delObjs();
    userDisplay->createUserObj(OBJ_DISABLED, "obj_disabled", nullptr, "场控关闭状态");
}
