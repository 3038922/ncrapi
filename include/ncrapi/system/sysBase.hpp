#pragma once
#include "json.hpp"
#include "ncrapi/util/util.hpp"
#include "object.hpp"

namespace ncrapi {
class SysBase
{

  public:
    //全局数据
    json jsonVal; //根数据
    SysBase(const json &paragam);
    //机器人初始参数
    std::vector<Obj *> obj; //存储机器人部件的名字

    /**
 * @brief 检查I2C 1-21是否有冲突
 * 
 * @param port 使用的端口号
 * @param name 部件的名字
 */
    void i2cCheck(const int port, const std::string name);
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
    //遥控模式下的参数
    bool isOPcontrol = 1;  //是否开启遥控模式 1开启 0关闭
    bool isAutonomous = 0; //是否开启自动赛测试 1开启 0关闭 主要画图用

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

  private:
    std::vector<int> _i2cPort; //1-21端口号
    std::vector<int> _adiPort; //1-8adi端口号
    /**
 * 从SD中读取文件 存入数据容器
 */
    bool readSDcard(json paragam);

    void print(const json &pragma, std::string_view ignore = "");
    //递归打印
    void recursionPrint(const json &pragma, std::string_view ignore);
};
} // namespace ncrapi
