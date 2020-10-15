
#pragma once
#include "ncrapi/units/QAngularSpeed.hpp"
#include "pid.hpp"

namespace ncrapi {

class VelPid : public Pid
{
  public:
    /**
     * Velocity PID controller
     * @param params Params (see VelPidParams docs)
     */
    VelPid(const std::string &name, const json &pragma);

    /**
*执行一次控制器迭代
*@param inewReading新测量
*@return控制器输出
     */
    virtual double step(const double inewReading) override;

  private:
    //QAngularSpeed _vel = 0_rpm, _lastVel = 0_rpm; //当前速度 上次速度
};
} // namespace ncrapi
