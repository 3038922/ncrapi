#pragma once
#include "display/lv_conf.h"
#include "display/lvgl.h"
#include <array>
#include <iostream>
#include <sstream>
extern lv_font_t ncrfont10;
// extern lv_img_t field;
LV_IMG_DECLARE(field); //声明一个场地图像变量
LV_IMG_DECLARE(logo);  //声明一个logo图像变量
typedef enum obj_flag
{
    OBJ_BTNM_SON, //起始页面下的选项
    BTNM_START,   //按钮阵列 为了防止重复删除 这个也要放前面

    OBJ_COMPETITION,
    OBJ_DISABLED,
    OBJ_AUTONOMOUS,
    OBJ_OPCONTROL,
    OBJ_CONFIRM, //从这里开始下面都无法lv_load_src(),因为是依附状态
} obj_flag;

typedef enum task_flag
{
    TASK_LOOP, //记录遥控模式下LOOP的TASK
    TASK_OTHER //其他模式下的线程
} task_flag;
namespace ncrapi
{

class UserDisplay
{
  public:
    //样式
    lv_theme_t *theme;
    lv_style_t mainStyle;
    std::array<lv_obj_t *, 8> displayObj = {nullptr};   //对象
    std::array<lv_task_t *, 2> displayTask = {nullptr}; //线程
    //标题栏
    lv_obj_t *otherLab = nullptr;
    lv_obj_t *loopLab = nullptr;
    lv_obj_t *logoObj = nullptr;
    //时间变量
    uint32_t loopTime = 0;
    uint32_t maxLoopTime = 0;
    uint32_t minLoopTime = 20;
    //自动赛选项按钮

    UserDisplay();
    void delObjs();
    void delTasks();
    void createUserObj(obj_flag objname, bool isSrcLoad, const char *terminalText, lv_obj_t *parent = nullptr, const char *labText = nullptr);
    void createUserTask(task_flag taskName, void (*task)(void *), uint32_t loopTime, const char *terminalText);
    void createCompe();
    void createOpObj(const std::string &userInfo);
    void createStartObj();
    void createVersion(lv_obj_t *parent);    //1
    void createConfig(lv_obj_t *parent);     //2
    void createVision(lv_obj_t *parent);     //3
    void createCustomTest(lv_obj_t *parent); //4
    void createSysInfo(lv_obj_t *parent);    //5
    void createPidTest(lv_obj_t *parent);    //6
    void createOdom(lv_obj_t *parent);       //7
    void createDebug(lv_obj_t *parent);      //8

    void createExitBtn(obj_flag objname, const int x = LV_HOR_RES - 80, const int y = LV_VER_RES - 50, const int width = 50, const int high = 25); //创建退出按钮
    void createResetBtn(obj_flag objname, const int x, const int y, const int width = 50, const int high = 25);                                    //创建重制传感器按钮
    void createMbox(obj_flag objname, const char *txt1, const char *txt2, const char *txt3, lv_btnm_action_t action);                              //创建一个消息框
    static lv_res_t closeAction(lv_obj_t *btn);                                                                                                    //退出按钮的动作
    static lv_res_t resetAction(lv_obj_t *btn);                                                                                                    //退出按钮的动作

    std::ostringstream ostr;
};
} // namespace ncrapi
extern ncrapi::UserDisplay *userDisplay;
