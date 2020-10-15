
#include "ncrapi/system/sysUser.hpp"
#include "ncrapi/system/visionData.hpp"
#include "ncrapi/userDisplay/userDisplay.hpp"
bool side; //0红方 1蓝方
std::array<lv_obj_t *, ncrapi::BASE_DATASUM> obj;
lv_style_t red;
lv_style_t blue;
lv_style_t green;
lv_style_t orange;
lv_style_t violet;
bool displayData = 0;

//滑块动作
static lv_res_t expAction(lv_obj_t *slider)
{
    lv_obj_t *lab = static_cast<lv_obj_t *>(lv_obj_get_free_ptr(slider));
    json *tempData = static_cast<json *>(lv_obj_get_free_ptr(lab)); //从lab取得tempData的指针.
    *tempData = lv_slider_get_value(slider);
    visionData->setExposure(*tempData);
    lv_label_set_text(lab, std::to_string(visionData->getExposure()).c_str());
    return LV_RES_OK;
}
lv_res_t rangeAction(lv_obj_t *btnm, const char *txt)
{
    lv_obj_t *labPtr = static_cast<lv_obj_t *>(lv_obj_get_free_ptr(btnm));
    json *valPtr = static_cast<json *>(lv_obj_get_free_ptr(labPtr)); //从btm获取关联的values
    float temp = *valPtr;
    if (!strcmp(txt, "+"))
        temp += 0.1;
    else if (!strcmp(txt, "-"))
        temp -= 0.1;
    std::stringstream oss;     //主要为了去掉小数点后多余的0 默认3位
    oss.setf(std::ios::fixed); //用定点格式显示浮点数,不会用科学计数法表示
    oss.precision(1);          //由于用了定点格式，设置变为了保留1位小数
    oss << temp;
    oss >> *valPtr;
    lv_label_set_text(labPtr, valPtr->dump().c_str());
    visionData->setSig(userDisplay->_tempData, lv_tabview_get_tab_act(userDisplay->displayObj[OBJ_BTNM_SON]) - 1);
    return LV_RES_OK;
}

static void visionTask(void *param)
{
    (void)param; /*Unused*/
    std::string str;

    std::array<pros::vision_object_s_t, 4> temp;
    if (!displayData)
    {
        int i = 0;
        for (auto &it : visionData->getData())
        {
            switch (it.signature)
            {
                case 1:
                    if (!side) //红方打蓝旗 这里要反一下 直接json搜索太慢了吧
                        userDisplay->drawRectangle(obj[i], i, it, &red);
                    else
                        userDisplay->drawRectangle(obj[i], i, it, &blue);
                    break;
                case 2:
                    userDisplay->drawRectangle(obj[i], i, it, &green);
                    break;
                case 3:
                    userDisplay->drawRectangle(obj[i], i, it, &orange);
                    break;
                case 4:
                    userDisplay->drawRectangle(obj[i], i, it, &violet);
                    break;
                default:
                    lv_obj_set_hidden(obj[i], true);
                    break;
            }
            i++;
        }
        visionData->showBaseData(str);
    }
    else
    {
        ;
        // int i = 0;
        // for (auto &it : visionData->getShootData())
        // {
        //     if (visionData->getColorMode() == 0)
        //     {
        //         if (it.signature == 1)
        //             if (!side) //红方打蓝旗 这里要反一下 直接json搜索太慢了吧
        //                 userDisplay->drawRectangle(obj[i], i, it, &blue);
        //             else
        //                 userDisplay->drawRectangle(obj[i], i, it, &red);
        //         else if (it.signature == 2)
        //             userDisplay->drawRectangle(obj[i], i, it, &green);
        //         else
        //             lv_obj_set_hidden(obj[i], true);
        //     }
        //     else
        //     {
        //         if (it.signature == 10)
        //             if (!side) //红方打蓝旗 这里要反一下 直接json搜索太慢了吧
        //                 userDisplay->drawRectangle(obj[i], i, it, &blue);
        //             else
        //                 userDisplay->drawRectangle(obj[i], i, it, &red);
        //         else
        //             lv_obj_set_hidden(obj[i], true);
        //     }
        //     i++;
        // }
        // visionData->showShootData(str);
    }
    lv_label_set_text(userDisplay->otherLab, str.c_str());
}
/**
 * 标签栏按下后的动作
 * @param tab 标签
 * @param x   几号标签
 */
static lv_res_t tabChose(lv_obj_t *tab, uint16_t x)
{

    if (x == 0)
    {
        std::string str;
        visionData->showSignature(str);
        lv_obj_t *lab = static_cast<lv_obj_t *>(lv_obj_get_free_ptr(lv_tabview_get_tab(tab, 0)));
        lv_label_set_text(lab, str.c_str());
        lv_obj_set_hidden(userDisplay->otherLab, true);
    }
    else
    {
        lv_obj_set_hidden(userDisplay->otherLab, false);
        userDisplay->_tempData["视觉信息"]["数据"] = x - 1;
        visionData->setSig(userDisplay->_tempData, x - 1);
    }
    return LV_RES_OK;
}
lv_res_t dataSwAction(lv_obj_t *sw) //SW的动作
{
    displayData = lv_sw_get_state(sw);
    for (auto &it : obj)
        lv_obj_set_hidden(it, true);
    return LV_RES_OK;
}
namespace ncrapi {
void UserDisplay::createVision(lv_obj_t *parent)
{

    if (displayObj[OBJ_BTNM_SON] == nullptr)
        displayObj[OBJ_BTNM_SON] = lv_tabview_create(parent, nullptr);
    displayObj[OBJ_BTNM_SON]->style_p->body.main_color = LV_COLOR_BLACK;
    side = sysData->jsonVal["自动赛"]["红方&蓝方"];
    //创建设置页面
    std::vector<lv_obj_t *> tabs; //标签页
    tabs.push_back(lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], "设置"));
    lv_page_set_scrl_fit(tabs.back(), false, false);
    lv_page_set_scrl_height(tabs.back(), 600);    //设置页面可滚动部分的高度
    static const char *updown[] = {"+", "-", ""}; //创建按钮矩阵
    _tempData = sysData->jsonVal;
    size_t posX = 0, posY = 5;
    lv_obj_t *key = nullptr;
    for (auto &it : _tempData["视觉信息"].items())
    {
        if (it.key() != "曝光度" && it.key() != "数据" && it.key() != "颜色范围")
        {
            /**key设置*/
            key = lv_label_create(tabs.back(), nullptr); //新建子项目的key
            lv_label_set_text(key, it.key().c_str());    //设置子项目的key的文本信息
            lv_obj_set_pos(key, posX, posY);             //设置key的位置
            /**value设置*/
            lv_obj_t *values = lv_label_create(tabs.back(), nullptr); //新建子项目的val
            lv_label_set_text(values, it.value().dump().c_str());     //设置按钮的值
            lv_obj_align(values, key, LV_ALIGN_OUT_RIGHT_MID, 10, 0); //values的位置紧贴key的位置

            /**updownBtn设置*/
            lv_obj_t *updownBtn = lv_btnm_create(tabs.back(), nullptr); //新建updownBtn
            lv_btnm_set_map(updownBtn, updown);
            lv_obj_set_size(updownBtn, 50, 25);

            lv_obj_set_free_ptr(updownBtn, values);   //创建关键指针. btn关联values 这里主要为了实现刷新
            lv_obj_set_free_ptr(values, &it.value()); //创建关键指针. values关联key

            lv_obj_align(updownBtn, values, LV_ALIGN_OUT_RIGHT_MID, 30, 0); //设置updownbtn位置
            lv_btnm_set_action(updownBtn, upDownAction);                    //设置updown按钮动作
            if (lv_obj_get_x(updownBtn) > 230)                              //key表示偶数换行 it.size()表示每行总共有2个数一个KEY 一个VAL
            {
                posX = 0;
                posY += 30;
            }
            else
                posX = 230;
        }
    }
    lv_obj_t *sigLab = lv_label_create(tabs.back(), nullptr); //创建基于页面1的label 用来存放sig
    lv_obj_set_pos(sigLab, 0, lv_obj_get_y(key) + 40);
    std::string sigStr;
    if (visionData != nullptr)
    {
        visionData->showSignature(sigStr);
        lv_label_set_text(sigLab, sigStr.c_str());
        lv_obj_set_free_ptr(tabs.back(), sigLab); //把sigLab关联到标签页第一页
        //创建其他页面
        int count = 0;
        for (auto &it : visionData->COLOR_DATA)
        {
            std::string temp = "按数据" + std::to_string(count);
            tabs.push_back(lv_tabview_add_tab(displayObj[OBJ_BTNM_SON], temp.c_str())); //添加项目
            lv_page_set_scrl_fit(tabs.back(), false, false);
            lv_page_set_scrl_height(tabs.back(), 200); //设置页面可滚动部分的高度
            //绘制滑块
            int pos = 20;
            for (auto &rangeIt : _tempData["视觉信息"]["颜色范围"].items())
            {
                /**key设置*/
                key = lv_label_create(tabs.back(), nullptr);                 //新建子项目的key
                lv_label_set_text(key, rangeIt.key().c_str());               //设置子项目的key的文本信息
                lv_obj_align(key, tabs.back(), LV_ALIGN_IN_TOP_MID, 0, pos); //设置KEY位置]

                /**value设置*/
                lv_obj_t *values = lv_label_create(tabs.back(), nullptr); //新建子项目的val
                std::stringstream oss;                                    //主要为了去掉小数点后多余的0 默认3位
                oss.setf(std::ios::fixed);                                //用定点格式显示浮点数,不会用科学计数法表示
                oss.precision(1);                                         //由于用了定点格式，设置变为了保留4位小数
                oss << rangeIt.value()[count].get<float>();
                lv_label_set_text(values, oss.str().c_str());
                lv_obj_align(values, key, LV_ALIGN_OUT_RIGHT_MID, 10, 0); //values的位置紧贴key的位置

                /**updownBtn设置*/
                lv_obj_t *updownBtn = lv_btnm_create(tabs.back(), nullptr); //新建updownBtn
                lv_btnm_set_map(updownBtn, updown);
                lv_obj_set_size(updownBtn, 50, 25);

                lv_obj_set_free_ptr(updownBtn, values);                                                //创建关键指针. btn关联values 这里主要为了实现刷新
                lv_obj_set_free_ptr(values, &_tempData["视觉信息"]["颜色范围"][rangeIt.key()][count]); //创建关键指针. values关联key

                lv_obj_align(updownBtn, values, LV_ALIGN_OUT_RIGHT_MID, 30, 0); //设置updownbtn位置
                lv_btnm_set_action(updownBtn, rangeAction);                     //设置updown按钮动作

                pos += 40;
            }
            count++;
        }
        lv_tabview_set_tab_act(displayObj[OBJ_BTNM_SON], sysData->jsonVal["视觉信息"]["数据"].get<uint16_t>() + 1, false); //设置进来的默认标签页
        /*当选项卡按下后进行的操作*/
        lv_tabview_set_tab_load_action(displayObj[OBJ_BTNM_SON], tabChose);
        otherLab = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr); //创建基于INFOObj的标签
        lv_obj_set_y(otherLab, 30);
        lv_obj_set_hidden(otherLab, true);

        //设定图形样式
        for (auto &it : obj)
        {
            //绘制图形
            it = lv_obj_create(displayObj[OBJ_BTNM_SON], nullptr);
            lv_obj_t *objLab = lv_label_create(it, nullptr);
            it->style_p = &lv_style_plain_color;
            it->style_p->body.empty = 1;
            it->style_p->body.border.width = 1;
            it->style_p->body.border.part = LV_BORDER_FULL;
            lv_obj_set_hidden(it, true);
            lv_obj_set_free_ptr(it, objLab);
        }

        red = *lv_obj_get_style(obj[0]);
        blue = *lv_obj_get_style(obj[0]);
        green = *lv_obj_get_style(obj[0]);
        orange = *lv_obj_get_style(obj[0]);
        violet = *lv_obj_get_style(obj[0]);
        red.body.border.color = LV_COLOR_RED;
        blue.body.border.color = LV_COLOR_BLUE;
        orange.body.border.color = LV_COLOR_ORANGE;
        violet.body.border.color = LV_COLOR_PURPLE;
        //绘制BASEDATE和SHOOTDATE之间的显示切换那妞
        lv_obj_t *dataSw = lv_sw_create(displayObj[OBJ_BTNM_SON], nullptr);       //第一个开关
        lv_obj_t *dataSwLab = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr); //第二个文本条
        lv_obj_set_size(dataSw, 50, 20);
        lv_sw_off(dataSw);
        lv_obj_align(dataSw, displayObj[OBJ_BTNM_SON], LV_ALIGN_CENTER, 10, 50);
        lv_obj_align(dataSwLab, dataSw, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        lv_sw_set_action(dataSw, dataSwAction); //设置按钮的动作
        lv_label_set_text(dataSwLab, "原始数据&弹射数据");

        //绘制滑块
        lv_obj_t *expSlider = lv_slider_create(displayObj[OBJ_BTNM_SON], nullptr);
        lv_obj_set_size(expSlider, 160, 30);
        lv_obj_set_pos(expSlider, LV_HOR_RES - 320, LV_VER_RES - 60);
        lv_slider_set_action(expSlider, expAction);
        lv_bar_set_value(expSlider, visionData->getExposure());
        lv_slider_set_range(expSlider, 0, 150); //设置滑块范围
        //绘制滑块边上的文本
        lv_obj_t *sliderKey = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr);
        lv_obj_align(sliderKey, expSlider, LV_ALIGN_OUT_LEFT_MID, -50, 0);
        lv_label_set_text(sliderKey, "曝光度");

        lv_obj_t *sliderVal = lv_label_create(displayObj[OBJ_BTNM_SON], nullptr);
        lv_obj_align(sliderVal, sliderKey, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        lv_label_set_text(sliderVal, std::to_string(visionData->getExposure()).c_str());
        lv_obj_set_free_ptr(expSlider, sliderVal);                        //把滑块的文本关联给滑块
        lv_obj_set_free_ptr(sliderVal, &_tempData["视觉信息"]["曝光度"]); //把tempData 传递给 滑块文本

        //创建保存按钮
        createSaveBtn(OBJ_BTNM_SON);
        createUserTask(TASK_OTHER, visionTask, 100, "视觉");
    }
    //退出按钮
    createExitBtn(OBJ_BTNM_SON);
} // namespace ncrapi
} // namespace ncrapi