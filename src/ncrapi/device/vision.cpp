#include "ncrapi/device/vision.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/util/util.hpp"

namespace ncrapi
{

Vision::Vision(const std::string &name, const int &port) : _port(port), _name(name),
                                                           pros::Vision(port)
{
    logger->info({_name, ":", std::to_string(_port), "构造成功"});
}
Vision::Vision(const std::string &name, const json &pragma) : _name(name), _port(pragma[name].get<int>()),
                                                              pros::Vision(pragma[name].get<int>())
{
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