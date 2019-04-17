#include "ncrapi/system/sysBase.hpp"
#include "ncrapi/userDisplay/userDisplay.hpp"

std::array<std::pair<lv_obj_t *, bool>, 6> flags = {std::make_pair(nullptr, false)};
namespace ncrapi
{
static void flagTask(void *param)
{
    for (auto &it : flags)
        if (it.second)
            lv_led_on(it.first);
        else
            lv_led_off(it.first);
}

void UserDisplay::createCustomTest(lv_obj_t *parent)
{
    createUserObj(OBJ_BTNM_SON, "自定义测试", parent);
    sysData->isOPcontrol = false;

    int x = lv_obj_get_width(displayObj[OBJ_BTNM_SON]) / 3 / 2 - 21, y = 20, count = 0; //每个LED 默认宽度是42

    for (auto &it : flags)
    {
        it.first = lv_led_create(displayObj[OBJ_BTNM_SON], nullptr);
        lv_obj_set_pos(it.first, x, y);
        count++;
        if (count % 2 == 0)
        {
            x += lv_obj_get_width(displayObj[OBJ_BTNM_SON]) / 3;
            y = 20;
        }
        else
            y += 80;
    }
    createUserTask(TASK_OTHER, flagTask, 100, "自定义测试");
}
} // namespace ncrapi