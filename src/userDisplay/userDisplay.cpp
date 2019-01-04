/**
 * @Author: 陈昱安
 * @Date:   2018-10-22T22:01:37+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-10-28T22:09:02+08:00
 */

#include "api.h"
#include "ncrapi/system/systemData.hpp"

namespace ncrapi
{
/**
 *构造函数初始化 
 */
UserDisplay::UserDisplay()
{
    displayObj[OBJ_BTNM_SON] = lv_obj_create(lv_scr_act(), nullptr);
    lv_obj_set_size(displayObj[OBJ_BTNM_SON], LV_HOR_RES, LV_VER_RES); //设置页面大小
    //增加自定义字库
    lv_font_add(&ncrfont10, nullptr);
    /*初始化外星人主题*/
    theme = lv_theme_alien_init(100, &ncrfont10);
    //设置 tabview样式
    theme->tabview.indic->body.padding.inner = 1;
    theme->tabview.btn.rel->text.font = &ncrfont10; //重新设置字体
    theme->tabview.btn.rel->body.padding.ver = 5;   //释放按钮的垂直填充应用于所有按钮
    //全局样式
    lv_style_copy(&mainStyle, &lv_style_pretty_color); //拷贝当前按钮样式
    mainStyle.body.main_color = LV_COLOR_BLACK;        //全局背影黑色
    mainStyle.body.grad_color = LV_COLOR_BLACK;        //全局背影黑色
    mainStyle.text.font = &ncrfont10;
    mainStyle.body.padding.ver = 5;

    //设置LOGO
    logoObj = lv_img_create(displayObj[OBJ_BTNM_SON], nullptr);

    lv_img_set_src(logoObj, &logo); //将创建的文件设置为图像
    lv_obj_align(logoObj, displayObj[OBJ_BTNM_SON], LV_ALIGN_IN_RIGHT_MID, 0, 0);
    lv_obj_animate(logoObj, LV_ANIM_FLOAT_TOP, 3000, 100, nullptr);
    //应用全局样式
    lv_obj_set_style(displayObj[OBJ_BTNM_SON], &mainStyle); /*设置Surand系统主题*/
    lv_theme_set_current(theme);
}
/**
 *自定义类创建 
 * @param 页面名称 
 * @param 是否重新绘制新屏幕 
 * @param 控制台字符串 
 * @param parent 父对象
 * @param labText  页面名称
 */
void UserDisplay::createUserObj(obj_flag objname, bool isSrcLoad, const char *terminalText, lv_obj_t *parent, const char *labText)
{
    if (displayObj[objname] == nullptr)
    {
        if (isSrcLoad)
        {
            displayObj[objname] = lv_obj_create(parent, nullptr);
            lv_scr_load(displayObj[objname]);
        }
        else
        {
            displayObj[objname] = lv_obj_create(parent, nullptr);

            lv_obj_set_style(displayObj[objname], &mainStyle);
            lv_obj_set_size(displayObj[objname], LV_HOR_RES, LV_VER_RES - 10); //设置页面大小
            lv_obj_set_style(displayObj[objname], &mainStyle);                 //设置样式
            //退出按钮
            createExitBtn(objname);
        }

        std::cout << "obj:" << terminalText << " create successful" << std::endl;
    }
    else
        std::cerr << "obj:" << terminalText << " already exist" << std::endl;

    if (labText != nullptr)
    {
        lv_obj_t *lab = lv_label_create(userDisplay->displayObj[objname], nullptr);
        lv_label_set_text(lab, labText);
    }
}
/**
 * 使用LVGLTASK函数创建一个线程
 * @param taskName 线程名称
 * @param task 线程函数
 * @param loopTime  循环时间
 * @param terminalText 线程名称
 */
void UserDisplay::createUserTask(task_flag taskName, void (*task)(void *), uint32_t loopTime, const char *terminalText)
{
    if (displayTask[taskName] == nullptr)
    {
        displayTask[taskName] = lv_task_create(task, loopTime, LV_TASK_PRIO_LOW, nullptr);
        std::cout << "task:" << terminalText << " create successful" << std::endl;
    }
    else
    {
        std::cout << "task:" << terminalText << " already exist" << std::endl;
    }
}
/**
 * 删除所有线程 
 */
void UserDisplay::delTasks()
{
    unsigned int flag = 1;
    for (auto &it : userDisplay->displayTask)
    {
        if (it != nullptr)
        {
            lv_task_del(it);
            it = nullptr;
            std::cout << "del task:" << flag << std::endl;
            flag++;
        }
    }
}
/**
 * 删除所有对象 
 */
void UserDisplay::delObjs()
{
    unsigned int flag = 1;
    for (auto &it : userDisplay->displayObj)
    {
        if (it != nullptr)
        {
            lv_obj_del(it);
            it = nullptr;
            std::cout << "del Obj:" << flag << std::endl;
            flag++;
        }
    }
}
/**
 * 用于获取系统LOOP时间的静态函数 
 * @param param 
 */
static void loopTask(void *param)
{
    (void)param;               /*Unused*/
    userDisplay->ostr.clear(); //1：调用clear()清除当前错误控制状态，其原型为 void clear (iostate state=goodbit);
    userDisplay->ostr.str(""); //2：调用str("")将缓冲区清零，清除脏数据
    userDisplay->ostr << "loop:" << userDisplay->loopTime << "max:" << userDisplay->maxLoopTime << "min:" << userDisplay->minLoopTime << std::endl;
    lv_label_set_text(userDisplay->loopLab, userDisplay->ostr.str().c_str());
}
/**
 * 创建遥控模块页面 
 */
void UserDisplay::createOpObj(const std::string &userInfo)
{
    delTasks();
    createUserTask(TASK_LOOP, loopTask, 100, "loopLab");
    delObjs();
    createUserObj(OBJ_OPCONTROL, true, "opControl");
    if (!pros::competition::is_connected()) //没插场控
        createStartObj();

    lv_obj_t *robotInfoLab = lv_label_create(displayObj[OBJ_OPCONTROL], nullptr);
    lv_obj_set_x(robotInfoLab, LV_HOR_RES / 2 - 30);
    lv_label_set_static_text(robotInfoLab, userInfo.c_str());
    loopLab = lv_label_create(displayObj[OBJ_OPCONTROL], nullptr);
    loopTask(nullptr);
}

static void sensorsTask(void *param)
{
    (void)param;                                                             /*Unused*/
    userDisplay->ostr.clear();                                               //1：调用clear()清除当前错误控制状态，其原型为 void clear (iostate state=goodbit);
    userDisplay->ostr.str("");                                               //2：调用str("")将缓冲区清零，清除脏数据
    userDisplay->ostr << std::fixed << std::setprecision(1) << std::setw(6); //流操纵算子
    for (auto &it : sysData->obj)
        it->showSensor();
    std::string temp = userDisplay->ostr.str();
    lv_label_set_text(userDisplay->otherLab, temp.c_str());
}
/**
 * 自动赛选择时候的确认按钮的动作
 * @param  btn 要实现动作的按钮的指针
 * @return     返回不知道啥....
 */
static std::array<lv_obj_t *, AUTO_NUMS> compSw;
static lv_res_t confirmBtnIncomp(lv_obj_t *btn)
{
    int i = 0;
    //获取开关状态
    for (auto &it : sysData->autoFlags)
    {
        it = lv_sw_get_state(compSw[i]);
        i++;
    }

    std::string str;
    sysData->autoSide == 0 ? str += "红方\n" : str += "蓝方\n";
    sysData->autoFlags[AUTO_FR] == 0 ? str += "前场\n" : str += "后场\n";
    sysData->autoFlags[AUTO_SHOOT] == 0 ? str += "射高旗\n" : str += "射中旗\n";
    sysData->autoFlags[AUTO_MID_SHOOT] == 0 ? str += "不二次射旗\n" : str += "二次射旗\n";
    sysData->autoFlags[AUTO_PLATFORM] == 0 ? str += "不开台\n" : str += "开台\n";
    sysData->autoFlags[AUTO_BUMPERFLAG] == 0 ? str += "不二次撞旗\n" : str += "二次撞旗\n";
    //创建确认页面
    userDisplay->createUserObj(OBJ_CONFIRM, true, "obj_confirm");
    //显示自动赛选项
    lv_obj_t *autoinfoLab = lv_label_create(userDisplay->displayObj[OBJ_CONFIRM], nullptr); //创建LAB条
    userDisplay->ostr.clear();                                                              //1：调用clear()清除当前错误控制状态，其原型为 void clear (iostate state=goodbit);
    userDisplay->ostr.str("");                                                              //2：调用str("")将缓冲区清零，清除脏数据
    userDisplay->ostr << str << std::endl;
    lv_label_set_text(autoinfoLab, userDisplay->ostr.str().c_str());
    // 传感器页面创建
    userDisplay->createUserTask(TASK_OTHER, sensorsTask, 100, "sensorInfo");                //创建一个线程
    userDisplay->otherLab = lv_label_create(userDisplay->displayObj[OBJ_CONFIRM], nullptr); //创建基于INFOObj的标签
    lv_obj_align(userDisplay->otherLab, autoinfoLab, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);        //设置传感器栏目位置
    //重置传感器按钮
    userDisplay->createResetBtn(OBJ_CONFIRM, LV_HOR_RES - 80, LV_VER_RES - 30);
    sensorsTask(nullptr); //刷新标签栏
    return LV_RES_OK;
}

/**
 * 标签栏按下后的动作
 * @param tab 标签
 * @param x   几号标签
 */
static void tabChose(lv_obj_t *tab, uint16_t x)
{
    if (x == 0)
    {
        sysData->autoIsMode = 0; //普通自动赛模式
        sysData->autoSide = 0;   //红方0
        userDisplay->theme->tabview.bg->body.main_color = LV_COLOR_RED;
    }
    else if (x == 1)
    {
        sysData->autoIsMode = 0; //普通自动赛模式
        sysData->autoSide = 360; //蓝方360
        userDisplay->theme->tabview.bg->body.main_color = LV_COLOR_BLUE;
    }
    else if (x == 2)
    {
        sysData->autoIsMode = 1; //纯自动
        sysData->autoSide = 0;   //技能赛默认红方
        userDisplay->theme->tabview.bg->body.main_color = LV_COLOR_BLACK;
    }
}
void UserDisplay::createCompe()
{
    delTasks();
    delObjs();
    createUserObj(OBJ_COMPETITION, true, "obj_competition");
    //创建标签栏
    lv_obj_t *tab = lv_tabview_create(displayObj[OBJ_COMPETITION], nullptr);

    lv_obj_set_size(tab, LV_HOR_RES, LV_VER_RES);      //设置位置
    theme->tabview.bg->body.main_color = LV_COLOR_RED; //进来后默认设置为红方

    lv_obj_t *redTab = lv_tabview_add_tab(tab, "红方");
    lv_obj_t *blueTab = lv_tabview_add_tab(tab, "蓝方");
    lv_obj_t *skillAutoTab = lv_tabview_add_tab(tab, "技能赛");
    /*当选项卡按下后进行的操作*/
    lv_tabview_set_tab_load_action(tab, tabChose);
    std::array<lv_obj_t *, AUTO_NUMS> compLab;
    int xFlag = 10, yFlag = 50, countForSw = 1, countForLab = 0;
    //创建各种开关和文本条 附带位置设置
    for (auto &it : compSw)
    {
        it = lv_sw_create(displayObj[OBJ_COMPETITION], nullptr);
        compLab[countForLab] = lv_label_create(tab, nullptr); //创建开关后面的文本条
        //位置设置
        lv_obj_set_pos(it, xFlag, yFlag);
        lv_obj_align(compLab[countForLab], it, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        yFlag += 60;
        countForSw++;
        countForLab++;
        if (countForSw >= 4)
        {
            xFlag = 250;
            yFlag = 50;
            countForSw = 0;
        }
    }
    //确认按钮
    lv_obj_t *confirmBtn = lv_btn_create(displayObj[OBJ_COMPETITION], nullptr); //创建确认开关
    lv_obj_t *confirmLab = lv_label_create(confirmBtn, nullptr);                //创建确认开关文本 这里设置按钮为父级

    lv_label_set_text(compLab[AUTO_FR], "前场&后场");
    lv_label_set_text(compLab[AUTO_SHOOT], "高旗&中旗");
    lv_label_set_text(compLab[AUTO_MID_SHOOT], "是否二次射旗");
    lv_label_set_text(compLab[AUTO_PLATFORM], "是否开台");
    lv_label_set_text(compLab[AUTO_BUMPERFLAG], "是否二次撞旗");
    lv_label_set_text(confirmLab, "确认");
    //大小设置
    lv_obj_set_size(confirmBtn, 200, 50);

    //设置确定按钮和其文本框的位置
    lv_obj_set_pos(confirmBtn, LV_HOR_RES - 200, LV_VER_RES - 50);
    //确认按钮的动作
    lv_btn_set_action(confirmBtn, LV_BTN_ACTION_PR, confirmBtnIncomp);

    //调用按钮页面
    //TODO 技能赛的动作
}
lv_res_t UserDisplay::closeAction(lv_obj_t *btn)
{
    (void)btn; /*Unused*/
    if (userDisplay->displayObj[OBJ_BTNM_SON] != nullptr)
    {
        lv_obj_del(userDisplay->displayObj[OBJ_BTNM_SON]);
        userDisplay->displayObj[OBJ_BTNM_SON] = nullptr;
    }
    if (userDisplay->displayTask[TASK_OTHER] != nullptr)
    {
        lv_task_del(userDisplay->displayTask[TASK_OTHER]);
        userDisplay->displayTask[TASK_OTHER] = nullptr;
    }
    return LV_RES_INV;
}
lv_res_t UserDisplay::resetAction(lv_obj_t *btn)
{
    (void)btn; /*Unused*/
    for (auto &it : sysData->obj)
        it->resetAllSensors();
    return LV_RES_INV;
}

void UserDisplay::createMbox(obj_flag objname, const char *txt1, const char *txt2, const char *txt3, lv_btnm_action_t action) //创建一个消息框
{
    lv_obj_t *mbox = lv_mbox_create(displayObj[objname], nullptr);
    lv_mbox_set_text(mbox, txt1);
    static const char *btns[] = {txt2, txt3, ""}; /*Button description. '\221' lv_btnm like control char*/
    lv_mbox_add_btns(mbox, btns, nullptr);
    lv_obj_set_width(mbox, 250);
    lv_obj_align(mbox, displayObj[objname], LV_ALIGN_IN_LEFT_MID, 0, 20); /*Align to the corner*/
    lv_mbox_set_action(mbox, action);
}

void UserDisplay::createExitBtn(obj_flag objname, const int x, const int y, const int width, const int high) //创建退出按钮
{
    lv_obj_t *exitBtn = lv_btn_create(displayObj[objname], nullptr);
    lv_obj_set_pos(exitBtn, x, y);
    lv_obj_set_size(exitBtn, width, high);
    lv_obj_t *exitLab = lv_label_create(exitBtn, nullptr);
    lv_label_set_text(exitLab, "exit");
    lv_btn_set_action(exitBtn, LV_BTN_ACTION_CLICK, closeAction);
}

void UserDisplay::createResetBtn(obj_flag objname, const int x, const int y, const int width, const int high)
{
    //退出重制传感器按钮
    lv_obj_t *resetBtn = lv_btn_create(displayObj[objname], nullptr);
    lv_obj_set_pos(resetBtn, x, y);
    lv_obj_set_size(resetBtn, width, high);
    lv_obj_t *resetLab = lv_label_create(resetBtn, nullptr);
    lv_label_set_text(resetLab, "reset");
    lv_btn_set_action(resetBtn, LV_BTN_ACTION_CLICK, resetAction);
}
} // namespace ncrapi