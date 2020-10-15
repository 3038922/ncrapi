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
QUANTITY_TYPE(0, 0, -1, 1, QAngularSpeed)

constexpr QAngularSpeed radps = radian / second;
constexpr QAngularSpeed rpm = (360 * degree) / minute;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static QAngularSpeed convertHertzToRadPerSec(QFrequency in)
{
    return (in.convert(Hz) / 2_pi) * radps;
}
#pragma GCC diagnostic pop

inline namespace literals {
constexpr QAngularSpeed operator"" _radps(long double x)
{
    return x * radps;
}
constexpr QAngularSpeed operator"" _radps(unsigned long long int x)
{
    return static_cast<double>(x) * radps;
}
constexpr QAngularSpeed operator"" _rpm(long double x)
{
    return x * rpm;
}
constexpr QAngularSpeed operator"" _rpm(unsigned long long int x)
{
    return static_cast<double>(x) * rpm;
}
} // namespace literals
} // namespace ncrapi
