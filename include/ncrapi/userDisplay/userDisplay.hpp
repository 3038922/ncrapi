#pragma once
extern "C" {
#include "display/lv_conf.h"
#include "display/lvgl.h"
#include "pros/vision.h"
extern lv_font_t ncrfont8; //微软雅黑8
LV_IMG_DECLARE(logo)       //声明一个logo图像变量
}
#include "ncrapi/system/json.hpp"

#include <sstream>

typedef enum obj_flag {
    OBJ_BTNM_SON, //起始页面下的选项
    BTNM_START,   //按钮阵列 为了防止重复删除 这个也要放前面

    OBJ_COMPETITION,
    OBJ_DISABLED,
    OBJ_AUTONOMOUS,
    OBJ_OPCONTROL,
} obj_flag;

typedef enum task_flag {
    TASK_LOOP, //记录遥控模式下LOOP的TASK
    TASK_OTHER //其他模式下的线程
} task_flag;
namespace ncrapi {

class UserDisplay
{
  public:
    lv_obj_t *terminal; //终端
    std::vector<lv_obj_t *> terminalLabs;
    lv_obj_t *errorLabs, *warningLabs; //控制台上的显示错误数量的lab
    //样式
    lv_theme_t *theme;
    lv_style_t mainStyle;
    lv_style_t terminalStyle;

    std::array<lv_obj_t *, 6> displayObj = {nullptr};   //对象
    std::array<lv_task_t *, 2> displayTask = {nullptr}; //线程
    //标题栏
    lv_obj_t *otherLab = nullptr;
    lv_obj_t *loopLab = nullptr;
    lv_obj_t *logoObj = nullptr;
    //时间变量
    uint32_t loopTime = 0;
    uint32_t maxLoopTime = 0;
    uint32_t minLoopTime = 20;
    //饿汉模式单例实现.线程安全
    static UserDisplay *initUserDisplay();
    static UserDisplay *getUserDisplay();
    json &setTmpJson() { return _tmpJson; }
    const json getTmpJson() { return _tmpJson; }
    //自动赛选项按钮
    void delObjs();
    void delTasks();
    void drawRectangle(lv_obj_t *obj, const int i, const pros::vision_object &data, lv_style_t *style);
    void createUserObj(obj_flag objname, const char *terminalText, lv_obj_t *parent = nullptr, const char *labText = nullptr);
    void createUserTask(task_flag taskName, void (*task)(void *), uint32_t loopTime, const char *terminalText, void *pragma = nullptr);
    void createCompe(lv_obj_t *parent = nullptr);            //自动赛选择页面
    static lv_res_t compTabChose(lv_obj_t *tab, uint16_t x); //用于自动赛选择的静态函数
    static lv_res_t confirmBtnInOdom(lv_obj_t *btn);         //用于ODOM自动赛选择的静态动作函数
    static lv_res_t confirmBtnIncomp(lv_obj_t *btn);         //用于自动赛确认页面的静态函数
    void createOpObj();
    static void loopTask(void *param);     //获取时间的线程
    void createTerminal(lv_obj_t *parent); //创建控制台
    void createStartObj(lv_obj_t *parent);
    void createVersion(lv_obj_t *parent);                                                                                                            //1
    void createConfig(lv_obj_t *parent);                                                                                                             //2
    void createVision(lv_obj_t *parent);                                                                                                             //3
    void createMaintenanceInfo(lv_obj_t *parent);                                                                                                    //4
    void createSysInfo(lv_obj_t *parent);                                                                                                            //5
    void createPidTest(lv_obj_t *parent);                                                                                                            //6
    void createOdom(lv_obj_t *parent);                                                                                                               //7
    void createCustomTest(lv_obj_t *parent);                                                                                                         //8
    void createOpticalTest(lv_obj_t *parent);                                                                                                        //9
    void createDistanceTest(lv_obj_t *parent);                                                                                                       //10
    void createExitBtn(obj_flag objname, const int x = LV_HOR_RES - 80, const int y = LV_VER_RES - 60, const int width = 50, const int high = 25);   //创建退出按钮
    void createSaveBtn(obj_flag objname, const int x = LV_HOR_RES - 140, const int y = LV_VER_RES - 60, const int width = 50, const int high = 25);  //创建保存按钮
    void createResetBtn(obj_flag objname, const int x = LV_HOR_RES - 140, const int y = LV_VER_RES - 60, const int width = 50, const int high = 25); //创建重制传感器按钮
    void createMbox(obj_flag objname, const char *txt1, const char *txt2, const char *txt3, lv_btnm_action_t action);                                //创建一个消息框
    lv_obj_t *createUpdownBtn(lv_obj_t *parent, const char *key, json &val, lv_btnm_action_t action = upDownAction);
    lv_obj_t *createUpdownBtnArr(lv_obj_t *parent, json &tempJson, const int column,
                                 const size_t w, const size_t h,
                                 const std::initializer_list<std::string> ignore = {""}); //创建更改按钮
    void init();
    static lv_res_t startBtnmAction(lv_obj_t *btnm, const char *txt); //启动页面动作
    static lv_res_t closeAction(lv_obj_t *btn);                       //退出按钮的动作
    static lv_res_t resetAction(lv_obj_t *btn);                       //重置按钮的动作
    static lv_res_t saveAction(lv_obj_t *btn);                        //保存按钮的动作
    static lv_res_t upDownAction(lv_obj_t *btnm, const char *txt);    //upDownBtn的动作
    static lv_res_t swAction(lv_obj_t *sw);
    static lv_res_t hideAction(lv_obj_t *btn); //隐藏按钮

    static lv_res_t clearAction(lv_obj_t *btn); //控制台清楚按钮

    std::ostringstream ostr;

  private:
    static UserDisplay *_userDisplay; // 单例对象在这里！
    json _tmpJson;
    UserDisplay();
};
#define userDisplay UserDisplay::getUserDisplay()
} // namespace ncrapi