#include "ncrapi/device/vision.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/system/sysBase.hpp"
#include "ncrapi/util/util.hpp"

namespace ncrapi
{

Vision::Vision(const std::string &name, const int &port) : _port(port), _name(name),
                                                           pros::Vision(port)
{
    sysData->i2cCheck(_port, _name);
    logger->info({_name, ":", std::to_string(_port), "构造成功"});
}
Vision::Vision(const std::string &name, const json &pragma) : _name(name), _port(pragma[name].get<int>()),
                                                              pros::Vision(pragma[name].get<int>())
{
    sysData->i2cCheck(_port, _name);
    logger->info({_name, ":", std::to_string(_port), "构造成功"});
}
std::string Vision::getName()
{
    return _name;
}
int Vision::getPort()
{
    return _port;
}

} // namespace ncrapi