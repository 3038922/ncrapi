/**
 * @Author: 陈昱安
 * @Date:   2018-10-22T22:01:37+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-10-28T22:09:02+08:00
 */

#include "api.h"
#include "ncrapi/ncrapi.hpp"
#include <iomanip>

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
    // theme->tabview.bg->body.padding.ver = 5;
    // theme->tabview.bg->body.border.width = 0;
    // theme->tabview.bg->text.line_space = 1;
    //全局样式
    lv_style_copy(&mainStyle, &lv_style_pretty_color); //拷贝当前按钮样式
    mainStyle.text.font = &ncrfont10;
    mainStyle.body.padding.ver = 5;
    if (!sysData->jsonVal["自动赛"]["红方&蓝方"]) //设置默认颜色
    {
        theme->tabview.bg->body.main_color = LV_COLOR_RED;
        mainStyle.body.main_color = LV_COLOR_RED;
    }
    else
    {
        theme->tabview.bg->body.main_color = LV_COLOR_BLUE;
        mainStyle.body.main_color = LV_COLOR_BLUE;
    }
    mainStyle.body.grad_color = LV_COLOR_BLACK; //渐变色黑色
    //设置LOGO
    logoObj = lv_img_create(displayObj[OBJ_BTNM_SON], nullptr);

    lv_img_set_src(logoObj, &logo); //将创建的文件设置为图像
    lv_obj_align(logoObj, displayObj[OBJ_BTNM_SON], LV_ALIGN_IN_RIGHT_MID, 0, 0);
    lv_obj_animate(logoObj, LV_ANIM_FLOAT_TOP, 3000, 100, nullptr);
    //应用全局样式
    lv_obj_set_style(displayObj[OBJ_BTNM_SON], &mainStyle); /*设置Surand系统主题*/
    lv_theme_set_current(theme);
    logger->info({"图像类构造成功"});
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

            lv_obj_set_size(displayObj[objname], LV_HOR_RES, LV_VER_RES - 10); //设置页面大小
            lv_obj_set_style(displayObj[objname], &mainStyle);                 //设置样式
            //退出按钮
            createExitBtn(objname);
        }
        logger->info({"图像类:", terminalText, " 构造成功"});
    }
    else
        logger->debug({"图像类:", terminalText, " 已经存在"});

    if (labText != nullptr)
    {
        lv_obj_t *lab = lv_label_create(displayObj[objname], nullptr);
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
void UserDisplay::createUserTask(task_flag taskName, void (*task)(void *), uint32_t loopTime, const char *terminalText, void *pragma)
{
    if (displayTask[taskName] == nullptr)
    {
        displayTask[taskName] = lv_task_create(task, loopTime, LV_TASK_PRIO_LOW, pragma);
        logger->info({"图像类线程:", terminalText, " 构造成功"});
    }
    else
        logger->warnning({"图像类线程:", terminalText, " 已经存在"});
}
/**
 * 删除所有线程 
 */
void UserDisplay::delTasks()
{
    unsigned int flag = 1;
    for (auto &it : displayTask)
    {
        if (it != nullptr)
        {
            lv_task_del(it);
            it = nullptr;
            std::cout << "删除图像类线程:" << flag << " 个" << std::endl;
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
    for (auto &it : displayObj)
    {
        if (it != nullptr)
        {
            lv_obj_del(it);
            it = nullptr;
            logger->info({"删除图像类:", std::to_string(flag), " 个"});
            flag++;
        }
    }
}
void UserDisplay::drawRectangle(lv_obj_t *obj, const pros::vision_object &data, lv_style_t *style)
{
    lv_obj_set_hidden(obj, false);
    lv_obj_set_style(obj, style);
    lv_obj_set_pos(obj, data.left_coord, data.top_coord);
    lv_obj_set_size(obj, data.width, data.height);
}
/**
 * 用于获取系统LOOP时间的静态函数 
 * @param param 
 */
void UserDisplay::loopTask(void *param)
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
    delObjs();
    createUserObj(OBJ_OPCONTROL, true, "遥控");
    if (!pros::competition::is_connected()) //没插场控
        createStartObj(displayObj[OBJ_OPCONTROL], LV_HOR_RES, LV_VER_RES - 20);
    lv_obj_t *robotInfoLab = lv_label_create(displayObj[OBJ_OPCONTROL], nullptr);
    lv_obj_set_x(robotInfoLab, LV_HOR_RES / 2 - 30);
    lv_label_set_static_text(robotInfoLab, userInfo.c_str());
    loopLab = lv_label_create(displayObj[OBJ_OPCONTROL], nullptr);
    createUserTask(TASK_LOOP, loopTask, 100, "循环时间条");
}

/**
 * 自动赛选择时候的确认按钮的动作
 * @param  btn 要实现动作的按钮的指针
 * @return     返回不知道啥....
 */

lv_res_t UserDisplay::confirmBtnIncomp(lv_obj_t *btn)
{
    sysData->saveData();
    //获取开关状态
    std::string str;
    for (auto &it : sysData->jsonVal["自动赛"].items())
    {
        std::string tempStr = it.key();
        auto pos = tempStr.find("&"); //查找间隔符号
        if (pos != std::string::npos)
        {
            if (it.value())
                tempStr.erase(0, pos + 1);
            else
                tempStr.erase(pos, tempStr.length());
            str += (tempStr + "\n");
        }
        else
            logger->error({"自动赛json选项设置错误 请用&间隔"});
    }
    // 创建确认页面
    lv_obj_t *confirm = lv_obj_create(userDisplay->displayObj[OBJ_COMPETITION], nullptr);

    lv_obj_set_size(confirm, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style(confirm, &userDisplay->mainStyle);
    userDisplay->createStartObj(confirm, LV_HOR_RES, LV_VER_RES - 5);
    //显示自动赛选项
    lv_obj_t *autoinfoLab = lv_label_create(userDisplay->displayObj[BTNM_START], nullptr); //创建LAB条
    userDisplay->ostr.clear();                                                             //1：调用clear()清除当前错误控制状态，其原型为 void clear (iostate state=goodbit);
    userDisplay->ostr.str("");                                                             //2：调用str("")将缓冲区清零，清除脏数据
    userDisplay->ostr << str << std::endl;
    lv_label_set_text(autoinfoLab, userDisplay->ostr.str().c_str());
    lv_obj_align(autoinfoLab, userDisplay->displayObj[BTNM_START], LV_ALIGN_IN_TOP_MID, 0, 0);
    return LV_RES_INV;
}
lv_res_t UserDisplay::confirmBtnInOdom(lv_obj_t *btn)
{
    sysData->saveData();
    lv_obj_del(userDisplay->displayObj[OBJ_COMPETITION]);
    userDisplay->displayObj[OBJ_COMPETITION] = nullptr;
    return LV_RES_INV;
}
/**
 * 标签栏按下后的动作
 * @param tab 标签
 * @param x   几号标签
 */
void UserDisplay::compTabChose(lv_obj_t *tab, uint16_t x)
{
    (void)tab; /*Unused*/
    if (x == 0)
    {
        sysData->jsonVal["自动赛"]["红方&蓝方"] = false;     //红方0
        sysData->jsonVal["自动赛"]["自动赛&纯自动"] = false; //红方0
        userDisplay->theme->tabview.bg->body.main_color = LV_COLOR_RED;
        userDisplay->mainStyle.body.main_color = LV_COLOR_RED;
    }
    else if (x == 1)
    {
        sysData->jsonVal["自动赛"]["红方&蓝方"] = true;      //红方0
        sysData->jsonVal["自动赛"]["自动赛&纯自动"] = false; //蓝方
        userDisplay->theme->tabview.bg->body.main_color = LV_COLOR_BLUE;
        userDisplay->mainStyle.body.main_color = LV_COLOR_BLUE;
    }
    else if (x == 2)
    {
        sysData->jsonVal["自动赛"]["自动赛&纯自动"] = true; //红方0
        userDisplay->theme->tabview.bg->body.main_color = LV_COLOR_BLACK;
        userDisplay->mainStyle.body.main_color = LV_COLOR_BLACK;
    }
    //应用全局样式
    lv_theme_set_current(userDisplay->theme);
}
lv_res_t UserDisplay::swAction(lv_obj_t *sw) //SW的动作
{
    json *tempData = static_cast<json *>(lv_obj_get_free_ptr(sw));
    *tempData = lv_sw_get_state(sw);
    return LV_RES_OK;
}
void UserDisplay::createCompe(lv_obj_t *parent)
{
    if (parent == nullptr)
    {
        delTasks();
        delObjs();
        createUserObj(OBJ_COMPETITION, true, "竞赛等待");
    }
    else
        createUserObj(OBJ_COMPETITION, false, "竞赛等待", parent);
    //创建标签栏
    lv_obj_t *tab = lv_tabview_create(displayObj[OBJ_COMPETITION], nullptr);
    lv_obj_set_size(tab, LV_HOR_RES, LV_VER_RES); //设置位置
    lv_obj_t *redTab = lv_tabview_add_tab(tab, "红方");
    lv_obj_t *blueTab = lv_tabview_add_tab(tab, "蓝方");
    lv_obj_t *skillTab = lv_tabview_add_tab(tab, "纯自动");
    lv_tabview_set_tab_act(tab, sysData->jsonVal["自动赛"]["红方&蓝方"].get<uint16_t>(), false); //设置默认红方还是蓝方
    /*当选项卡按下后进行的操作*/
    lv_tabview_set_tab_load_action(tab, compTabChose);
    int posX = 10, posY = 50;
    std::vector<std::pair<lv_obj_t *, lv_obj_t *>> compSw; //
    //创建各种开关和文本条 附带位置设置
    for (auto &it : sysData->jsonVal["自动赛"].items())
    {
        if (it.key() != "红方&蓝方" && it.key() != "自动赛&纯自动")
        {
            compSw.push_back(std::make_pair(lv_label_create(displayObj[OBJ_COMPETITION], nullptr), lv_sw_create(displayObj[OBJ_COMPETITION], nullptr))); //创建文本条和开关
            lv_label_set_text(compSw.back().first, it.key().c_str());                                                                                    /*设置文字*/
            lv_obj_set_size(compSw.back().second, 60, 25);
            if (it.value().get<bool>())
                lv_sw_on(compSw.back().second); //设置按钮默认值
            else
                lv_sw_off(compSw.back().second); //设置按钮默认值
            //位置设置
            lv_obj_set_pos(compSw.back().first, posX, posY);                                        //设置lab文字的位置
            lv_obj_align(compSw.back().second, compSw.back().first, LV_ALIGN_OUT_RIGHT_MID, 10, 0); //设置开关的位置
            lv_sw_set_action(compSw.back().second, swAction);                                       //设置按钮的动作
            lv_obj_set_free_ptr(compSw.back().second, &it.value());                                 //关联按钮和json
            if (lv_obj_get_x(compSw.back().second) > 230)                                           //key表示偶数换行 it.szie()表示每行总共有2个数一个KEY 一个VAL
            {
                posX = 10;
                posY += 60;
            }
            else
                posX = 260;
        }
    }
    //确认按钮设置
    lv_obj_t *confirmBtn = lv_btn_create(displayObj[OBJ_COMPETITION], nullptr); //创建确认开关
    lv_obj_t *confirmLab = lv_label_create(confirmBtn, nullptr);                //创建确认开关文本 这里设置按钮为父级
    lv_label_set_text(confirmLab, "确\n认");
    lv_obj_set_size(confirmBtn, 40, 100); //大小设置
    //设置确定按钮和其文本框的位置
    lv_obj_align(confirmBtn, displayObj[OBJ_COMPETITION], LV_ALIGN_CENTER, -20, 0);

    //确认按钮的动作
    if (parent == nullptr)
        lv_btn_set_action(confirmBtn, LV_BTN_ACTION_CLICK, confirmBtnIncomp);
    else
        lv_btn_set_action(confirmBtn, LV_BTN_ACTION_CLICK, confirmBtnInOdom);
    //调用按钮页面
    //TODO 技能赛的动作
}
lv_res_t UserDisplay::closeAction(lv_obj_t *btn)
{
    (void)btn; /*Unused*/
    if (userDisplay->displayObj[OBJ_BTNM_SON] != nullptr)
    {
        if (!sysData->jsonVal["自动赛"]["红方&蓝方"]) //设置默认颜色
            userDisplay->displayObj[OBJ_BTNM_SON]->style_p->body.main_color = LV_COLOR_RED;
        else
            userDisplay->displayObj[OBJ_BTNM_SON]->style_p->body.main_color = LV_COLOR_BLUE;
        lv_obj_del(userDisplay->displayObj[OBJ_BTNM_SON]);
        userDisplay->displayObj[OBJ_BTNM_SON] = nullptr;
    }
    if (userDisplay->displayTask[TASK_OTHER] != nullptr)
    {
        lv_task_del(userDisplay->displayTask[TASK_OTHER]);
        userDisplay->displayTask[TASK_OTHER] = nullptr;
    }
    if (sysData->isOPcontrol == false)
        sysData->isOPcontrol = true;

    return LV_RES_INV;
}
void UserDisplay::createVersion(lv_obj_t *parent)
{

    createUserObj(OBJ_BTNM_SON, false, "版本页面", parent);
    displayObj[OBJ_BTNM_SON]->style_p->body.main_color = LV_COLOR_BLACK;
    lv_obj_t *verLab = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr); //创建LAB条
    userDisplay->ostr.clear();                                             //1：调用clear()清除当前错误控制状态，其原型为 void clear (iostate state=goodbit);
    userDisplay->ostr.str("");                                             //2：调用str("")将缓冲区清零，清除脏数据
    userDisplay->ostr << "pros版本:" << PROS_VERSION_STRING << '\n'
                      << "LVGL版本:" << LVGL_VERSION_INFO << '\n'
                      << "JSON版本:" << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR << "." << NLOHMANN_JSON_VERSION_PATCH << '\n'
                      << "UserJSON版本:" << sysData->jsonVal["json版本号"] << '\n'
                      << "NCRAPI版本:" << NCR_VERSION_STRING << std::endl;
    lv_label_set_text(verLab, userDisplay->ostr.str().c_str());
}

void UserDisplay::createStartObj(lv_obj_t *parent, size_t hor, size_t ver)
{
    static const char *startBtnm[] = {"系统信息", "日志信息", "\n",
                                      "全局参数设置", "维护信息", "\n",
                                      "PID调试", "视觉传感器设置", "\n",
                                      "ODOM测试", "版本号", ""};
    if (displayObj[BTNM_START] == nullptr)
        displayObj[BTNM_START] = lv_btnm_create(parent, nullptr); //创建按钮集群

    lv_btnm_set_map(displayObj[BTNM_START], startBtnm);
    lv_obj_set_size(displayObj[BTNM_START], hor, ver);
    lv_obj_set_y(displayObj[BTNM_START], LV_VER_RES - ver);
    lv_btnm_set_action(displayObj[BTNM_START], startBtnmAction);
}
/**
 * 按钮阵列的动作
 * @param  btnm 按钮阵列
 * @param  txt  按钮的名字
 * @return      系统值
 */
lv_res_t UserDisplay::startBtnmAction(lv_obj_t *btnm, const char *txt)
{
    (void)btnm; /*Unused*/
    if (!strcmp(txt, "系统信息"))
        userDisplay->createSysInfo(userDisplay->displayObj[BTNM_START]); //1
    else if (!strcmp(txt, "全局参数设置"))
        userDisplay->createConfig(userDisplay->displayObj[BTNM_START]); //2 创建CONFIGSET页面
    else if (!strcmp(txt, "维护信息"))
        userDisplay->createMaintenanceInfo(userDisplay->displayObj[BTNM_START]); //3
    else if (!strcmp(txt, "视觉传感器设置"))
        userDisplay->createVision(userDisplay->displayObj[BTNM_START]); //4创建视觉页面
    else if (!strcmp(txt, "版本号"))
        userDisplay->createVersion(userDisplay->displayObj[BTNM_START]); //5
    else if (!strcmp(txt, "日志信息"))
        userDisplay->createDebug(userDisplay->displayObj[BTNM_START]); //6
    else if (!strcmp(txt, "PID调试"))
        userDisplay->createPidTest(userDisplay->displayObj[BTNM_START]); //7
    else if (!strcmp(txt, "ODOM测试"))
        userDisplay->createOdom(userDisplay->displayObj[BTNM_START]); //8
    logger->info({"选择 ", txt});
    return LV_RES_INV;
}

lv_res_t UserDisplay::resetAction(lv_obj_t *btn)
{
    (void)btn; /*Unused*/
    for (auto &it : sysData->obj)
        it->resetAllSensors();
    return LV_RES_OK;
}
lv_res_t UserDisplay::saveAction(lv_obj_t *btn)
{
    json *tempData = static_cast<json *>(lv_obj_get_free_ptr(btn));
    sysData->jsonVal = *tempData;
    sysData->saveData();
    return LV_RES_OK;
}
lv_res_t UserDisplay::upDownAction(lv_obj_t *btnm, const char *txt)
{
    lv_obj_t *labPtr = static_cast<lv_obj_t *>(lv_obj_get_free_ptr(btnm));
    json *valPtr = static_cast<json *>(lv_obj_get_free_ptr(labPtr)); //从btm获取关联的valus
    static uint32_t time = 0;
    static uint32_t flag = 1;
    if (lv_tick_elaps(time) <= 200)
        flag += 10;
    else
        flag = 1;
    if (valPtr->is_number_integer())
    {
        int temp = *valPtr;
        if (!strcmp(txt, "+"))
            temp += 1;
        else if (!strcmp(txt, "-"))
            temp -= 1;
        *valPtr = temp;
    }
    else if (valPtr->is_boolean())
    {
        bool temp = *valPtr;
        if (!strcmp(txt, "+"))
            temp = !temp;
        else if (!strcmp(txt, "-"))
            temp = !temp;
        *valPtr = temp;
    }
    else if (valPtr->is_number_float())
    {
        double_t temp = *valPtr;
        if (!strcmp(txt, "+"))
            temp += 0.001 * flag;
        else if (!strcmp(txt, "-"))
            temp -= 0.001 * flag;
        std::stringstream oss;     //主要为了去掉小数点后多余的0 默认3位
        oss.setf(std::ios::fixed); //用定点格式显示浮点数,不会用科学计数法表示
        oss.precision(3);          //由于用了定点格式，设置变为了保留4位小数
        oss << temp;
        oss >> *valPtr;
    }
    lv_label_set_text(labPtr, valPtr->dump().c_str());
    time = lv_tick_get();
    return LV_RES_OK;
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
void UserDisplay::createSaveBtn(obj_flag objname, const int x, const int y, const int width, const int high) //创建退出按钮
{
    lv_obj_t *saveBtn = lv_btn_create(displayObj[objname], nullptr);
    lv_obj_set_pos(saveBtn, x, y);
    lv_obj_set_size(saveBtn, width, high);
    lv_obj_t *saveLab = lv_label_create(saveBtn, nullptr);
    lv_label_set_text(saveLab, "save");
    lv_btn_set_action(saveBtn, LV_BTN_ACTION_CLICK, saveAction);
    lv_obj_set_free_ptr(saveBtn, &_tempData);
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