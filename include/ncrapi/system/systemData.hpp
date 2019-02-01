#pragma once

#include "json.hpp"
#include "ncrapi/util/util.hpp"
#include "object.hpp"
#define AUTO_NUMS 5
namespace ncrapi
{
class SystemData
{

  public:
    json jsonVal; //根数据
    FILE *debugFile = nullptr;
    std::string robotInfo;
    SystemData(const json &pragam);
    //机器人初始参数
    std::vector<Obj *> obj; //存储机器人部件的名字

    //自动赛参数
    bool autoIsMode = 0; //自动赛模式 普通自动0 纯自动1
    int autoSide = 0;    //红方 0 蓝方360
    std::array<bool, AUTO_NUMS> autoFlags;
    using AutoDataType = std::array<bool, AUTO_NUMS>;

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
    bool readSDcard();
    bool saveData();
};

} // namespace ncrapi
typedef enum auto_flag
{
    AUTO_FR,        //前场0 后场1
    AUTO_SHOOT,     //射高旗0 射中旗1
    AUTO_MID_SHOOT, //是否射中间的旗子 0不射 1射
    AUTO_PLATFORM,  //0不开台 1 开台
    AUTO_BUMPERFLAG //0不撞中间旗 1 撞中间旗子
} auto_flag;
//旗子枚举
typedef enum flagPosName
{
    leftHighFlag = 0,
    leftMiddleFlag,
    leftLowFlag,
    middleHighFlag,
    middleMiddleFlag,
    middleLowFlag,
    rightHighFlag,
    rightMiddleFlag,
    rightLowFlag
} flagPosName;
extern ncrapi::SystemData *sysData;
