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
std::shared_ptr<ncrapi::Chassis> chassis = nullptr;
std::shared_ptr<ncrapi::Generic> lift = nullptr;
std::shared_ptr<ncrapi::Generic> catapule = nullptr;   //弹射
std::shared_ptr<ncrapi::Generic> ballintake = nullptr; //吸吐
std::shared_ptr<ncrapi::Generic> cap = nullptr;        //夹子

/**
 * 初始化函数
 */

void initialize()

{

    //系统日志初始化
    logger = std::make_unique<ncrapi::Logger>(2);
    //系统初始化
    sysData = std::make_unique<ncrapi::SysBase>(userData);
    //显示初始化
    userDisplay = std::make_unique<ncrapi::UserDisplay>();
    lv_obj_t *lab1 = lv_label_create(userDisplay->displayObj[OBJ_BTNM_SON], nullptr);
    lv_obj_set_y(lab1, 40);
    //遥控器初始化
    lv_label_set_text(lab1, "遥控器初始化中...");
    joy1 = std::make_shared<pros::Controller>(CONTROLLER_MASTER); //主遥控器
    //显示用户信息
    lv_obj_t *lab2 = lv_label_create(userDisplay->displayObj[OBJ_BTNM_SON], nullptr);
    std::stringstream oss;
    oss << userData["系统信息"]["机器人类型"].get<std::string>() << " " << userData["系统信息"]["队伍编号"].get<std::string>() << " "
        << userData["系统信息"]["用户"].get<std::string>() << "\n版本号:" << NCR_VERSION_STRING;
    lv_label_set_text(lab2, oss.str().c_str());

    //demo for nancy
    lv_label_ins_text(lab1, LV_LABEL_POS_LAST, "\n底盘初始化中...");
    chassis = std::make_shared<ncrapi::Chassis>(sysData->jsonVal["底盘"]);
    lv_label_ins_text(lab1, LV_LABEL_POS_LAST, "\n升降初始化中...");
    lift = std::make_shared<ncrapi::Generic>("升降", sysData->jsonVal["升降"]);
    lv_label_ins_text(lab1, LV_LABEL_POS_LAST, "\n弹射初始化中...");
    catapule = std::make_shared<ncrapi::Generic>("弹射", sysData->jsonVal["弹射"]);
    lv_label_ins_text(lab1, LV_LABEL_POS_LAST, "\n吸吐初始化中...");
    ballintake = std::make_shared<ncrapi::Generic>("吸吐", sysData->jsonVal["吸吐"]);
    lv_label_ins_text(lab1, LV_LABEL_POS_LAST, "\n盘子夹初始化中...");
    cap = std::make_shared<ncrapi::Generic>("夹子", sysData->jsonVal["夹子"]);
    lv_label_ins_text(lab1, LV_LABEL_POS_LAST, "\n机器人初始化完毕...");
    logger->info({"机器人初始化完毕"});
    lv_obj_del(userDisplay->displayObj[OBJ_BTNM_SON]);
    userDisplay->displayObj[OBJ_BTNM_SON] = nullptr;
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
    userDisplay->createUserObj(OBJ_DISABLED, true, "obj_disabled", nullptr, "场控关闭状态");
}
