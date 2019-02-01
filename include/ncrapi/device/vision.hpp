#pragma once
#include "ncrapi/system/json.hpp"
#include "pros/vision.hpp"

namespace ncrapi {
class Vision : public pros::Vision
{
  public:
    explicit Vision(const std::string &name, const int &port);
    explicit Vision(const std::string &name, const json &pragma);
    std::string getName();
    int getPort();

  private:
    const std::string _name;
    const int _port;
};
} // namespace ncrapi