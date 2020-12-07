#pragma once
#include "ncrapi/device/distanceGroup.hpp"
namespace ncrapi {

class DistanceSystem : public DistanceGroup
{
  public:
    //激光传感器系统初始化
    static DistanceSystem *initDistanceSystem(const json &pragma);
    //但获取单例指针
    static DistanceSystem *getDistanceSystem();

  private:
    static DistanceSystem *_distanceSystem; // 单例对象
    DistanceSystem(const json &pragma);
};
#define distanceSystem DistanceSystem::getDistanceSystem()
} // namespace ncrapi