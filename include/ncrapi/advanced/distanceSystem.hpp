#pragma once
#include "ncrapi/device/distanceGroup.hpp"

namespace ncrapi {
using SegmentAngle = std::tuple<std::string, QAngle, QAngle>; //0 角名字 1 角  2 分段
using SegmentAngle8 = std::array<SegmentAngle, 8>;            //0-下1 1-下2 2-右1 3-右2 4-上1 5-上2 6-左1 7-左2
using Coordinate = std::tuple<QLength, QLength, QAngle>;

class DistanceSystem : public DistanceGroup
{
  protected:
    enum ODOMSTATE {
        X = 0,
        Y,
        THETA
    };
    enum ORIENTATION {
        DOWN1 = 0,
        DOWN2,
        RIGHT1,
        RIGHT2,
        UP1,
        UP2,
        LEFT1,
        LEFT2
    };

  public:
    //激光传感器系统初始化
    static DistanceSystem *initDistanceSystem(const json &pragma);
    //但获取单例指针
    static DistanceSystem *getDistanceSystem();
    /**
     * @brief 使用激光传感器自动校准X 或者Y 轴坐标
     * 
     * @param nowCoordinate 当前坐标的引用
     */
    void calibrationCoordinates(Coordinate &nowCoordinate);

  private:
    /**
   * @brief 根据当前X Y坐标分段
   * 
   * @param x X坐标
   * @param y Y坐标
   */
    void _subsection(const QLength x, const QLength y);
    /**
     * @brief 分段算法
     * 
     * @param nowCoordinate 当前坐标的引用
     * @param distance 传感器的返回距离
     * @param angleOffset 角度偏移量
     * @param name 传感器名字
     */
    void _segmentedJudgment(Coordinate &nowCoordinate, const QLength distance, const QAngle angleOffset, const std::string &name);
    static DistanceSystem *_distanceSystem; // 单例对象
    DistanceSystem(const json &pragma);
    SegmentAngle8 _segmentAngle; //关于角的分段
};
#define distanceSystem DistanceSystem::getDistanceSystem()
} // namespace ncrapi