#pragma once
#include "display/lvgl.h"
#include "ncrapi/util/timer.hpp"
#include <iostream>
#include <sstream>

namespace ncrapi {

class Logger
{
    const std::string RESET = "\033[0m";
    const std::string BLACK = "\033[30m";              /* Black */
    const std::string RED = "\033[31m";                /* Red */
    const std::string GREEN = "\033[32m";              /* Green */
    const std::string YELLOW = "\033[33m";             /* Yellow */
    const std::string BLUE = "\033[34m";               /* Blue */
    const std::string MAGENTA = "\033[35m";            /* Magenta */
    const std::string CYAN = "\033[36m";               /* Cyan */
    const std::string WHITE = "\033[37m";              /* White */
    const std::string BOLDBLACK = "\033[1m\033[30m";   /* Bold Black */
    const std::string BOLDRED = "\033[1m\033[31m";     /* Bold Red */
    const std::string BOLDGREEN = "\033[1m\033[32m";   /* Bold Green */
    const std::string BOLDYELLOW = "\033[1m\033[33m";  /* Bold Yellow */
    const std::string BOLDBLUE = "\033[1m\033[34m";    /* Bold Blue */
    const std::string BOLDMAGENTA = "\033[1m\033[35m"; /* Bold Magenta */
    const std::string BOLDCYAN = "\033[1m\033[36m";    /* Bold Cyan */
    const std::string BOLDWHITE = "\033[1m\033[37m";   /* Bold White */
  public:
    typedef enum LOG_LEVEL {
        ERROR = 0,
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
      * @tparam Args 可变长度模板
      * @param args 要打印的内容
      */
    template <typename... Args>
    void error(Args... args)
    {
        _errorCount++;
        auto count = "#FF0000 " + std::to_string(_errorCount) + "#";
        if (errorLabs != nullptr)
            lv_label_set_text(errorLabs, count.c_str());

        auto time = getNowTimer();
        if (!isComp)
        {
            _terminalOut(RED + time);
            _terminalOut(args...);
        }
        _lvglError("#FF0000 " + time + "#");
        _lvglError(args...);
    }
    /**
     * @brief 打印黄色警告信息到控制台和机器人内置控制台
     *
     * @tparam Args 可变长度模板
     * @param args 要打印的内容
     */
    template <typename... Args>
    void warning(Args... args)
    {
        _warningCount++;
        auto count = "#CCFF00 " + std::to_string(_warningCount) + "#";
        if (warningLabs != nullptr)
            lv_label_set_text(errorLabs, count.c_str());

        auto time = getNowTimer();
        if (!isComp)
        {
            _terminalOut(YELLOW + time);
            _terminalOut(args...);
        }
        _lvglWarning("#CCFF00 " + time + "#");
        _lvglWarning(args...);
    }
    /**
     * @brief 打印蓝色调试信息到控制台和机器人内置控制台
     *
     * @tparam Args 可变长度模板
     * @param args 要打印的内容
     */
    template <typename... Args>
    void debug(Args... args)
    {
        if (!isComp)
        {
            _terminalOut(BLUE + getNowTimer());
            _terminalOut(args...);
        }
    }
    /**
     * @brief 打印白色普通信息到控制台和机器人内置控制台
     *
     * @tparam Args 可变长度模板
     * @param args 要打印的内容
     */
    template <typename... Args>
    void info(Args... args)
    {
        if (!isComp)
        {
            _terminalOut(RESET + getNowTimer());
            _terminalOut(args...);
        }
    }
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
    std::stringstream terminalStr[2];
    lv_obj_t *errorLabs = nullptr, *warningLabs = nullptr; //控制台上的显示错误数量的lab

  private:
    Logger();
    ~Logger() {}
    static Logger *_logger; // 单例对象在这里！;
    bool isComp = false;
    std::string getNowTimer();
    template <typename T>
    void _terminalOut(T val)
    {
        _stream << val;
        std::cout << _stream.str();
        _stream.clear(); //1：调用clear()清除当前错误控制状态，其原型为 void clear (iostate state=goodbit);
        _stream.str(""); //2：调用str("")将缓冲区清零，清除脏数据
    }

    template <typename T, typename... Args>
    void _terminalOut(T val, Args... args)
    {
        _stream << val;
        _terminalOut(args...);
    }
    template <typename T>
    void _lvglError(T val)
    {
        terminalStr[0] << val;
    }

    template <typename T, typename... Args>
    void _lvglError(T val, Args... args)
    {
        terminalStr[0] << val;
        _lvglError(args...);
    }
    template <typename T>
    void _lvglWarning(T val)
    {
        terminalStr[1] << val;
    }

    template <typename T, typename... Args>
    void _lvglWarning(T val, Args... args)
    {
        terminalStr[1] << val;
        _lvglWarning(args...);
    }
    std::stringstream _stream; //字符串流
    size_t _errorCount = 0, _warningCount = 0;
};
#define logger Logger::getLogger()
} // namespace ncrapi
