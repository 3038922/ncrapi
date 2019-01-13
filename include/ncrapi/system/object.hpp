#pragma once
#include "ncrapi\util\timer.hpp"
#include "ncrapi\util\util.hpp"
#include "pros/adi.hpp"
#include "pros/misc.hpp"
#include "pros/motors.hpp"
#include "pros/rtos.hpp"
#include <vector>

namespace ncrapi
{
class Obj
{
  public:
    virtual void showSensor() = 0;
    virtual const std::string showName() const = 0;
    virtual void showDetailedInfo() = 0;
    virtual void resetAllSensors() = 0;
    virtual void stop() = 0;
};
} // namespace ncrapi