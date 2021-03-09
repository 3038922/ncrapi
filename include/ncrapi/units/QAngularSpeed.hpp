/*
 * This code is a modified version of Benjamin Jurke's work in 2015. You can read his blog post
 * here:
 * https://benjaminjurke.com/content/articles/2015/compile-time-numerical-unit-dimension-checking/
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once
#include "ncrapi/units/QAngle.hpp"
#include "ncrapi/units/QFrequency.hpp"
#include "ncrapi/units/QTime.hpp"
#include "ncrapi/units/RQuantity.hpp"

namespace ncrapi {
QUANTITY_TYPE(0, 0, -1, 1, QAngularSpeed)               //角速度
constexpr QAngularSpeed cps = (0.01 * degree) / second; // 角度/秒
constexpr QAngularSpeed radps = radian / second;        // 弧度/秒
constexpr QAngularSpeed rpm = (360 * degree) / minute;  // 几转/分
constexpr QAngularSpeed rps = (360 * degree) / second;  // 几转/秒

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static QAngularSpeed convertHertzToRadPerSec(QFrequency in)
{
    return (in.convert(Hz) / 2_pi) * radps;
}
#pragma GCC diagnostic pop

inline namespace literals {
/**
* @brief 角度/秒
* 
*/
constexpr QAngularSpeed operator"" _cps(long double x)
{
    return x * cps;
}
/**
    * @brief 角度/秒
    * 
    */
constexpr QAngularSpeed operator"" _cps(unsigned long long int x)
{
    return static_cast<double>(x) * cps;
}
/**
* @brief 弧度/秒
* 
*/
constexpr QAngularSpeed operator"" _radps(long double x)
{
    return x * radps;
}
/**
    * @brief 弧度/秒
    * 
    */
constexpr QAngularSpeed operator"" _radps(unsigned long long int x)
{
    return static_cast<double>(x) * radps;
}
/**
 * @brief 几转/分
 * 
 */
constexpr QAngularSpeed operator"" _rpm(long double x)
{
    return x * rpm;
}
/**
 * @brief 几转/分
 * 
 */
constexpr QAngularSpeed operator"" _rpm(unsigned long long int x)
{
    return static_cast<double>(x) * rpm;
}
/**
 * @brief 几转/秒
 * 
 */
constexpr QAngularSpeed operator"" _rps(long double x)
{
    return x * rps;
}
/**
 * @brief 几转/秒
 * 
 */
constexpr QAngularSpeed operator"" _rps(unsigned long long int x)
{
    return static_cast<double>(x) * rps;
}
} // namespace literals
} // namespace ncrapi
