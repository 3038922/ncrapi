#include "ncrapi/system/sysBase.hpp"
#include "ncrapi/userDisplay/userDisplay.hpp"

namespace ncrapi
{

void UserDisplay::createConfig(lv_obj_t *parent)
{

    if (displayObj[OBJ_BTNM_SON] == nullptr)
        displayObj[OBJ_BTNM_SON] = lv_tabview_create(parent, nullptr);
    std::vector<lv_obj_t *> tabs;
    std::vector<lv_obj_t *> key;
    std::vector<lv_obj_t *> values;
    std::vector<lv_obj_t *> updownBtn;
    static const char *updown[] = {"+", "-", ""};
    _tempData = sysData->jsonVal;
    for (auto &it : sysData->obj) //遍历项目名字
    {
        size_t posX = 0, posY = 10;

        /*创建TABS*/
        tabs.push_back(lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], it->showName().c_str())); //添加项目
        lv_page_set_scrl_fit(tabs.back(), false, false);
        lv_page_set_scrl_height(tabs.back(), 600); //设置页面可滚动部分的高度
        for (auto &it : _tempData[it->showName()].items())
        {
            if (it.key() != "PID参数")
                for (auto &it1 : it.value().items())
                {
                    key.push_back(lv_label_create(tabs.back(), nullptr)); //KEY就一个
                    lv_obj_set_pos(key.back(), posX, posY);               //设置key的位置
                    lv_label_set_text(key.back(), it1.key().c_str());     //设置KEY的文本 //静态文本有问题
                    int pos = 0;
                    for (auto &it2 : it1.value().items())
                    {
                        if (!it1.value().is_array()) //如果他不是数组就创建key
                        {
                            /**key设置*/
                            key.push_back(lv_label_create(tabs.back(), nullptr)); //新建子项目的key
                            lv_label_set_text(key.back(), it2.key().c_str());     //设置子项目的key的文本信息
                            if (pos == 0)
                                lv_obj_align(key.back(), *(key.end() - 2), LV_ALIGN_OUT_RIGHT_MID, 10, 0); //子项目key的位置紧贴上一个key的位置
                            else
                                lv_obj_align(key.back(), updownBtn.back(), LV_ALIGN_OUT_RIGHT_MID, 10, 0); //子项目key的位置紧贴上一个upDownBtn的位置

                            /**value设置*/
                            values.push_back(lv_label_create(tabs.back(), nullptr));                //新建子项目的val
                            lv_label_set_text(values.back(), it2.value().dump().c_str());           //设置按钮的值
                            lv_obj_align(values.back(), key.back(), LV_ALIGN_OUT_RIGHT_MID, 10, 0); //values的位置紧贴key的位置
                        }
                        else //是数组
                        {
                            /**value设置*/
                            values.push_back(lv_label_create(tabs.back(), nullptr));      //新建子项目的val
                            lv_label_set_text(values.back(), it2.value().dump().c_str()); //设置按钮的值
                            if (pos == 0)
                                lv_obj_align(values.back(), key.back(), LV_ALIGN_OUT_RIGHT_MID, 10, 0); //values的位置紧贴key的位置
                            else
                                lv_obj_align(values.back(), updownBtn.back(), LV_ALIGN_OUT_RIGHT_MID, 10, 0); //values的位置紧贴key的位置
                        }

                        /**updownBtn设置*/
                        updownBtn.push_back(lv_btnm_create(tabs.back(), nullptr)); //新建updownBtn
                        lv_btnm_set_map(updownBtn.back(), updown);
                        lv_obj_set_size(updownBtn.back(), 50, 25);

                        lv_obj_set_free_ptr(updownBtn.back(), values.back()); //创建关键指针. btn关联values 这里主要为了实现刷新
                        lv_obj_set_free_ptr(values.back(), &it2.value());     //创建关键指针. values关联key

                        lv_obj_align(updownBtn.back(), values.back(), LV_ALIGN_OUT_RIGHT_MID, 10, 0); //设置updownbtn位置
                        lv_btnm_set_action(updownBtn.back(), upDownAction);                           //设置updown按钮动作
                        pos++;
                    };
                    if (lv_obj_get_x(updownBtn.back()) > 230) //key表示偶数换行 it.szie()表示每行总共有2个数一个KEY 一个VAL
                    {
                        posX = 0;
                        posY += 35;
                    }
                    else
                        posX = 230;
                }
        }
    }

    //创建保存按钮
    createSaveBtn(OBJ_BTNM_SON);
    //退出按钮
    createExitBtn(OBJ_BTNM_SON);
}
} // namespace ncrapi