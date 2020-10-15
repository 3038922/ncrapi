#pragma once
#include "ncrapi/units/QLength.hpp"
#include "sysBase.hpp"

namespace ncrapi
{
class SysUser : public SysBase
{
    using PidDebugTuple = std::tuple<double, double, double, double, double>; //PIDDATA的类型

  public:
    SysUser(const json &paragam);
    //综合参数
    QLength robotLength; //机器人长
    QLength robotWidth;  //机器人宽
    QLength siteSize;    //场地边长 编码器的测量值*常数+机器人长度
    QLength matSize;     //一个垫子的长度 = 场地长度 / 6
    double wheelSpacing; //轮间距 系数
    double diameter;     //车轮直径 系数
    QLength odomMin;
    QLength odomMax;
    bool isInit = false;
    //PID参数数据
    std::vector<PidDebugTuple> pidDebugData;       //PID DEBUG数据
    std::vector<std::vector<double>> _recoderData; //记录机器人数据
    void testAction(void (*autonomous)());
    //逻辑相关
    bool isShooted = false; //是否应发射 这里可能有线程同步的问题

    //PID相关
    size_t test = 0;                                              //0关闭 1 前后 2 左右
    double pidTestTarget = 0;                                     //PID TEST的目标值
    std::pair<QTime, double> pidData[2] = {{0_ms, 0}, {0_ms, 0}}; //接受PID测试时候的返回值
    //录像参数
    int recoderMode = 0; //0 不录像 1录像 2 回放
    void recoder(std::vector<double> &data);
    void showRecoder();

  private:
};

} // namespace ncrapi
extern std::unique_ptr<ncrapi::SysUser> sysData;