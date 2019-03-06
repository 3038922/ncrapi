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
    void error(std::initializer_list<std::string> val);
    void warnning(std::initializer_list<std::string> val);
    void debug(std::initializer_list<std::string> val);
    void info(std::initializer_list<std::string> val);
    bool showData(LEVEL level, std::string &str);
    bool close(LEVEL level);
    bool clean(LEVEL level);
    size_t getLevel();

  private:
    void output(std::initializer_list<std::string> &val);
    std::string _str;
    std::vector<FILE *> _data;
    std::vector<std::string> _level = {"/usd/error.log", "/usd/warnning.log", "/usd/debug.log", "/usd/info.log"};
};
} // namespace ncrapi
extern std::unique_ptr<ncrapi::Logger> logger;