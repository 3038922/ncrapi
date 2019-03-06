
#include "ncrapi/device/adi.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/util/util.hpp"

namespace ncrapi
{

ADIAnalogIn::ADIAnalogIn(const std::string &name, const int &port) : _name(name),
                                                                     _port(port),
                                                                     pros::ADIAnalogIn(port)
{
    logger->info({_name, ":", std::to_string(_port), "构造成功"});
}
ADIAnalogIn::ADIAnalogIn(const std::string &name, const json &pragma) : _name(name),
                                                                        _port(pragma[name].get<int>()),
                                                                        pros::ADIAnalogIn(pragma[name].get<int>())
{
    logger->info({_name, ":", std::to_string(_port), "构造成功"});
}

std::string ADIAnalogIn::getName()
{
    return _name;
}
int ADIAnalogIn::getPort()
{
    return _port;
}

ADIDigitalIn::ADIDigitalIn(const std::string &name, const int &port) : _name(name),
                                                                       _port(port),
                                                                       pros::ADIDigitalIn(port)
{
    logger->info({_name, ":", std::to_string(_port), "构造成功"});
}

ADIDigitalIn::ADIDigitalIn(const std::string &name, const json &pragma) : _name(name),
                                                                          _port(pragma[name].get<int>()),
                                                                          pros::ADIDigitalIn(pragma[name].get<int>())
{
    logger->info({_name, ":", std::to_string(_port), "构造成功"});
}
std::string ADIDigitalIn::getName()
{
    return _name;
}
int ADIDigitalIn::getPort()
{
    return _port;
}

ADIEncoder::ADIEncoder(const std::string &name, const std::pair<int, int> &port) : _name(name),
                                                                                   _port(port),
                                                                                   pros::ADIEncoder(port.first, port.second)
{
    logger->info({_name, ":", std::to_string(_port.first), " ", std::to_string(_port.second), "构造成功"});
}

ADIEncoder::ADIEncoder(const std::string &name, const json &pragma) : _name(name),
                                                                      _port(std::make_pair<int, int>(pragma[name].at(0).get<int>(), pragma[name].at(1).get<int>())),
                                                                      pros::ADIEncoder(pragma[name].at(0).get<int>(), pragma[name].at(1).get<int>())
{
    logger->info({_name, ":", std::to_string(_port.first), " ", std::to_string(_port.second), "构造成功"});
}
std::string ADIEncoder::getName()
{
    return _name;
}
std::pair<int, int> ADIEncoder::getPort()
{
    return _port;
}

} // namespace ncrapi