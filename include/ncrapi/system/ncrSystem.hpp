#pragma once
#include "json.hpp"
#include "ncrapi/i18n/chinese.hpp"
#include "ncrapi/units/QLength.hpp"
#include "ncrapi/util/util.hpp"
#include "object.hpp"
#include "pros/misc.hpp"


namespace ncrapi {

class NcrSystem
{
  public:
    //饿汉模式单例实现.线程安全
    static NcrSystem *initNcrSystem(const json &pragma);
    //但获取单例指针
    static NcrSystem *getNcrSystem();
    //全局数据
    json jsonVal; //根数据

    //遥控模式下的参数
    bool isOPcontrol = 1;  //是否开启遥控模式 1开启 0关闭
    bool isAutonomous = 0; //是否开启自动赛测试 1开启 0关闭 主要画图用
    //逻辑相关
    bool isInit = false;
    bool isShooted = false; //是否应发射 这里可能有线程同步的问题    //机器人初始参数
    std::vector<Obj *> obj; //存储机器人部件的名字

    /**
 * @brief 检查I2C 1-21是否有冲突
 * 
 * @param port 使用的端口号
 * @param name 部件的名字
 */
    void
    i2cCheck(const int port, const std::string name);
    /**
 * @brief 检查ADI 1-8是否有冲突
 * 
 * @param port 使用的端口号
 * @param name 部件的名字
 */
    void adiCheck(const int port, const std::string name);
    /**
 * @brief 检查ADI 1-8是否有冲突 主要用于双线的ADI传感器
 * 
 * @param port 使用的端口号
 * @param name 部件的名字
 */
    void adiCheck(const std::pair<int, int> port, const std::string name);

    /**
    *增加部件名字 
    * @param str 部件的名字
    */
    void addObj(Obj *generic);
    /**
     *获取当前机器人部件总数 
     * @return size_t 部件总数
     */
    size_t getObjNums();
    /**
     *停止所有部件运作
     */
    void stopAllObj();

    /**
 * 以vector 容器为基础修改保存文件
 */
    bool saveData();

    //PID参数数据
    std::vector<std::vector<double>> _recoderData; //记录机器人数据
    void testAction(void (*autonomous)(), void (*customTest)(), std::shared_ptr<pros::Controller> joy);

    //PID相关
    size_t test = 0;                                              //0关闭 1 前后 2 左右
    double pidTestTarget = 0;                                     //PID TEST的目标值
    std::pair<QTime, double> pidData[2] = {{0_ms, 0}, {0_ms, 0}}; //接受PID测试时候的返回值

    //录像参数
    int recoderMode = 0; //0 不录像 1录像 2 回放
    void recoder(std::vector<double> &data);
    void showRecoder();

  private:
    NcrSystem(const json &pragma);
    static NcrSystem *_ncrSystem; // 单例对象
    std::vector<int> _i2cPort;    //1-21端口号
    std::vector<int> _adiPort;    //1-8adi端口号
    /**
 * 从SD中读取文件 存入数据容器
 */
    bool readSDcard(json pragma);

    void print(const json &pragma, std::string_view ignore = "");
    //递归打印
    void recursionPrint(const json &pragma, std::string_view ignore);
};
#define ncrSys NcrSystem::getNcrSystem()
} // namespace ncrapi