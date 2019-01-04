#include "ncrapi/userDisplay/userDisplay.hpp"

namespace ncrapi
{
void UserDisplay::createOdom(lv_obj_t *parent)
{
    createUserObj(OBJ_BTNM_SON, false, "odomPage", parent);
    //创建图像
    lv_obj_t *img = lv_img_create(displayObj[OBJ_BTNM_SON], nullptr);
    lv_img_set_src(img, &field); //将创建的文件设置为图像
}
} // namespace ncrapi