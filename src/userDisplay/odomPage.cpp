#include "ncrapi/ncrapi.hpp"
#include <iomanip>

namespace ncrapi
{
void UserDisplay::createOdom(lv_obj_t *parent)
{
    createUserObj(OBJ_BTNM_SON, false, "里程计页面", parent);
    //创建图像
    lv_obj_t *img = lv_img_create(displayObj[OBJ_BTNM_SON], nullptr);
    lv_img_set_src(img, &field); //将创建的文件设置为图像
}
} // namespace ncrapi