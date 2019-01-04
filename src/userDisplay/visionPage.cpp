
#include "ncrapi/system/systemData.hpp"

namespace ncrapi
{
void UserDisplay::createVision(lv_obj_t *parent)
{

    if (displayObj[OBJ_BTNM_SON] == nullptr)
        displayObj[OBJ_BTNM_SON] = lv_tabview_create(parent, nullptr);
    lv_obj_t *builtInSigTab = lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], "内置签名");

    lv_obj_t *getSigTab = lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], "按数据0");
    lv_obj_t *getSizeTab = lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], "按数据1");
    lv_obj_t *getCodeTab = lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], "按数据2");
    /*当选项卡按下后进行的操作*/

    //退出按钮
    createExitBtn(OBJ_BTNM_SON);
}
} // namespace ncrapi