#include "ncrapi/device/vision.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/system/sysUser.hpp"

namespace ncrapi {

Vision::Vision(const std::string &name, const int &port) : _name(name),
                                                           pros::Vision(port)
{
    sysData->i2cCheck(port, _name);
    logger->info({_name, ":", std::to_string(port), "构造成功"});
}
Vision::Vision(const std::string &name, const json &pragma) : _name(name), pros::Vision(pragma[name].get<int>())
{
    sysData->i2cCheck(pragma[name].get<int>(), _name);
    logger->info({_name, ":", pragma[name].get<std::string>(), "构造成功"});
}
std::string Vision::getName()
{
    return _name;
}
} // namespace ncrapi