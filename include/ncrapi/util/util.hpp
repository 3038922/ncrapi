#pragma once
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ncrapi {
#define MAX_BUF_LEN 1024
#define MAX_KEY_LEN 64
#define MAX_VAL_LEN 256
using DataTypeDouble = std::vector<std::pair<std::string, double>>;
using DataTypeInt = std::vector<std::pair<std::string, int>>;
using DataTypeString = std::vector<std::pair<std::string, std::string>>;
using DataTypeMotor = std::vector<std::pair<uint8_t, uint8_t>>;

template <typename T>
T copySign(T val)
{
    if (val > 0)
        return 1;
    if (val < 0)
        return -1;
    return 0;
}

/**
 * string类转数字
 * @tparam 要转的类型
 * @param str 
 * @return T 
 */
template <typename T>
T stringToNum(const std::string &str)
{
    std::istringstream iss(str);
    T num;
    iss >> num;
    return num;
}

/**
 * 以VECTOR容器为基础 按关键字设置 
 * @tparam T 数据类型
 * @param data 数据名称
 * @param key 要修改的的值的关键字
 * @param value 修改的值
 * @return true 修改成功
 * @return false 修改失败
 */
template <typename T, typename DATA>
bool setData(DATA &data, std::string key, T value)
{
    if (data.empty())
        return false;
    for (auto &it : data)
        if (it.first == key)
        {
            it.second = value; //如果找到了就修改
            return true;
        }
    data.push_back(std::pair(key, value)); //如果没找到就新增
    return true;
}

/**
 * 按KEY查找关键字 
 * @tparam T 转换的类型
 * @param data 要查找的数据容器
 * @param key 关键字
 * @return T 自动推断
 */
template <typename DATA>
auto read(const DATA &data, std::string key)
{
    auto it = find_if(data.begin(), data.end(), [&key](decltype(*(data.begin())) &a) { return a.first == key; });
    if (it != data.end())
        return (it->second);
    else
        std::cout << "关键字: " << key << " 没有找到" << std::endl;
}

/**
     * 打印数据库到CMD 
     * @param data 要打印的数据库
     */
template <typename DATA>
void showData(DATA &data)
{
    for (auto &it : data)
        std::cout << it.first << " " << it.second << std::endl;
}
/**
 * 从SD中读取文件 存入数据容器
 * @tparam DATA 数据容器类型
 * @param data 数据容器
 * @param filePath 路径
 * @param name 名字
 */
template <typename DATA>
void readSDcard(DATA *data, const std::string &filePath, const std::string name)
{
    FILE *file = fopen(filePath.c_str(), "r");
    if (file == nullptr)
    {
        std::cerr << name << " 文件打开错误,请检查SD卡!" << std::endl;
    }
    else
    {
        char buf[MAX_BUF_LEN];
        int text_comment = 0;
        while (fgets(buf, MAX_BUF_LEN, file) != nullptr)
        {
            //trim
            for (int n = strlen(buf) - 1; n >= 0; n--)
            {
                if (buf[n] != ' ' && buf[n] != '\t' && buf[n] != '\n')
                    break;
                buf[n + 1] = '\0';
            }
            //trim
            // to skip text comment with flags /* ... */
            if (buf[0] != '#' && (buf[0] != '/' || buf[1] != '/'))
            {
                if (strstr(buf, "/*") != NULL)
                {
                    text_comment = 1;
                    continue;
                }
                else if (strstr(buf, "*/") != NULL)
                {
                    text_comment = 0;
                    continue;
                }
            }
            if (text_comment == 1)
            {
                continue;
            }

            int buf_len = strlen(buf);
            // ignore and skip the line with first chracter '#', '=' or '/'
            if (buf_len <= 1 || buf[0] == '#' || buf[0] == '=' || buf[0] == '/')
            {
                continue;
            }
            buf[buf_len - 1] = '\0';

            char _paramk[MAX_KEY_LEN] = {0}, _paramv[MAX_VAL_LEN] = {0};
            int _kv = 0, _klen = 0, _vlen = 0;
            int i = 0;
            for (i = 0; i < buf_len; ++i)
            {
                if (buf[i] == ' ')
                    continue;
                // scan param key name
                if (_kv == 0 && buf[i] != '=')
                {
                    if (_klen >= MAX_KEY_LEN)
                        break;
                    _paramk[_klen++] = buf[i];
                    continue;
                }
                else if (buf[i] == '=')
                {
                    _kv = 1;
                    continue;
                }
                // scan param key value
                if (_vlen >= MAX_VAL_LEN || buf[i] == '#')
                    break;
                _paramv[_vlen++] = buf[i];
            }
            if (strcmp(_paramk, "") == 0 || strcmp(_paramv, "") == 0)
                continue;
            data->push_back(std::make_pair(_paramk, stringToNum<decltype(data->begin()->second)>(_paramv)));
        }
        fclose(file);
    }
}
/**
 * 以vector 容器为基础修改保存文件
 * @tparam T 数据的类型
 * @param data 数据的名称
 * @param filePath  数据的路径
 * @return true 保存成功
 * @return false 保存失败
 */
template <typename DATA>
bool saveData(DATA &data, std::string filePath)
{
    FILE *file = fopen(filePath.c_str(), "w");
    if (file == NULL)
    {
        std::cerr << "文件保存失败" << std::endl;
        return false;
    }
    for (auto &it : data)
        fprintf(file, "%s = %s\n", it.first.c_str(), std::to_string(it.second).c_str());
    fclose(file);
    return true;
}
} // namespace ncrapi