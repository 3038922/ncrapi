#pragma once
#include "ncrapi/advanced/filter/kalman.hpp"
#include "pid.hpp"

namespace ncrapi {

class FeedForwardPid : public Pid<double, double>
{
    enum FFstates {
        accel,  //加速
        cruise, //定速巡航
        deccel, //减速刹车
        rest    //重置
    };
    enum pidPragma { KP = 0,
                     KI,
                     KD,
                     BIAS,
                     LIMIT,
                     Q,
                     R };

  public:
    FeedForwardPid(const std::string &name, const json &pragma);
    FeedForwardPid(const std::string &name, const DataType &pragma);
    virtual double step(const double inewReading, const bool showDebug = false) override;
    virtual void setFilterPragma(const double q, const double r);
    virtual void showPidParams() override;
    virtual void reset() override;

  protected:
    virtual void calcNowKdSum() override;
    virtual void calcNowKiSum() override;
    virtual void calcNowOutput() override;
    virtual void showDebugData() override;

    std::unique_ptr<KalmanFilter<double>> _filter;

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