#pragma once
#include "ncrapi/advanced/filter/kalman.hpp"
#include "pid.hpp"
namespace ncrapi {
/**
 * @brief 速度环 增量式
 * 
 */
class VelPid : public Pid<double, double>
{
    enum pidPragma { KP = 0,
                     KI,
                     KD,
                     BIAS,
                     LIMIT,
                     Q,
                     R };

  public:
    VelPid(const std::string &name, const json &pragma);
    VelPid(const std::string &name, const DataType &pragma);
    double step(const double inewReading, const bool isShowDebug = false) override;
    virtual void setFilterPragma(const double q, const double r);
    virtual void showPidParams() override;
    virtual void reset() override;

  protected:
    virtual void calcNowKdSum() override;
    virtual void calcNowKpSum() override;
    virtual void calcNowKiSum() override;
    virtual void calcNowOutput() override;
    void showDebugData() override;
    std::unique_ptr<KalmanFilter<double>> _filter;
    double _incrementOuput = 0.0;

  private:
    //QAngularSpeed _vel = 0_rpm, _lastVel = 0_rpm; //当前速度 上次速度
};
} // namespace ncrapi
