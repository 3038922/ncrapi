
#include "ncrapi/userDisplay/userDisplay.hpp"

namespace ncrapi {
void UserDisplay::createCustomTest(lv_obj_t *parent)
{
    createUserObj(OBJ_BTNM_SON, false, "自定义测试页面", parent);

    //测试自定义绘图
}
} // namespace ncrapi