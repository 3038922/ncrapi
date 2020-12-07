#pragma once
#include "json.hpp"
#include "ncrapi/i18n/chinese.hpp"
#include "ncrapi/units/QLength.hpp"
#include "ncrapi/util/util.hpp"
#include "object.hpp"
#include "pros/misc.hpp"

namespace ncrapi {
enum ROBOTMODE {
    NORMAL = 0,
    PID_SPEED,
    PID_FR,
    PID_ROTATE,
    PID_AUTO_AIMING,
    PID_DRIVER_SQUARE,
    ODOM_DRIVER_SQUARE,
    AUTONOMOUS,
    CUSTOM_TEST
};

class NcrSystem
{
  public:
    //饿汉模式单例实现.线程安全
    static NcrSystem *initNcrSystem(const json &pragma);
    //但获取单例指针
    static NcrSystem *getNcrSystem();
    json &setJson() { return _jsonVal; }
    //全局数据
    const json getJson() { return _jsonVal; }
    /**
     * @brief 获取机器人当前模式
     * 
     * @return ROBOTMODE 返回模式 0 正常模式 1 调试模式
     */
    ROBOTMODE getRobotMode() { return _mode; }
    /**
     * @brief 设置当前机器人模式
     * 
     * @param mode 模式
     */
    void setRobotMode(ROBOTMODE mode);

    //逻辑相关
    std::vector<Obj *> obj; //存储机器人部件的名字
    /**
     * @brief 部件初始化检查模块
     * 
     * @param name 部件名字
     * @param port 部件端口
     * @return true 没有错误
     * @return false 有错误
     */
    bool deviceCheck(const std::string name, const int port);
    /**
      * @brief 检查I2C 1-21是否有冲突
      * @param name 部件的名字
      * @param port 使用的端口号
      */
    void i2cCheck(const std::string name, const int port);
    /**
      * @brief 检查ADI 1-8是否有冲突
      * @param name 部件的名字
      * @param port 使用的端口号
      */
    void adiCheck(const std::string name, const int port);
    /**
      * @brief 检查ADI 1-8是否有冲突 主要用于双线的ADI传感器
      * @param name 部件的名字
      * @param port 使用的端口号
      */
    void adiCheck(const std::string name, const std::pair<int, int> port);

    /**
      * @brief 增加部件名字 
      * @param str 部件的名字
      */
    void addObj(Obj *generic);
    /**
     * @brief 获取当前是红方还是蓝方
     * 
     * @return true 蓝方
     * @return false 红方
     */
    bool getSide();
    /**
     * @brief 获取红方蓝方的char标记 用于分球判断
     * 
     * @return char 红方会返回'R' 蓝方返回 'B'
     */
    char getSideFlag();
    /**
     * @brief 获取当前机器人部件总数 
     * @return size_t 部件总数
     */
    size_t getObjNums();
    /**
     *@brief 停止所有部件运作
     */
    void stopAllObj();

    /**
      * @brief  以vector 容器为基础修改保存文件
      */
    bool saveData();

    //PID参数数据
    std::vector<std::vector<double>> _recoderData; //记录机器人数据
    void testAction(void (*autonomous)(), void (*customTest)(), std::shared_ptr<pros::Controller> joy);

    //PID相关
    double pidTestTarget = 0;                                     //PID TEST的目标值
    std::pair<QTime, double> pidData[2] = {{0_ms, 0}, {0_ms, 0}}; //接受PID测试时候的返回值

    //录像参数
    int recoderMode = 0; //0 不录像 1录像 2 回放
    void recoder(std::vector<double> &data);
    void showRecoder();

  private:
    char _sideFlag; //红方标记为B 蓝方标记为R
    NcrSystem(const json &pragma);
    static NcrSystem *_ncrSystem; // 单例对象
    json _jsonVal;                //根数据
    std::vector<int> _i2cPort;    //1-21端口号
    std::vector<int> _adiPort;    //1-8adi端口号
    /**
 * 从SD中读取文件 存入数据容器
 */
    bool readSDcard(json pragma);

    void print(const json &pragma, std::string_view ignore = "");
    //递归打印
    void recursionPrint(const json &pragma, std::string_view ignore);
    //遥控模式下的参数
    ROBOTMODE _mode = NORMAL;
};
#define ncrSys NcrSystem::getNcrSystem()
} // namespace ncrapi