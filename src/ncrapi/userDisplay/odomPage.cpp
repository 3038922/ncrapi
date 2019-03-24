#include "ncrapi/ncrapi.hpp"
#include <iomanip>
static lv_res_t autoChonseAction(lv_obj_t *btn)
{
    userDisplay->createCompe(lv_obj_get_parent(userDisplay->displayObj[OBJ_BTNM_SON]));
    return LV_RES_INV;
}
namespace ncrapi
{
void UserDisplay::createOdom(lv_obj_t *parent)
{
    createUserObj(OBJ_BTNM_SON, false, "里程计页面", parent);
    displayObj[OBJ_BTNM_SON]->style_p->body.main_color = LV_COLOR_BLACK;
    sysData->isOPcontrol = false; //关闭遥控
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

    //创建文本栏目
    otherLab = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr); //创建基于INFOObj的标签
    lv_label_set_recolor(otherLab, true);
    //创建自动赛选择按钮
    lv_obj_t *autoChoseBtn = lv_btn_create(displayObj[OBJ_BTNM_SON], nullptr);
    lv_obj_set_pos(autoChoseBtn, 230, LV_VER_RES - 50);
    lv_obj_set_size(autoChoseBtn, 100, 25);
    lv_btn_set_action(autoChoseBtn, LV_BTN_ACTION_CLICK, autoChonseAction);
    lv_obj_t *autoChoseLab = lv_label_create(autoChoseBtn, nullptr);
    lv_label_set_text(autoChoseLab, "自动赛");

    //创建保存按钮
    createSaveBtn(OBJ_BTNM_SON);
}
} // namespace ncrapi