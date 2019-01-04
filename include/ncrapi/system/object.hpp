#pragma once
#include <string>
namespace ncrapi
{
class Obj
{
  public:
    virtual void showSensor() = 0;
    virtual const std::string showName() const = 0;
    virtual void showDetailedInfo() = 0;
    virtual void resetAllSensors() = 0;
};
} // namespace ncrapi