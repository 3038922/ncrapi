#include "ncrapi/system/systemData.hpp"
lv_obj_t *debugLab;
static lv_res_t clearAction(lv_obj_t *btn)
{
    (void)btn; /*Unused*/
    if (sysData->debugFile != nullptr)
    {
        fclose(sysData->debugFile);                        //先关闭
        sysData->debugFile = fopen("/usd/debug.txt", "w"); //清空 先关闭写模式再以写模式打开
        fclose(sysData->debugFile);                        //再关闭
        lv_label_set_text(debugLab, "");                   //清空文本
        sysData->debugFile = fopen("/usd/debug.txt", "a"); //再继续以写模式打开
    }
    else
        std::cerr << "debug 文件打开错误,请检查SD卡!" << std::endl;
    return LV_RES_INV;
}
namespace ncrapi
{
void UserDisplay::createDebug(lv_obj_t *parent)
{

    createUserObj(OBJ_BTNM_SON, false, "机器人检测页面", parent);
    std::string debugStr;
    sysData->showDebugData(debugStr);
    debugLab = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr);
    lv_label_set_long_mode(debugLab, LV_LABEL_LONG_EXPAND);
    lv_label_set_text(debugLab, debugStr.c_str());

    //创建清空按钮
    lv_obj_t *clearBtn = lv_btn_create(displayObj[OBJ_BTNM_SON], nullptr);
    lv_obj_set_pos(clearBtn, LV_HOR_RES - 140, LV_VER_RES - 50);
    lv_obj_set_size(clearBtn, 50, 25);
    lv_obj_t *clearLab = lv_label_create(clearBtn, nullptr);
    lv_label_set_text(clearLab, "clear");
    lv_btn_set_action(clearBtn, LV_BTN_ACTION_CLICK, clearAction);
}
} // namespace ncrapi