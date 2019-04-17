#include "ncrapi/system/logger.hpp"
#include "ncrapi/userDisplay/userDisplay.hpp"
#include "pros/misc.hpp"
#include "pros/rtos.hpp"
#include <cstdlib>
#include <iostream>
namespace ncrapi {

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
Logger::Logger()
{
    if (pros::competition::get_status() == COMPETITION_CONNECTED)
    {
        isComp = true;
        std::cout << "日志类构造成功,场控已连接" << std::endl;
    }
    else
        std::cout << "日志类构造成功,场控未连接" << std::endl;
}
void Logger::error(std::initializer_list<std::string> val)
{
    output(val);
    _errorCount++;
    if (userDisplay != nullptr)
    {
        terminalStr[ERROR] += "#FF0000 " + _str + "#\n";
        auto temp2 = "#FF0000 " + std::to_string(_errorCount) + "#";
        lv_label_set_text(userDisplay->errorLabs, temp2.c_str());
    }
    if (!isComp)
        std::cout << RED << _str << std::endl;
}
void Logger::warnning(std::initializer_list<std::string> val)
{
    output(val);
    _warnningCount++;
    if (userDisplay != nullptr)
    {
        terminalStr[WARNNING] += "#CCFF00 " + _str + "#\n";
        auto temp2 = "#CCFF00 " + std::to_string(_warnningCount) + "#";
        lv_label_set_text(userDisplay->warnningLabs, temp2.c_str());
    }
    if (!isComp)
        std::cout << YELLOW << _str << std::endl;
}
void Logger::debug(std::initializer_list<std::string> val)
{
    if (!isComp)
    {
        output(val);
        std::cout << BLUE << _str << std::endl;
    }
}
void Logger::info(std::initializer_list<std::string> val)
{
    if (!isComp)
    {
        output(val);
        std::cout << RESET << _str << std::endl;
    }
}
void Logger::clearCount()
{
    _errorCount = 0;
    _warnningCount = 0;
    lv_label_set_text(userDisplay->errorLabs, "#FF0000 0#");
    lv_label_set_text(userDisplay->warnningLabs, "#CCFF00 0#");
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