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

#include "ncrapi/units/QLength.hpp"
#include "ncrapi/units/QTime.hpp"
#include "ncrapi/units/RQuantity.hpp"

namespace ncrapi {
QUANTITY_TYPE(0, 1, -1, 0, QSpeed)
constexpr QSpeed mmps = millimeter / second; //毫米/每秒
constexpr QSpeed cmps = centimeter / second; //厘米/每秒
constexpr QSpeed mps = meter / second;       //米/每秒
constexpr QSpeed miph = mile / hour;         //英里/每小时
constexpr QSpeed kmph = kilometer / hour;    //公里每小时

inline namespace literals {
constexpr QSpeed operator"" _mmps(long double x)
{
    return static_cast<double>(x) * mmps;
}
constexpr QSpeed operator"" _cmps(long double x)
{
    return static_cast<double>(x) * cmps;
}
constexpr QSpeed operator"" _mps(long double x)
{
    return static_cast<double>(x) * mps;
}
constexpr QSpeed operator"" _miph(long double x)
{
    return static_cast<double>(x) * mile / hour;
}
constexpr QSpeed operator"" _kmph(long double x)
{
    return static_cast<double>(x) * kilometer / hour;
}
constexpr QSpeed operator"" _mmps(unsigned long long int x)
{
    return static_cast<double>(x) * mmps;
}
constexpr QSpeed operator"" _cmps(unsigned long long int x)
{
    return static_cast<double>(x) * cmps;
}
constexpr QSpeed operator"" _mps(unsigned long long int x)
{
    return static_cast<double>(x) * mps;
}
constexpr QSpeed operator"" _miph(unsigned long long int x)
{
    return static_cast<double>(x) * mile / hour;
}
constexpr QSpeed operator"" _kmph(unsigned long long int x)
{
    return static_cast<double>(x) * kilometer / hour;
}
} // namespace literals
} // namespace ncrapi
