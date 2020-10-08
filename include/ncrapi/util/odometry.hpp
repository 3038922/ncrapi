#pragma once
#include "ncrapi/system/logger.hpp"
#include "ncrapi/units/QAngle.hpp"
#include "ncrapi/units/QLength.hpp"
#include <iostream>
#include <sstream>
#include <tuple>

namespace ncrapi {

class OdomState
{

  public:
    QLength x = 0_mm, y = 0_mm;
    QAngle theta = 0_deg;
    OdomState() {}
};

class DistanceAndAngle
{
  public:
    QLength length = 0_mm;
    QAngle theta = 0_deg;
    DistanceAndAngle(const QLength ilength, const QAngle itheta) : length(ilength), theta(itheta) {}

    DistanceAndAngle() {}
};

class OdomMath
{
  private:
    OdomMath() {}

  public:
    /**
     * 计算从机器人到点的距离
     * @param  ix     X 坐标
     * @param  iy     Y 坐标
     * @param  istate 当前状态
     * @return        Distance between the points
     */
    static QLength computeDistanceToPoint(const QLength ix, const QLength iy, const OdomState &istate);
    /**
     * 计算从给定的测距状态到给定点的角度。
     * @param  ix     X coordinate
     * @param  iy     Y coordinate
     * @param  istate Odometry state
     * @return        Angle to the point
     */
    static QAngle computeAngleToPoint(const QLength ix, const QLength iy, const OdomState &istate);

    /**
     * 计算从给定的测距状态到给定点的距离和角度。
     * @param  ix     X coordinate
     * @param  iy     Y coordinate
     * @param  istate Odometry state
     * @return        Distance and angle to the point
     */
    static DistanceAndAngle computeDistanceAndAngleToPoint(const QLength ix, const QLength iy, const OdomState &istate);

    /**
     * 试图猜测底盘和车轮直径的两种测距尺度。这可能会让你近距离，但它只是一个快速和临时修复。两个测距尺度必须通过实验找到您的特定机器人。
     * @param chassisDiam 底盘任意一角到中心的轮距以毫米为单位
     * @param wheelDiam   边缘到轮缘的轮径，单位为毫米 大万象4英寸 中万向3.25
     * @param ticksPerRev 每轮一圈旋转方形编码器刻度（默认值为360）
     */
    static std::tuple<double, double> guessScales(const QLength chassisDiam, const QLength wheelDiam, const double ticksPerRev = 360.0);
};
} // namespace ncrapi
