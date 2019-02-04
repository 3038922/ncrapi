#pragma once
#include <iostream>
#include <sstream>

namespace ncrapi
{
//数学常数
static constexpr double analogInToV = 286.0;
static constexpr double inchToMM = 25.4;
static constexpr double degreeToRadian = 0.01745;
static constexpr double radianToDegree = 57.2957795;
static constexpr double pi = 3.1415926535897932;
template <typename T>
T copySign(T val)
{
    if (val > 0)
        return 1;
    if (val < 0)
        return -1;
    return 0;
}

template <typename T>
T clamp(const T &val, const T min, const T max, std::string_view name)
{
    T temp = val;
    if (val < min)
    {
        temp = min;
        std::cerr << name << "超限,已设置为最小值:" << min << std::endl;
    }
    else if (val > max)
    {
        temp = max;
        std::cerr << name << "超限,已设置为最大值:" << max << std::endl;
    }
    return temp;
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