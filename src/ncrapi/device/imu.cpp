#include "ncrapi/device/imu.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/system/sysUser.hpp"
namespace ncrapi {
Imu::Imu(const std::string &name, const int &port) : _name(name),
                                                     pros::Imu(port)
{
    sysData->adiCheck(port, _name);
    logger->info({_name, ":", std::to_string(port), "构造成功"});
}
Imu::Imu(const std::string &name, const json &pragma) : _name(name),
                                                        pros::Imu(pragma[name].get<int>())
{
    sysData->adiCheck(pragma[name].get<int>(), _name);
    logger->info({_name, ":", pragma[name].get<std::string>(), "构造成功"});
}
std::string Imu::getName()
{
    return _name;
}
} // namespace ncrapi