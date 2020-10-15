#pragma once
#include "ncrapi/system/logger.hpp"
#include <sstream>

namespace ncrapi {

template <typename T>
int copySign(T val)
{
    if (val > 0.0)
        return 1;
    else if (val < 0.0)
        return -1;
    else
        return 0;
}
/**
 * 限制最大最小值的模板夹子函数，取值范围在min~max之间
 * @param val 输入值
 * @param min 最小值
 * @param max 最大值
 * @return T 
 */
template <typename T>
T clamp(const T &val, const T min, const T max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    else
        return val;
}
/**
 * 限制最大最小值的模板夹子函数，取值范围在min~max之间
 * @param val 输入值
 * @param min 最小值
 * @param max 最大值
 * @return T 
 */

template <typename T>
T clamp(const T &val, const T min, const T max, std::string name)
{
    if (val < min)
    {
        logger->warning({name, "超限,输入值:", std::to_string(val), "已设置为最小值:", std::to_string(min)});
        return min;
    }
    else if (val > max)
    {
        logger->warning({name, "超限,输入值:", std::to_string(val), "已设置为最大值:", std::to_string(max)});
        return max;
    }
    else
        return val;
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

} // namespace ncrapi