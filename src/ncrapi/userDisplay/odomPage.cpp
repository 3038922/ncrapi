#include "ncrapi/ncrapi.hpp"
#include <iomanip>
//lv_obj_t *robot;
lv_obj_t *line;
std::vector<lv_point_t> point;
static void odomTask(void *param)
{
    double *config = static_cast<double *>(param);
    userDisplay->ostr.clear();                                               //1：调用clear()清除当前错误控制状态，其原型为 void clear (iostate state=goodbit);
    userDisplay->ostr.str("");                                               //2：调用str("")将缓冲区清零，清除脏数据
    userDisplay->ostr << std::fixed << std::setprecision(1) << std::setw(6); //流操纵算子
    chassis->showOdom(true);
    lv_point_t pointData = {static_cast<short>(chassis->getState().y.convert(ncrapi::millimeter) / *config), static_cast<short>(chassis->getState().x.convert(ncrapi::millimeter) / *config)};
    short flag = static_cast<short>(sysData->odomMin.convert(ncrapi::millimeter) / (*config)); //规避初始那条线的变量
    if ((point.back().x != pointData.x || point.back().y != pointData.y))
        point.push_back(pointData);
    if (point.size() > 150)
        point.erase(point.begin());
    else if (point.begin()->x == flag && point.begin()->y == flag)
        point.erase(point.begin());
    lv_line_set_points(line, point.data(), point.size()); //这里会有BUG 第二次的时候数据太多了需要清理数据
    lv_obj_align(userDisplay->otherLab, line, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_label_set_text(userDisplay->otherLab, userDisplay->ostr.str().c_str());
}
static lv_res_t autoChoseAction(lv_obj_t *btn)
{
    userDisplay->createCompe(lv_obj_get_parent(userDisplay->displayObj[OBJ_BTNM_SON]));
    return LV_RES_INV;
}
namespace ncrapi {
void UserDisplay::createOdom(lv_obj_t *parent)
{
    createUserObj(OBJ_BTNM_SON, "里程计页面", parent);
    displayObj[OBJ_BTNM_SON]->style_p->body.main_color = LV_COLOR_BLACK;
    sysData->isOPcontrol = false; //关闭遥控
    sysData->test = 6;            //进来后默认是6 自动赛模式
    //创建图像
    lv_obj_t *img = lv_img_create(displayObj[OBJ_BTNM_SON], nullptr);
    lv_img_set_src(img, &field); //将创建的文件设置为图像
    /***************创建系统设置按钮集合************/
    static const char *updown[] = {"+", "-", ""}; //创建按钮矩阵
    _tempData = sysData->jsonVal;
    size_t posX = 230, posY = 0;
    lv_obj_t *key;
    for (auto &it : _tempData["系统信息"].items())
    {

        if (it.key() != "机器人类型" && it.key() != "队伍编号" && it.key() != "用户")
        {
            /**key设置*/
            key = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr); //新建子项目的key
            lv_label_set_text(key, it.key().c_str());                 //设置子项目的key的文本信息
            lv_obj_set_pos(key, posX, posY);                          //设置key的位置
            /**value设置*/
            lv_obj_t *values = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr); //新建子项目的val
            lv_label_set_text(values, it.value().dump().c_str());                  //设置按钮的值
            lv_obj_align(values, key, LV_ALIGN_OUT_RIGHT_MID, 10, 0);              //values的位置紧贴key的位置

            /**updownBtn设置*/
            lv_obj_t *updownBtn = lv_btnm_create(displayObj[OBJ_BTNM_SON], nullptr); //新建updownBtn
            lv_btnm_set_map(updownBtn, updown);
            lv_obj_set_size(updownBtn, 50, 25);

            lv_obj_set_free_ptr(updownBtn, values);   //创建关键指针. btn关联values 这里主要为了实现刷新
            lv_obj_set_free_ptr(values, &it.value()); //创建关键指针. values关联key

            lv_obj_align(updownBtn, values, LV_ALIGN_OUT_RIGHT_MID, 30, 0); //设置updownbtn位置
            lv_btnm_set_action(updownBtn, upDownAction);                    //设置updown按钮动作
            posY += 30;
        }
    }
    /***************创建系统设置按钮集合************/
    line = lv_line_create(img, nullptr); //创建一条线
    static lv_style_t styleLine;
    lv_style_copy(&styleLine, &lv_style_plain);
    styleLine.line.color = LV_COLOR_GREEN;
    styleLine.line.width = 2;
    lv_line_set_style(line, &styleLine);
    //创建文本栏目
    otherLab = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr); //创建基于INFOObj的标签
    lv_label_set_recolor(otherLab, true);
    //创建自动赛选择按钮
    lv_obj_t *autoChoseBtn = lv_btn_create(displayObj[OBJ_BTNM_SON], nullptr);
    lv_obj_set_pos(autoChoseBtn, 230, LV_VER_RES - 60);
    lv_obj_set_size(autoChoseBtn, 100, 25);
    lv_btn_set_action(autoChoseBtn, LV_BTN_ACTION_CLICK, autoChoseAction);
    lv_obj_t *autoChoseLab = lv_label_create(autoChoseBtn, nullptr);
    lv_label_set_text(autoChoseLab, "自动赛");

    //创建保存按钮
    createSaveBtn(OBJ_BTNM_SON);
    //创建一些比例之类的参数
    static double odomConfig = sysData->siteSize.convert(millimeter) / lv_obj_get_height(img); //比例
    //创建赛场边长栏目
    lv_obj_t *siteSizeLab = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr);
    lv_obj_set_pos(siteSizeLab, 230, posY); //在最后一个栏目最下方
    std::string siteStr = "赛场边长: " + std::to_string(sysData->siteSize.convert(millimeter));
    lv_label_set_text(siteSizeLab, siteStr.c_str());
    createUserTask(TASK_OTHER, odomTask, 100, "系统信息", &odomConfig);
}
} // namespace ncrapi