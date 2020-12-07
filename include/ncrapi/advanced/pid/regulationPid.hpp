#pragma once
#include "velPid.hpp"
namespace ncrapi {
/**
 * @brief 梯形PID 增量式
 * 
 */
class RegulationPid : public VelPid
{

  public:
    RegulationPid(const std::string &name, const json &pragma);
    RegulationPid(const std::string &name, const DataType &pragma);

  protected:
    virtual void calcNowKiSum() override;

  private:
};
} // namespace ncrapi
