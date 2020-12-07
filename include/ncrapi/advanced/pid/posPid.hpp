#pragma once
#include "ncrapi/advanced/filter/kalman.hpp"
#include "pid.hpp"

namespace ncrapi {
class PosPid : public Pid<double, double>
{
  protected:
    enum pidPragma { KP = 0,
                     KI,
                     KD,
                     BIAS,
                     LIMIT,
                     Q,
                     R };

  public:
    PosPid(const std::string &name, const json &pragma);
    PosPid(const std::string &name, const DataType &pragma);
    virtual void setFilterPragma(const double q, const double r);
    virtual void showPidParams() override;
    virtual void reset() override;

  protected:
    virtual void calcNowKdSum() override;

    virtual void calcNowKiSum() override;
    std::unique_ptr<KalmanFilter<double>> _filter;

  private:
};
} // namespace ncrapi