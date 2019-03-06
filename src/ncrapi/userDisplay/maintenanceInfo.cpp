
#include "ncrapi/system/sysBase.hpp"
#include "ncrapi/userDisplay/userDisplay.hpp"

namespace ncrapi
{
void UserDisplay::createMaintenanceInfo(lv_obj_t *parent)
{
    if (displayObj[OBJ_BTNM_SON] == nullptr)
        displayObj[OBJ_BTNM_SON] = lv_tabview_create(parent, nullptr);
    displayObj[OBJ_BTNM_SON]->style_p->body.main_color = LV_COLOR_BLACK;
    _tempData = sysData->jsonVal;
    for (auto &it : sysData->obj) //遍历项目名字
    {
        size_t posX = 0, posY = 0;
        /*创建TABS*/
        lv_obj_t *tabs = lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], it->showName().c_str());
        lv_page_set_scrl_fit(tabs, false, false);
        lv_page_set_scrl_height(tabs, 600); //设置页面可滚动部分的高度
        for (auto &it : _tempData[it->showName()].items())
        {
            if (it.key() == "马达" || it.key() == "ADI")
                for (auto &it1 : it.value().items())
                {
                    lv_obj_t *lmeter = lv_lmeter_create(tabs, nullptr); //绘制滑块条
                    lv_lmeter_set_range(lmeter, 0, 100);
                    lv_lmeter_set_value(lmeter, 100);
                    lv_obj_set_pos(lmeter, posX, posY);
                    lv_obj_set_size(lmeter, 85, 85); //设置滑块条大小
                    lv_obj_t *val = lv_label_create(lmeter, nullptr);
                    lv_obj_align(val, nullptr, LV_ALIGN_CENTER, 0, 0);
                    lv_label_set_text(val, std::to_string(lv_lmeter_get_value(lmeter)).c_str()); //设置KEY的文本 //静态文本有问题
                    lv_obj_t *key = lv_label_create(lmeter, nullptr);
                    lv_obj_align(key, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
                    lv_label_set_text(key, it1.key().c_str()); //设置KEY的文本 //静态文本有问题
                    if (lv_obj_get_x(lmeter) >= 360)
                    {
                        posX = 0;
                        posY += 90;
                    }
                    else
                        posX += 120;
                }
        }
    }

    //创建保存按钮
    createSaveBtn(OBJ_BTNM_SON);
    //退出按钮
    createExitBtn(OBJ_BTNM_SON);
}
} // namespace ncrapi