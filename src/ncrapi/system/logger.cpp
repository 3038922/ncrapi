#include "ncrapi/system/logger.hpp"
#include "pros/rtos.hpp"
#include <cstdlib>
#include <iostream>

namespace ncrapi
{
#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */
Logger::Logger(const size_t Level)
{
    for (size_t i = 0; i <= Level; i++)
    {
        _data.push_back(fopen(_level[i].c_str(), "a"));
        if (_data.back() == nullptr)
            std::cerr << _level[i] << " 创建失败,请检查SD卡!" << std::endl;
        else
            std::cout << _level[i] << " 创建成功" << std::endl;
    }
}
void Logger::error(std::initializer_list<std::string> val)
{
    output(val);
    if (_data[ERROR] != nullptr)
        fprintf(_data[ERROR], "#FF0000 %s#\n", _str.c_str());
    std::cout << RED << _str << std::endl;
}
void Logger::warnning(std::initializer_list<std::string> val)
{
    output(val);
    if (_data[WARNNING] != nullptr)
        fprintf(_data[WARNNING], "#CCFF00 %s#\n", _str.c_str());
    std::cout << YELLOW << _str << std::endl;
}
void Logger::debug(std::initializer_list<std::string> val)
{
    output(val);
    if (_data[DEBUG] != nullptr)
        fprintf(_data[DEBUG], "#3366FF %s#\n", _str.c_str());
    std::cout << BLUE << _str << std::endl;
}
void Logger::info(std::initializer_list<std::string> val)
{
    output(val);
    if (_data[INFO] != nullptr)
        fprintf(_data[INFO], "#FFFFFF %s#\n", _str.c_str());
    std::cout << RESET << _str << std::endl;
}
bool Logger::showData(LEVEL level, std::string &str)
{
    if (_data[level] != nullptr)
    {
        fclose(_data[level]);
        _data[level] = fopen(_level[level].c_str(), "r"); //先关闭写模式再以读模式打开
        char buf[1024];
        while (fgets(buf, 1024, _data[level]) != nullptr)
            str += buf;
        fclose(_data[level]);                             //先关闭读模式
        _data[level] = fopen(_level[level].c_str(), "a"); //再继续以写模式打开
        return true;
    }
    std::cerr << _level[level] << " 文件打开错误,请检查SD卡!" << std::endl;
    return false;
}
bool Logger::close(LEVEL level)
{
    if (_data[level] != nullptr)
    {
        fclose(_data[level]);
        _data[level] = nullptr;
        return true;
    }
    std::cerr << _level[level] << " 文件打开错误,请检查SD卡!" << std::endl;
    return false;
}
bool Logger::clean(LEVEL level)
{
    if (_data[level] != nullptr)
    {
        fclose(_data[level]);                             //先关闭
        _data[level] = fopen(_level[level].c_str(), "w"); //清空 先关闭写模式再以写模式打开
        fclose(_data[level]);                             //再关闭
        _data[level] = fopen(_level[level].c_str(), "a"); //再继续以写模式打开
        return true;
    }
    std::cerr << _level[level] << " 文件打开错误,请检查SD卡!" << std::endl;
    return false;
}
size_t Logger::getLevel()
{
    return (_data.size() - 1);
}
void Logger::output(std::initializer_list<std::string> &val)
{
    _str.clear();
    int nowTime = pros::millis();
    _str = "[" + std::to_string((nowTime % 3600000) / 60000) + ":" + std::to_string((nowTime % 60000) / 1000) + ":" + std::to_string(nowTime % 1000) + "]:";
    for (auto &it : val)
        _str += it;
}
} // namespace ncrapi