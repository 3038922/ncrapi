/**
 * @Author: 陈昱安
 * @Date:   2018-10-22T22:01:37+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-10-28T22:09:06+08:00
 */

#include "ncrapi/userDisplay/userDisplay.hpp"

/**
 * 按钮阵列的动作
 * @param  btnm 按钮阵列
 * @param  txt  按钮的名字
 * @return      系统值
 */
static lv_res_t btnm_action(lv_obj_t *btnm, const char *txt)
{
    (void)btnm; /*Unused*/
    //TODO 做一个字符串判断
    if (!strcmp(txt, "系统信息"))
        userDisplay->createSysInfo(userDisplay->displayObj[BTNM_START]);
    if (!strcmp(txt, "全局参数设置"))
        userDisplay->createConfig(userDisplay->displayObj[BTNM_START]); //创建CONFIGSET页面
    if (!strcmp(txt, "自定义测试"))
        userDisplay->createCustomTest(userDisplay->displayObj[BTNM_START]);
    if (!strcmp(txt, "视觉传感器设置"))
        userDisplay->createVision(userDisplay->displayObj[BTNM_START]); //创建视觉页面
    if (!strcmp(txt, "版本号"))
        userDisplay->createVersion(userDisplay->displayObj[BTNM_START]);
    if (!strcmp(txt, "机器人检测"))
        userDisplay->createDebug(userDisplay->displayObj[BTNM_START]);
    if (!strcmp(txt, "PID调试"))
        userDisplay->createPidTest(userDisplay->displayObj[BTNM_START]);
    if (!strcmp(txt, "ODOM测试"))
        userDisplay->createOdom(userDisplay->displayObj[BTNM_START]);

    printf("Key pressed: %s\n", txt);
    return LV_RES_INV;
}
namespace ncrapi
{
void UserDisplay::createStartObj()
{
    static const char *startBtnm[] = {"系统信息", "机器人检测", "\n",
                                      "全局参数设置", "自定义测试", "\n",
                                      "PID调试", "视觉传感器设置", "\n",
                                      "ODOM测试", "版本号", ""};
    if (displayObj[BTNM_START] == nullptr)
        displayObj[BTNM_START] = lv_btnm_create(displayObj[OBJ_OPCONTROL], nullptr); //创建按钮集群

    lv_btnm_set_map(displayObj[BTNM_START], startBtnm);
    lv_obj_set_size(displayObj[BTNM_START], LV_HOR_RES, LV_VER_RES - 20);
    lv_obj_set_y(displayObj[BTNM_START], 20);
    lv_btnm_set_action(displayObj[BTNM_START], btnm_action);
}
} // namespace ncrapi