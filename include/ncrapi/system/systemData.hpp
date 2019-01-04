#pragma once
#include "api.h"
#include "configSet.hpp"
#include "object.hpp"
#include <array>

#define AUTO_NUMS 5
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

namespace ncrapi
{
class SystemData
{
  public:
    //机器人初始参数
    using dataType = std::vector<std::pair<std::string, std::string>>;
    using citer = std::vector<std::pair<std::string, std::string>>::const_iterator; //迭代器别名

    dataType _pidData;
    FILE *debugFile = nullptr;

    std::vector<Obj *> obj; //存储机器人部件的名字

    //自动赛参数
    bool autoIsMode = 0; //自动赛模式 普通自动0 纯自动1
    int autoSide = 0;    //红方 0 蓝方360
    std::array<bool, AUTO_NUMS> autoFlags;
    //传感器数据
    //构造函数
    SystemData()
    {
        Config pid(&_pidData, "/usd/pid.txt");    //读取PID配置
        debugFile = fopen("/usd/debug.txt", "a"); //以附加方式打开
        if (debugFile == nullptr)
        {
            std::cerr << "debug 文件打开错误,请检查SD卡!" << std::endl;
        }
    }
    /**
    *增加部件名字 
    * @param str 部件的名字
    */
    void
    addObj(Obj *generic)
    {
        obj.push_back(generic);
    }
    /**
     *获取当前机器人部件总数 
     * @return size_t 部件总数
     */
    size_t getObjNums()
    {
        return obj.size();
    }
    void addDebugData(std::initializer_list<std::string> val)
    {
        std::string str;
        for (auto &it : val)
            str += it;
        if (debugFile != nullptr)
        {
            std::cerr << str << std::endl;
            uint32_t nowTime = pros::millis();
            uint32_t minutes = (nowTime % (1000 * 60 * 60)) / 60000;
            uint32_t seconds = (nowTime % (1000 * 60)) / 1000;
            std::string time;
            time += std::to_string(minutes);
            time += ":";
            time += std::to_string(seconds);
            fprintf(debugFile, "%s  %s\n", time.c_str(), str.c_str());
        }
        else
            std::cerr << "debug 文件打开错误,请检查SD卡!" << std::endl;
    }
    void showDebugData(std::string &str)
    {
        if (debugFile != nullptr)
        {
            fclose(debugFile);
            debugFile = fopen("/usd/debug.txt", "r"); //先关闭写模式再以读模式打开
            char buf[MAX_BUF_LEN];
            while (fgets(buf, MAX_BUF_LEN, debugFile) != nullptr)
                str += buf;
            fclose(debugFile);                        //先关闭读模式
            debugFile = fopen("/usd/debug.txt", "a"); //再继续以写模式打开
        }
        else
            std::cerr << "debug 文件打开错误,请检查SD卡!" << std::endl;
    }
    void closeDebugData()
    {
        if (debugFile != nullptr)
            fclose(debugFile);
        else
            std::cerr << "debug 文件打开错误,请检查SD卡!" << std::endl;
    }
    /**
     * 打印数据库到CMD 
     * @param data 要打印的数据库
     */
    void showData(dataType &data)
    {
        for (auto &it : data)
            std::cout << it.first << " " << it.second << std::endl;
    }

    /**
     * 查找函数
     * @param data 要查找的数据库
     * @param  key 关键字
     * @return     返回迭代器(在VECTOR中的位置 没找到返回END())
     */
    citer find(dataType &data, std::string key)
    {
        citer it = data.begin();
        for (; it != data.end(); it++)
        {
            if (it->first == key)
                return it;
        }
        return data.end();
    }

    /**
     * 按KEY查找CONFIG的值
     * @param data 要读取的数据库
     * @param  key 关键字
     * @return     关键字对应的值
     */
    template <class T>
    T read(dataType &data, std::string key)
    {
        citer it = find(data, key);
        if (it == data.end())
            std::cerr << "Key not found" << std::endl;
        else
            return stringToNum<T>(it->second);
    }

    /**
     * 修改config的值
     * @param data 要修改的数据库
     * @param  key   关键字
     * @param  value 关键字对应的值
     * @return        修改成功返回true 失败返回false
     */
    bool setConfig(dataType &data, std::string key, std::string value)
    {
        if (data.empty())
            return false;
        for (auto &it : data)
            if (it.first == key)
            {
                it.second = value; //如果找到了就修改
                return true;
            }
        data.push_back(std::pair<std::string, std::string>(key, value)); //如果没找到就新增
        return true;
    }
    /**
     * 遍历保存config
     * @param filePath 要保存文件的路径名
     * @param data 要保存的数据库
     * @return 成功返回true 失败返回false
     */
    bool saveConfig(dataType &data, std::string filePath)
    {
        FILE *file = fopen(filePath.c_str(), "w");
        if (file == NULL)
        {
            std::cerr << "file open error" << std::endl;
            return false;
        }
        for (auto &it : data)
            fprintf(file, "%s = %s\n", it.first.c_str(), it.second.c_str());
        fclose(file);
        return true;
    }
    template <class T>
    static T stringToNum(const std::string &str);
}; // namespace ncrapi
template <class T>
T SystemData::stringToNum(const std::string &str)
{
    std::istringstream iss(str);
    T num;
    iss >> num;
    return num;
}
} // namespace ncrapi
extern ncrapi::SystemData *sysData;
