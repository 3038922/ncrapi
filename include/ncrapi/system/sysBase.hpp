#pragma once
#include "json.hpp"
#include "ncrapi/util/util.hpp"
#include "object.hpp"
#include <memory>

namespace ncrapi
{
class SysBase
{

  public:
    //全局数据
    std::string robotInfo;
    json jsonVal; //根数据

    SysBase(const json &pragam);
    //机器人初始参数
    std::vector<Obj *> obj; //存储机器人部件的名字

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
    /**
 * 从SD中读取文件 存入数据容器
 */
    bool readSDcard(json pragam);

    /**
 * 以vector 容器为基础修改保存文件
 */
    bool saveData();
    void print(const json &pragma, std::string_view ignore = nullptr);
    //递归打印
    void recursionPrint(const json &pragma, std::string_view ignore);
};
} // namespace ncrapi

extern std::unique_ptr<ncrapi::SysBase> sysData;