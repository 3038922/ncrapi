#pragma once
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
namespace ncrapi
{

class Logger
{
  public:
    typedef enum LEVEL
    {
        ERROR = 0,
        WARNNING,
        DEBUG,
        INFO

    } LEVEL;
    Logger(const size_t Level = 0);
    /**
     * @brief 打印红色错误信息到控制台和机器人内置控制台
     * 
     * @param val 具体格式是logger->error({"要打印的东西",std::to_string(变量名)});
     */
    void error(std::initializer_list<std::string> val);
    /**
     * @brief 打印黄色警告信息到控制台和机器人内置控制台
     * 
     * @param val 具体格式是logger->error({"要打印的东西",std::to_string(变量名)});
     */
    void warnning(std::initializer_list<std::string> val);
    /**
     * @brief 打印蓝色调试信息到控制台和机器人内置控制台
     * 
     * @param val 具体格式是logger->error({"要打印的东西",std::to_string(变量名)});
     */
    void debug(std::initializer_list<std::string> val);
    /**
     * @brief 打印白色普通信息到控制台和机器人内置控制台
     * 
     * @param val 具体格式是logger->error({"要打印的东西",std::to_string(变量名)});
     */
    void info(std::initializer_list<std::string> val);
    bool showData(LEVEL level, std::string &str);
    bool close(LEVEL level);
    bool clean(LEVEL level);
    size_t getLevel();

  private:
    size_t _lever = 0;
    void output(std::initializer_list<std::string> &val);
    std::string _str;
    std::array<FILE *, 4> _data = {nullptr, nullptr, nullptr, nullptr};
    std::vector<std::string> _level = {"/usd/error.log", "/usd/warnning.log", "/usd/debug.log", "/usd/info.log"};
};
} // namespace ncrapi
extern std::unique_ptr<ncrapi::Logger> logger;