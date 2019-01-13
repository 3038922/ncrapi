
#include "ncrapi\system\systemData.hpp"
namespace ncrapi
{
SystemData::SystemData()
{
    readSDcard(&_pidData, "/usd/pid.txt", "pid");
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
void SystemData::addObj(Obj *generic)
{
    obj.push_back(generic);
}
/**
     *获取当前机器人部件总数 
     * @return size_t 部件总数
     */
size_t SystemData::getObjNums()
{
    return obj.size();
}
/**
     *停止所有部件运作
     */
void SystemData::stopAllObj()
{
    for (auto &it : obj)
        it->stop();
}

void SystemData::addDebugData(std::initializer_list<std::string> val)
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
void SystemData::showDebugData(std::string &str)
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
void SystemData::closeDebugData()
{
    if (debugFile != nullptr)
        fclose(debugFile);
    else
        std::cerr << "debug 文件打开错误,请检查SD卡!" << std::endl;
}
} // namespace ncrapi