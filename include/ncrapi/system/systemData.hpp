#pragma once
#include "json.hpp"
#include "ncrapi/util/util.hpp"
#include "object.hpp"
#include <memory>
namespace ncrapi
{
class SystemData
{
    using PidDebugTuple = std::tuple<uint32_t, double, double, double, double, double>; //PIDDATA的类型

  public:
    json jsonVal; //根数据
    //PID参数数据
    std::vector<PidDebugTuple> pidDebugData; //PID DEBUG数据
    FILE *debugFile = nullptr;

    SystemData(const json &pragam);
    //机器人初始参数
    std::vector<Obj *> obj; //存储机器人部件的名字
    //自动赛参数
    int autoSide = 0; //红方 0 蓝方360
                      // std::vector<bool> autoFlag; //临时存存
    //遥控模式下的参数
    bool isOPcontrol = 1; //是否开启遥控模式 1开启 0关闭

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

    void addDebugData(std::initializer_list<std::string> val);
    void showDebugData(std::string &str);
    void closeDebugData();
    // void recoder(std::vector<double> &data);
    void showRecoder();
    /**
 * 从SD中读取文件 存入数据容器
 * @tparam DATA 数据容器类型
 * @param data 数据容器
 * @param filePath 路径
 * @param name 名字
 */

    bool readSDcard();

    /**
 * 以vector 容器为基础修改保存文件
 * @tparam T 数据的类型
 * @param data 数据的名称
 * @param filePath  数据的路径
 * @return true 保存成功
 * @return false 保存失败
 */

    bool saveData();
    void print(const json &pragma, std::string_view ignore = nullptr);
    //递归打印
    void recursionPrint(const json &pragma, std::string_view ignore);
    void frTest();
    void rotateTest();
};
} // namespace ncrapi
extern std::unique_ptr<ncrapi::SystemData> sysData;
