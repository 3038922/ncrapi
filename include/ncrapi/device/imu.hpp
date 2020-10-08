#include "ncrapi/system/json.hpp"
#include "pros/imu.hpp"
namespace ncrapi {
class Imu : public pros::Imu
{
  public:
    Imu(const std::string &name, const int &port);
    Imu(const std::string &name, const json &pragma);
    std::string getName();

  private:
    const std::string _name;
};
} // namespace ncrapi