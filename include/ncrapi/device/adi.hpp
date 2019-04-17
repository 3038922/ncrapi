#pragma once
#include "ncrapi/system/json.hpp"
#include "pros/adi.hpp"

namespace ncrapi {
class ADIAnalogIn : public pros::ADIAnalogIn
{
  public:
    ADIAnalogIn(const std::string &name, const int &port);
    ADIAnalogIn(const std::string &name, const json &pragma);

    std::string getName();
    int getPort();

  private:
    const int _port;
    const std::string _name;
};
using ADIPoten = ADIAnalogIn;
using ADILine = ADIAnalogIn;
class ADIDigitalIn : public pros::ADIDigitalIn
{
  public:
    ADIDigitalIn(const std::string &name, const int &port);

    ADIDigitalIn(const std::string &name, const json &pragma);
    std::string getName();
    int getPort();

  private:
    const int _port;
    const std::string _name;
};
using ADIBumper = ADIDigitalIn;
class ADIEncoder : public pros::ADIEncoder
{
  public:
    ADIEncoder(const std::string &name, const std::pair<int, int> &port);
    ADIEncoder(const std::string &name, const json &pragma);
    std::string getName();
    std::pair<int, int> getPort();

  private:
    const std::pair<int, int> _port;
    const std::string _name;
};
} // namespace ncrapi