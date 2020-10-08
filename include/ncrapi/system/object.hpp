#pragma once
#include "ncrapi/device/motor.hpp"
#include <string>

// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-parameter"
// #include <vector>
// #pragma GCC diagnostic pop
#define PROS_USE_SIMPLE_NAMES

#define PROS_USE_LITERALS
namespace ncrapi {
class Obj
{
  public:
    virtual void showSensor() = 0;
    virtual const std::string showName() const = 0;
    virtual void showDetailedInfo() = 0;
    virtual void resetAllSensors() = 0;
    virtual void stop() = 0;
    virtual void setMode(int flag) = 0;
    virtual bool isSafeMode() = 0;
    virtual const std::vector<Motor> getMotorInfo() const = 0;
};
} // namespace ncrapi