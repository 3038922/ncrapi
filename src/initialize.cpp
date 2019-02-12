/**
 * @Author: 陈昱安
 * @Date:   2018-09-16T00:20:57+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-10-28T22:49:50+08:00
 */

#include "main.hpp"
//全局变量和类

std::unique_ptr<ncrapi::SysBase> sysData = nullptr;         //系统数据类
std::unique_ptr<ncrapi::UserDisplay> userDisplay = nullptr; //图像数据类
//全局初始化构造函数
//部件类初始化
//demo for nancy
std::shared_ptr<pros::Controller> joy1;
std::shared_ptr<ncrapi::Chassis> chassis;
std::shared_ptr<ncrapi::Generic> lift;
std::shared_ptr<ncrapi::Generic> catapule;   //弹射
std::shared_ptr<ncrapi::Generic> ballintake; //吸吐
std::shared_ptr<ncrapi::Generic> cap;        //夹子

//消息框动作函数
lv_res_t choseSideAction(lv_obj_t *mbox, const char *txt)
{
    if (!strcmp(txt, "红方"))
    {
        // sysData->autoIsMode = 0; //普通自动赛模式
        sysData->autoSide = 0; //红方0
        userDisplay->theme->tabview.bg->body.main_color = LV_COLOR_RED;
        userDisplay->theme->mbox.bg->body.main_color = LV_COLOR_RED;
        userDisplay->mainStyle.body.main_color = LV_COLOR_RED;
        lv_mbox_set_text(lv_mbox_get_from_btn(mbox), txt);
        lv_obj_del(mbox);
    }
    if (!strcmp(txt, "蓝方"))
    {
        //   sysData->autoIsMode = 0; //普通自动赛模式
        sysData->autoSide = 360; //蓝方360
        userDisplay->theme->tabview.bg->body.main_color = LV_COLOR_BLUE;
        userDisplay->theme->mbox.bg->body.main_color = LV_COLOR_BLUE;
        userDisplay->mainStyle.body.main_color = LV_COLOR_BLUE;
        lv_mbox_set_text(lv_mbox_get_from_btn(mbox), txt);
        lv_obj_del(mbox);
    }
    return LV_RES_OK; /*Return OK if the message box is not deleted*/
}

/**
 * 初始化函数
 */

void initialize()
{ //系统初始化
    sysData = std::make_unique<ncrapi::SysBase>(userData);
    //显示初始化
    userDisplay = std::make_unique<ncrapi::UserDisplay>();
    userDisplay->createMbox(OBJ_BTNM_SON, "请选择红蓝方(默认红方)", "红方", "蓝方", choseSideAction);
    lv_obj_t *lab1 = lv_label_create(userDisplay->displayObj[OBJ_BTNM_SON], nullptr);
    //遥控器初始化
    lv_label_set_text(lab1, "遥控器初始化中...");
    joy1 = std::make_shared<pros::Controller>(CONTROLLER_MASTER); //主遥控器
    //显示用户信息
    lv_obj_t *lab2 = lv_label_create(userDisplay->displayObj[OBJ_BTNM_SON], nullptr);
    lv_obj_set_y(lab2, 20);
    std::string temp = sysData->robotInfo;
    temp += "\n版本号:";
    temp += NCR_VERSION_STRING;
    lv_label_set_text(lab2, temp.c_str());

    //demo for nancy
    lv_label_set_text(lab1, "底盘初始化中...");
    chassis = std::make_shared<ncrapi::Chassis>(sysData->jsonVal["底盘"]);
    lv_label_set_text(lab1, "升降初始化中...");
    lift = std::make_shared<ncrapi::Generic>("升降", sysData->jsonVal["升降"]);
    lv_label_set_text(lab1, "弹射初始化中...");
    catapule = std::make_shared<ncrapi::Generic>("弹射", sysData->jsonVal["弹射"]);
    lv_label_set_text(lab1, "吸吐初始化中...");
    ballintake = std::make_shared<ncrapi::Generic>("吸吐", sysData->jsonVal["吸吐"]);
    lv_label_set_text(lab1, "夹子初始化中...");
    cap = std::make_shared<ncrapi::Generic>("夹子", sysData->jsonVal["夹子"]);
    //demo for nancy
    lv_label_set_text(lab1, "机器人初始化完毕...");
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
