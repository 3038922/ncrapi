#include "ncrapi/system/systemData.hpp"

/**
 * 标签栏按下后的动作
 * @param tab 标签
 * @param x   几号标签
 */
static void tabChose(lv_obj_t *tab, uint16_t x)
{
}
namespace ncrapi
{
void UserDisplay::createPidTest(lv_obj_t *parent)
{
    if (displayObj[OBJ_BTNM_SON] == nullptr)
        displayObj[OBJ_BTNM_SON] = lv_tabview_create(parent, nullptr);

    lv_obj_t *frTab = lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], "前后PID调试");
    lv_obj_t *rotateTab = lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], "左右PID调试");
    /*当选项卡按下后进行的操作*/
    lv_tabview_set_tab_load_action(displayObj[OBJ_BTNM_SON], tabChose);
    //退出按钮
    createExitBtn(OBJ_BTNM_SON);
}

} // namespace ncrapi