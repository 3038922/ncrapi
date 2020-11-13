#include "ncrapi/ncrapi.hpp"
void userauto(const json &jsonVal, ncrapi::Timer &autoTimer)
{
    //今天这比赛四个地方自动其实是一样的 那写一套自动就可以通吃所有了
    //判断红蓝方
    int sideFlag = 1;
    if (jsonVal[I18N_RED_ALLIANCE "&" I18N_BLUE_ALLIANCE]) //蓝方 没有加！取反噢
        sideFlag = -1;
    //判断前后场
    if (!jsonVal["前场&后场"]) //前场
        ;
    else //后场
        sideFlag *= -1;
    if (!jsonVal["放X中&不放"]) //如果放中间
    {
    }
    if (!jsonVal["放Y中&不放"]) //如果放家里中间
    {
    }
    if (!jsonVal["放中间&不放"]) //如果放Y轴中间
    {
    }
}
void skillAuto(const json &jsonVal, ncrapi::Timer &autoTimer)
{
}