#include "ncrapi/util/odometry.hpp"
namespace ncrapi {

/**
     * 计算从机器人到点的距离
     * @param  ix     X 坐标
     * @param  iy     Y 坐标
     * @param  istate 当前状态
     * @return        Distance between the points
     */
QLength OdomMath::computeDistanceToPoint(const QLength ix, const QLength iy, const OdomState &istate)
{
    const QLength xDiff = ix - istate.x;
    const QLength yDiff = iy - istate.y;
    return ((xDiff * xDiff) + (yDiff * yDiff)).sqrt();
}
/**
     * 计算从给定的测距状态到给定点的角度。
     * @param  ix     X coordinate
     * @param  iy     Y coordinate
     * @param  istate Odometry state
     * @return        Angle to the point
     */
QAngle OdomMath::computeAngleToPoint(const QLength ix, const QLength iy, const OdomState &istate)
{
    const QLength xDiff = ix - istate.x;
    const QLength yDiff = iy - istate.y;
    return QAngle(std::atan2(yDiff.getValue(), xDiff.getValue()) * radian) - istate.theta;
}

/**
     * 计算从给定的测距状态到给定点的距离和角度。
     * @param  ix     X coordinate
     * @param  iy     Y coordinate
     * @param  istate Odometry state
     * @return        Distance and angle to the point
     */
DistanceAndAngle OdomMath::computeDistanceAndAngleToPoint(const QLength ix, const QLength iy, const OdomState &istate)
{
    const QLength xDiff = ix - istate.x;
    const QLength yDiff = iy - istate.y;
    DistanceAndAngle out;
    out.length = ((xDiff * xDiff) + (yDiff * yDiff)).sqrt();
    out.theta = QAngle(std::atan2(yDiff.getValue(), xDiff.getValue()) * radian) - istate.theta; //弧度转角度后-当前角度
    return out;
}

/**
     * 试图猜测底盘和车轮直径的两种测距尺度。这可能会让你近距离，但它只是一个快速和临时修复。两个测距尺度必须通过实验找到您的特定机器人。
     * @param chassisDiam 底盘任意一角到中心的轮距以毫米为单位
     * @param wheelDiam   轮子直径，单位为毫米
     * @param ticksPerRev 每轮一圈旋转方形编码器刻度（默认值为360）
     */
std::tuple<double, double> OdomMath::guessScales(const QLength chassisDiam, const QLength wheelDiam, const double ticksPerRev)
{
    //const float scale = ((wheelDiam * pi * inchToMM) / ticksPerRev) * 0.9945483364; //这个比例通常超过这个数额。
    const double scale = ((wheelDiam.convert(millimeter) * 1_pi) / ticksPerRev); //直径
    //const float turnScale = (1.0 / (chassisDiam * inchToMM)) * radianToDegree * 2;
    const double turnScale = 180.0 / 1_pi / chassisDiam.convert(millimeter) * scale; //轮间距
    return std::make_tuple(turnScale, scale);
}
} // namespace ncrapi
