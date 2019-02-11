
#include "ncrapi/ncrapi.hpp"
// flagCoordinate flagCoordinate[2][9] = {{{289, 387, 1080}, {289, 387, 724}, {289, 387, 369}, {190, 1681, 1080}, {190, 1681, 724}, {190, 1681, 369}, {289, 3112, 1080}, {289, 3112, 724}, {289, 3112, 369}},                                                                          //普通自动赛坐标
//                                        {{289, 387, 1080}, {289, 387, 724}, {289, 387, 369}, {289, 387 + 2 * MAT_SIZE, 1080}, {289, 2 * MAT_SIZE, 724}, {289, 2 * MAT_SIZE, 369}, {289, 387 + 4 * MAT_SIZE, 1080}, {289, 387 + 4 * MAT_SIZE, 724}, {289, 387 + 4 * MAT_SIZE, 369}}}; //纯自动旗子坐标

namespace ncrapi
{
SystemData::SystemData(const json &pragam) : jsonVal(pragam)
{

    //系统信息录入
    robotInfo = jsonVal["系统信息"]["机器人类型"];
    robotInfo += jsonVal["系统信息"]["队伍编号"];
    robotInfo += jsonVal["系统信息"]["用户"];
    //DEBUG模式打开
    debugFile = fopen("/usd/debug.txt", "a"); //以附加方式打开
    if (debugFile == nullptr)
    {
        std::cerr << "debug 文件打开错误,请检查SD卡!" << std::endl;
    }

    std::cout << "系统类构造成功" << std::endl;
}

/*
 * 从SD中读取文件 存入数据容器
 * @tparam DATA 数据容器类型
 * @param data 数据容器
 * @param filePath 路径
 * @param name 名字
 */

bool SystemData::readSDcard()
{
    FILE *file = fopen("config.json", "r");
    if (file == nullptr)
    {
        std::cerr << "json 文件打开错误" << std::endl;
        return false;
    }
    char buf[1024];
    std::string line;
    while (fgets(buf, 1024, file) != nullptr) //读取一行
        line += buf;
    jsonVal = json::parse(line);
    //std::cout << line << std::endl;
    fclose(file);
    return true;
}

/**
 * 以vector 容器为基础修改保存文件
 * @tparam T 数据的类型
 * @param data 数据的名称
 * @param filePath  数据的路径
 * @return true 保存成功
 * @return false 保存失败
 */

bool SystemData::saveData()
{
    FILE *file = fopen("config.json", "w");
    if (file == nullptr)
    {
        std::cerr << "json 保存失败" << std::endl;
        return false;
    }
    fprintf(file, "%s", jsonVal.dump(4).c_str()); //保存
    std::cout << "保存成功" << std::endl;
    fclose(file);
    return true;
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
        char buf[1024];
        while (fgets(buf, 1024, debugFile) != nullptr)
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

void SystemData::print(const json &pragma, std::string_view ignore)
{
    for (auto &it : pragma.items())
        if (it.key() != ignore)
            for (auto &it1 : it.value().items())
            {
                std::cout << it1.key() << " ";
                for (auto &it2 : it1.value().items())
                    std::cout << it2.key() << ":" << it2.value() << " ";
                std::cout << std::endl;
            }
}
//递归打印
void SystemData::recursionPrint(const json &pragma, std::string_view ignore)
{
    for (auto &[key, val] : pragma.items())
    {
        if (key != ignore)
        {
            if (val.is_structured())
            {
                std::cout << "\n"
                          << key << ": "; //这里可以右移
                recursionPrint(val, ignore);
            }
            else
            {
                std::cout << key << ":" << val << " ";
            }
        }
    }
    return;
}

} // namespace ncrapi
