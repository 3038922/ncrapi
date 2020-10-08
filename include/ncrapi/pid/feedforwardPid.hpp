#pragma once
#include "pid.hpp"
namespace ncrapi {
enum FFstates {
    accel,  //加速
    cruise, //定速巡航
    deccel, //减速刹车
    rest    //重置
};
class FeedForwardPid : public Pid
{
  public:
    FeedForwardPid(const std::string &name, const std::vector<double> &pragma);
    FeedForwardPid(const std::string &name, const json &pragma);
    /**
     * 前馈控制 位置式
     * @param  inewReading 传感器输入值
     * @return            电机PWM值
     */
    virtual double step(const double inewReading) override;
    /**
     * @brief 重置控制器
     * 
     */
    virtual void reset() override;

  private:
    /**
   * @brief   前馈控制
   * 
   * @param max_a 最大加速度 mm/s
   * @param max_v 最大速度 mm/s2
   * @param target 目标距离
   * @return double  mp_pos
   */
    virtual void statemMachine(const double max_a, const double max_v);

    double _ffVel = 0, _ffPos = 0, _ffAccelDist = 0;
    FFstates _ffState, _lastffState;
};
} // namespace ncrapi