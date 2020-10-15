#include "ncrapi/ncrapi.hpp"
#include <iomanip>
/**
 * 标签栏按下后的动作
 * @param tab 标签
 * @param x   几号标签
 */
static lv_res_t pidChoseAction(lv_obj_t *tab, uint16_t x)
{
    (void)tab;

    if (x == 0)
    {
        sysData->test = 0;            //PID速度环测试
        sysData->pidTestTarget = 0.5; //机器最大速度一般是1.5米多每秒
        lv_obj_set_pos(userDisplay->otherLab, 180, 30);
        lv_obj_set_hidden(userDisplay->otherLab, false); //设置隐藏
    }
    else if (x == 1)
    {
        sysData->test = 1;            //前后测试
        sysData->pidTestTarget = 500; //进来默认值是500
        lv_obj_set_pos(userDisplay->otherLab, 180, 30);
        lv_obj_set_hidden(userDisplay->otherLab, false); //设置隐藏
    }
    else if (x == 2)
    {
        sysData->test = 2;           //左右测试
        sysData->pidTestTarget = 45; //进来默认值是45
        lv_obj_set_pos(userDisplay->otherLab, 180, 30);
        lv_obj_set_hidden(userDisplay->otherLab, false); //设置隐藏
    }
    else if (x == 3)
    {
        sysData->test = 3;           //左右视觉+FGYRO 测试
        sysData->pidTestTarget = 45; //进来默认值是45
        lv_obj_set_pos(userDisplay->otherLab, 180, 30);
        lv_obj_set_hidden(userDisplay->otherLab, false); //设置隐藏
    }
    else if (x == 4)
    {
        sysData->test = 4; //pid的正方形测试
        lv_obj_set_pos(userDisplay->otherLab, 0, 30);
        lv_obj_set_hidden(userDisplay->otherLab, false); //设置隐藏
    }
    else if (x == 5) //数据展示
    {
        lv_obj_t *dataLab = static_cast<lv_obj_t *>(lv_obj_get_free_ptr(lv_tabview_get_tab(tab, x)));
        sysData->test = 0;
        lv_obj_set_hidden(userDisplay->otherLab, true);
        std::ostringstream ostr;
        ostr << std::fixed << std::setprecision(1) << std::setw(6); //流操纵算子
        for (auto &it : sysData->pidDebugData)
            ostr << " error:" << std::get<0>(it) << " output:" << std::get<1>(it) << " p:" << std::get<2>(it) << " i:" << std::get<3>(it) << " d:" << std::get<4>(it) << std::endl;
        lv_label_set_text(dataLab, ostr.str().c_str());      //这里使用userDisplay的ostr会不会被多线程影响?
        lv_label_set_long_mode(dataLab, LV_LABEL_LONG_ROLL); //设置文本滚动 //系统有BUG 暂时先用这个
        lv_obj_set_size(dataLab, 440, 200);
        lv_label_set_anim_speed(dataLab, 50); //设置滚动条速度 lvgl5.1会有BUG
    }
    else //图表显示
    {
        std::vector<lv_chart_series_t *> ser;
        lv_obj_t *chart = static_cast<lv_obj_t *>(lv_obj_get_free_ptr(lv_tabview_get_tab(tab, x)));
        int temp;
        sysData->pidTestTarget < 50 ? temp = 50 : temp = sysData->pidTestTarget; //直接转换
        lv_chart_set_range(chart, -temp, temp);                                  //进来默认范围 设置图标范围
        ser.push_back(lv_chart_add_series(chart, LV_COLOR_RED));                 //时间线
        ser.push_back(lv_chart_add_series(chart, LV_COLOR_GREEN));               //误差线
        ser.push_back(lv_chart_add_series(chart, LV_COLOR_BLUE));                //OUTPUT线
        sysData->test = 0;
        lv_obj_set_hidden(userDisplay->otherLab, true);
        size_t count = 0;
        //     pros::delay(100);
        for (auto &it : sysData->pidDebugData)
        {
            ser[0]->points[count] = std::get<0>(it); //时间线
            ser[1]->points[count] = std::get<1>(it); //误差线 error
            ser[2]->points[count] = std::get<2>(it); //output线
            count++;
        }
        //刷新图标不能放这里
    }
    return LV_RES_OK;
}
static lv_res_t tartgetBtnmAction(lv_obj_t *btnm, const char *txt)
{
    (void)btnm; /*Unused*/
    std::string temp = txt;
    if (temp == "全场")
        sysData->pidTestTarget = sysData->siteSize.convert(ncrapi::millimeter) - sysData->robotLength.convert(ncrapi::millimeter) - 40;
    else
        sysData->pidTestTarget = atoi(txt);
    return LV_RES_OK;
}
static lv_res_t squareAction(lv_obj_t *btnm, const char *txt)
{
    (void)btnm; /*Unused*/
    if (!strcmp(txt, "PID正方形"))
        sysData->test = 4;
    else if (!strcmp(txt, "ODOM正方形"))
        sysData->test = 5;
    return LV_RES_OK;
}
static void pidTask(void *param)
{
    (void)param;
    std::string tempStr = "Time1:" + std::to_string(sysData->pidData[0].first.convert(ncrapi::millisecond)) + " error1:" + std::to_string(sysData->pidData[0].second) + "\n";
    tempStr += "Time2:" + std::to_string(sysData->pidData[1].first.convert(ncrapi::millisecond)) + " error2:" + std::to_string(sysData->pidData[1].second) + "\n";
    userDisplay->ostr.clear();                                               //1：调用clear()清除当前错误控制状态，其原型为 void clear (iostate state=goodbit);
    userDisplay->ostr.str("");                                               //2：调用str("")将缓冲区清零，清除脏数据
    userDisplay->ostr << std::fixed << std::setprecision(1) << std::setw(6); //流操纵算子
    chassis->showOdom();
    tempStr += userDisplay->ostr.str();
    lv_label_set_text(userDisplay->otherLab, tempStr.c_str());
}
namespace ncrapi {
void UserDisplay::createPidTest(lv_obj_t *parent)
{

    if (displayObj[OBJ_BTNM_SON] == nullptr)
        displayObj[OBJ_BTNM_SON] = lv_tabview_create(parent, nullptr);
    displayObj[OBJ_BTNM_SON]->style_p->body.main_color = LV_COLOR_BLACK;
    sysData->isOPcontrol = false; //关闭遥控
    sysData->test = 0;            //进来后默认是0
    sysData->pidTestTarget = 0.5; //进来默认值是500
    std::vector<lv_obj_t *> tabs; //标签页
    //创建匹配的PID测试目标选项
    static std::vector<const char *> testBtnText[] = {{"0", "0.5", "1", "1.5", ""},
                                                      {"100", "500", "1000", "1800", "全场", ""},
                                                      {"10", "45", "90", "180", ""},
                                                      {"10", "45", "90", "180", ""},
                                                      {"PID正方形", "ODOM正方形", ""}};
    size_t flag = 0;
    static const char *updown[] = {"+", "-", ""}; //创建按钮矩阵
    _tempData = sysData->jsonVal;
    std::string mode;
    if (sysData->jsonVal["底盘"]["参数"]["模式"] == 0)
    {
        mode = "模式0";
        chassis->setMode(0);
    }
    else
    {
        mode = "模式1";
        chassis->setMode(1);
    }
    for (auto &it : _tempData["底盘"]["PID参数"][mode].items()) //很精髓的创建方式
    {
        tabs.push_back(lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], it.key().c_str())); //添加项目
        lv_page_set_scrl_fit(tabs.back(), false, false);
        lv_page_set_scrl_height(tabs.back(), 600); //设置页面可滚动部分的高度
        size_t posX = 0, posY = 0;
        for (auto &it1 : it.value().items())
        {
            /**key设置*/
            lv_obj_t *key = lv_label_create(tabs.back(), nullptr); //新建子项目的key
            lv_label_set_text(key, it1.key().c_str());             //设置子项目的key的文本信息
            lv_obj_set_pos(key, posX, posY);                       //设置key的位置
            /**value设置*/
            lv_obj_t *values = lv_label_create(tabs.back(), nullptr); //新建子项目的val
            lv_label_set_text(values, it1.value().dump().c_str());    //设置按钮的值
            lv_obj_align(values, key, LV_ALIGN_OUT_RIGHT_MID, 10, 0); //values的位置紧贴key的位置

            /**updownBtn设置*/
            lv_obj_t *updownBtn = lv_btnm_create(tabs.back(), nullptr); //新建updownBtn
            lv_btnm_set_map(updownBtn, updown);
            lv_obj_set_size(updownBtn, 50, 25);

            lv_obj_set_free_ptr(updownBtn, values);    //创建关键指针. btn关联values 这里主要为了实现刷新
            lv_obj_set_free_ptr(values, &it1.value()); //创建关键指针. values关联key

            lv_obj_align(updownBtn, values, LV_ALIGN_OUT_RIGHT_MID, 30, 0); //设置updownbtn位置
            lv_btnm_set_action(updownBtn, upDownAction);                    //设置updown按钮动作
            posY += 30;
        }
        //创建匹配的PID测试目标选项

        lv_obj_t *testBtn = lv_btnm_create(tabs.back(), nullptr); //创建按钮集群
        lv_btnm_set_map(testBtn, testBtnText[flag].data());
        lv_obj_set_size(testBtn, 230, 30);
        lv_btnm_set_action(testBtn, tartgetBtnmAction);
        lv_btnm_set_toggle(testBtn, true, 1);
        lv_obj_align(testBtn, tabs.back(), LV_ALIGN_CENTER, 50, 30); //按钮的位置设置
        flag++;
    }
    tabs.push_back(lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], "正方形测试")); //添加数据烂
    lv_page_set_scrl_fit(tabs.back(), false, false);
    lv_page_set_scrl_height(tabs.back(), 200);                //设置页面可滚动部分的高度
    lv_obj_t *testBtn = lv_btnm_create(tabs.back(), nullptr); //创建按钮集群
    lv_btnm_set_map(testBtn, testBtnText[flag].data());       //这里flag是最后一个
    lv_obj_set_size(testBtn, 230, 30);
    lv_btnm_set_action(testBtn, squareAction);
    lv_btnm_set_toggle(testBtn, true, 0);
    lv_obj_align(testBtn, tabs.back(), LV_ALIGN_CENTER, 0, 30); //按钮的位置设置

    tabs.push_back(lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], "数据")); //添加数据烂
    lv_page_set_scrl_fit(tabs.back(), false, false);
    lv_page_set_scrl_height(tabs.back(), 200);                 //设置页面可滚动部分的高度
    lv_obj_t *dataLab = lv_label_create(tabs.back(), nullptr); //创建控制台文本条
    lv_obj_set_free_ptr(tabs.back(), dataLab);                 //设置当前页面关联数据lab

    tabs.push_back(lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], "图表")); //添加图表烂
    lv_page_set_scrl_fit(tabs.back(), false, false);
    lv_page_set_scrl_height(tabs.back(), 200); //设置页面可滚动部分的高度
    lv_tabview_set_tab_load_action(displayObj[OBJ_BTNM_SON], pidChoseAction);
    //创建图表
    lv_obj_t *chart = lv_chart_create(tabs.back(), nullptr);
    lv_obj_set_size(chart, 460, 160);
    //lv_obj_align(chart, updownBtn.front(), LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_POINT);        //设置模式 点模式 线模式太坑了
    lv_chart_set_series_opa(chart, LV_OPA_70);            /*Opacity of the data series*/
    lv_chart_set_series_width(chart, 1);                  /*Line width and point radious*/
    lv_obj_t *chartLab = lv_label_create(chart, nullptr); //创建图标文本条
    lv_obj_set_pos(chartLab, 375, 0);
    lv_chart_set_point_count(chart, 127);
    lv_label_set_text(chartLab, "红线:时间\n绿线:误差\n蓝线:输出");
    lv_obj_set_free_ptr(tabs.back(), chart); //设置当前页面管关联图表

    //创建保存按钮
    createSaveBtn(OBJ_BTNM_SON);
    //退出按钮
    createExitBtn(OBJ_BTNM_SON);
    otherLab = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr); //线程lab
    lv_obj_set_pos(userDisplay->otherLab, 180, 30);                //第一次进来时候的位置
    //刷新图表
    lv_chart_refresh(chart); /*Required after direct set*/
    createUserTask(TASK_OTHER, pidTask, 100, "PID测试");
} // namespace ncrapi

} // namespace ncrapi