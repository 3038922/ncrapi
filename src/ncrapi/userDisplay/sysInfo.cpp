/**
 * @Author: 陈昱安
 * @Date:   2018-10-22T22:01:37+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-10-28T22:09:06+08:00
 */
#include "ncrapi/system/sysUser.hpp"
#include "ncrapi/userDisplay/userDisplay.hpp"
#include <iomanip>
static void sysInfoTask(void *param)
{
    (void)param; /*Unused*/
    uint16_t temp = lv_tabview_get_tab_act(userDisplay->displayObj[OBJ_BTNM_SON]);
    userDisplay->ostr.clear();                               //1：调用clear()清除当前错误控制状态，其原型为 void clear (iostate state=goodbit);
    userDisplay->ostr.str("");                               //2：调用str("")将缓冲区清零，清除脏数据
    userDisplay->ostr << std::fixed << std::setprecision(1); //流操纵算子  //std::setw(6) 宽度控制 //精度控制会影响后面的输出 std::setprecision(1)
    if (temp == 0)
        for (auto &it : sysData->obj)
            it->showSensor();
    else
        sysData->obj[temp - 1]->showDetailedInfo();
    lv_label_set_text(userDisplay->otherLab, userDisplay->ostr.str().c_str());
}

/**
 * 标签栏按下后的动作
 * @param tab 标签
 * @param x   几号标签 0开始的
 */
static lv_res_t tabChose(lv_obj_t *tab, uint16_t x)
{
    //这个似乎不能删
    return LV_RES_OK;
}
namespace ncrapi {
void UserDisplay::createSysInfo(lv_obj_t *parent)
{

    if (displayObj[OBJ_BTNM_SON] == nullptr)
        displayObj[OBJ_BTNM_SON] = lv_tabview_create(parent, nullptr);
    displayObj[OBJ_BTNM_SON]->style_p->body.main_color = LV_COLOR_BLACK;
    std::vector<lv_obj_t *> tabs;
    tabs.push_back(lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], "概览"));                     //先建立一个概览
    for (auto &it : sysData->obj)                                                             //遍历项目名字
        tabs.push_back(lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], it->showName().c_str())); //添加项目

    /*当选项卡按下后进行的操作*/
    lv_tabview_set_tab_load_action(displayObj[OBJ_BTNM_SON], tabChose);

    //创建文本条
    otherLab = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr); //创建基于INFOObj的标签
    lv_obj_set_y(otherLab, 30);                                    //设置文本条位置

    //重置传感器按钮
    createResetBtn(OBJ_BTNM_SON);
    //退出按钮
    createExitBtn(OBJ_BTNM_SON);
    createUserTask(TASK_OTHER, sysInfoTask, 100, "系统信息");
}
} // namespace ncrapi