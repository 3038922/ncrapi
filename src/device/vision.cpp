#include "ncrapi/device/vision.hpp"
#include "ncrapi/util/util.hpp"
#include <iostream>

namespace ncrapi {

Vision::Vision(const std::string &name, const int &port) : _port(port), _name(name),
                                                           pros::Vision(port)
{
    std::cout << _name << ":" << _port << " 构造成功" << std::endl;
}
Vision::Vision(const std::string &name, const json &pragma) : _name(name), _port(pragma[name].get<int>()),
                                                              pros::Vision(pragma[name].get<int>())
{
    std::cout << _name << ":" << _port << " 构造成功" << std::endl;
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