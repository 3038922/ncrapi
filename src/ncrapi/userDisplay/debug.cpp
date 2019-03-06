#include "ncrapi/system/logger.hpp"
#include "ncrapi/userDisplay/userDisplay.hpp"
#include <string>

static lv_res_t clearAction(lv_obj_t *btn)
{
    lv_obj_t *debugLab = static_cast<lv_obj_t *>(lv_obj_get_free_ptr(btn));
    int level = lv_obj_get_free_num(debugLab);
    if (logger->clean(static_cast<ncrapi::Logger::LEVEL>(level)))
        lv_label_set_text(debugLab, "");
    return LV_RES_INV;
}
namespace ncrapi
{
void UserDisplay::createDebug(lv_obj_t *parent)
{
    std::vector<std::string> Level = {"错误", "警告", "调试", "信息"};
    if (displayObj[OBJ_BTNM_SON] == nullptr)
        displayObj[OBJ_BTNM_SON] = lv_tabview_create(parent, nullptr);
    displayObj[OBJ_BTNM_SON]->style_p->body.main_color = LV_COLOR_BLACK;
    for (size_t i = 0; i <= logger->getLevel(); i++)
    {
        lv_obj_t *tabs = lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], Level[i].c_str());
        std::string debugStr;
        logger->showData(static_cast<Logger::LEVEL>(i), debugStr);
        lv_obj_t *debugLab = lv_label_create(tabs, nullptr); //创建文本条
        lv_label_set_long_mode(debugLab, LV_LABEL_LONG_ROLL);
        lv_label_set_anim_speed(debugLab, 1);
        lv_label_set_array_text(debugLab, debugStr.c_str(), debugStr.size());
        lv_obj_set_size(debugLab, LV_HOR_RES - 30, LV_VER_RES - 20);
        lv_label_set_recolor(debugLab, true);

        //创建清空按钮
        lv_obj_t *clearBtn = lv_btn_create(tabs, nullptr);
        lv_obj_set_pos(clearBtn, LV_HOR_RES - 140, LV_VER_RES - 79);
        lv_obj_set_size(clearBtn, 50, 25);
        lv_obj_t *clearLab = lv_label_create(clearBtn, nullptr);
        lv_label_set_text(clearLab, "clear");
        lv_btn_set_action(clearBtn, LV_BTN_ACTION_CLICK, clearAction);
        lv_obj_set_free_ptr(clearBtn, debugLab);
        lv_obj_set_free_num(debugLab, i);
    }
    //退出按钮
    createExitBtn(OBJ_BTNM_SON);
}
} // namespace ncrapi