#include "ncrapi/system/configSet.hpp"
namespace ncrapi
{
void UserDisplay::createConfig(lv_obj_t *parent)
{

    createUserObj(OBJ_BTNM_SON, false, "configPage", parent);
    lv_obj_t *configLab = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr); //创建LAB条

    userDisplay->ostr.clear(); //1：调用clear()清除当前错误控制状态，其原型为 void clear (iostate state=goodbit);
    userDisplay->ostr.str(""); //2：调用str("")将缓冲区清零，清除脏数据
    //config->showConfig();
    std::string temp = userDisplay->ostr.str();
    lv_label_set_text(configLab, temp.c_str());
    //创建保存按钮

    //读取SD卡内容

    //设置SD卡内容 需要创建一系列按钮
}
} // namespace ncrapi