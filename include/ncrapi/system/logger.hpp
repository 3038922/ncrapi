#pragma once
#include "display/lvgl.h"
#include "ncrapi/util/timer.hpp"

namespace ncrapi {

class Logger
{
  public:
    typedef enum LEVEL {
        ERROR,
        WARNING,
        DEBUG,
        INFO

    } LEVEL;
    //饿汉模式单例实现.线程安全 因为日志永不关闭 所以就不写释放了
    static Logger *initLogger();
    static Logger *getLogger();
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
    void warning(std::initializer_list<std::string> val);
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
    /**
     * @brief 清理error和warning的计数器
     * 
     */
    void clearCount();
    /**
     * @brief DEBUG模块的线程
     * 
     */
    static void loop(void *pragma);
    std::string terminalStr[2];
    lv_obj_t *errorLabs = nullptr, *warningLabs = nullptr; //控制台上的显示错误数量的lab

  private:
    Logger();
    ~Logger() {}
    static Logger *_logger; // 单例对象在这里！;
    bool isComp = false;
    void output(std::initializer_list<std::string> &val);
    std::string _str;
    size_t _errorCount = 0, _warningCount = 0;
};
#define logger Logger::getLogger()
} // namespace ncrapi
