/**
 * @Author: 陈昱安
 * @Date:   2018-09-16T00:20:57+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-10-28T22:49:50+08:00
 */

#include "main.hpp"
//全局变量和类
ncrapi::SystemData *sysData;      //系统数据类
ncrapi::UserDisplay *userDisplay; //图像数据类

//全局初始化构造函数
//部件类初始化
//demo for nancy
pros::Controller *joy1; //主遥控器
pros::Controller *joy2; //副遥控器

ncrapi::Chassis *chassis; //底盘
ncrapi::Generic *lift;    //旋转器
ncrapi::Generic *flipper; //旋转器

//消息框动作函数
lv_res_t choseSideAction(lv_obj_t *mbox, const char *txt)
{
    if (!strcmp(txt, "红方"))
    {
        sysData->autoIsMode = 0; //普通自动赛模式
        sysData->autoSide = 0;   //红方0
        userDisplay->theme->tabview.bg->body.main_color = LV_COLOR_RED;
        userDisplay->theme->mbox.bg->body.main_color = LV_COLOR_RED;
        userDisplay->mainStyle.body.main_color = LV_COLOR_RED;
        lv_mbox_set_text(lv_mbox_get_from_btn(mbox), txt);
        lv_obj_del(mbox);
    }
    if (!strcmp(txt, "蓝方"))
    {
        sysData->autoIsMode = 0; //普通自动赛模式
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
    sysData = new ncrapi::SystemData();
    //显示初始化
    userDisplay = new ncrapi::UserDisplay;
    userDisplay->createMbox(OBJ_BTNM_SON, "请选择红蓝方(默认红方)", "红方", "蓝方", choseSideAction);
    lv_obj_t *lab1 = lv_label_create(userDisplay->displayObj[OBJ_BTNM_SON], nullptr);
    //遥控器初始化
    lv_label_set_text(lab1, "遥控器初始化中...");
    joy1 = new pros::Controller(CONTROLLER_MASTER);  //主遥控器
    joy2 = new pros::Controller(CONTROLLER_PARTNER); //副遥控器
    //显示用户信息
    lv_obj_t *lab2 = lv_label_create(userDisplay->displayObj[OBJ_BTNM_SON], nullptr);
    lv_obj_set_y(lab2, 30);
    std::string temp = robotInfo;
    temp += " 版本号:";
    temp += NCR_VERSION_STRING;
    lv_label_set_text(lab2, temp.c_str());

    //demo for nancy
    lv_label_set_text(lab1, "底盘初始化中...");
    chassis = new ncrapi::Chassis({pros::Motor(LF, pros::E_MOTOR_GEARSET_18, LF_REVERSE, pros::E_MOTOR_ENCODER_DEGREES),
                                   pros::Motor(LB, pros::E_MOTOR_GEARSET_18, LB_REVERSE, pros::E_MOTOR_ENCODER_DEGREES),
                                   pros::Motor(RF, pros::E_MOTOR_GEARSET_18, RF_REVERSE, pros::E_MOTOR_ENCODER_DEGREES),
                                   pros::Motor(RB, pros::E_MOTOR_GEARSET_18, RB_REVERSE, pros::E_MOTOR_ENCODER_DEGREES)});
    pros::delay(1000);
    lv_label_set_text(lab1, "升降初始化中...");
    lift = new ncrapi::Generic({pros::Motor(LIFT_LEFT, pros::E_MOTOR_GEARSET_18, LIFT_LEFT_REVERSE, pros::E_MOTOR_ENCODER_DEGREES),
                                pros::Motor(LIFT_RIGHT, pros::E_MOTOR_GEARSET_18, LIFT_RIGHT_REVERSE, pros::E_MOTOR_ENCODER_DEGREES)},
                               "升降", 10);
    pros::delay(1000);

    lv_label_set_text(lab1, "旋转器初始化中...");
    flipper = new ncrapi::Generic({pros::Motor(FLIPPER, pros::E_MOTOR_GEARSET_18, FLIPPER_REVERSE, pros::E_MOTOR_ENCODER_DEGREES)}, "旋转器", 10); //旋转器
    pros::delay(1000);
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
