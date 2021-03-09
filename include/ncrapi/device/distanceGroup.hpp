#pragma once
#include "ncrapi/device/distance.hpp"
namespace ncrapi {

class DistanceGroup
{
  public:
    /**
   * @brief 传感器的安装位置
   * 
   */
    enum DistancePos {
        FRONT = 0,
        BACK,
        LEFT,
        RIGHT,
    };

  public:
    /**
   * @brief 构造一个激光传感器的组
   * 构造范例    DistanceGroup mydisG("激光传感器组", {Distance("left", 1,200_mm), Distance("right", 2,200_mm)});
   * @param name 组的名字
   * @param distanceList 链表
   * @param offset 激光传感器距离机器中心的偏移量
   */
    DistanceGroup(const std::string name, const std::vector<Distance> &distanceList);
    /**
   * @brief 构造一个激光传感器的组
   *
   * @param name 组的名字
   * @param pragma JSON数据
   */
    DistanceGroup(const std::string name, const json &pragma);
    /**
     * @brief 以获取当前到传感器的测量距离
     * @param pos  传感器编号 FRONT BACK LEFT RIGHT
     * @return QLength 可以转换成毫米 厘米 米等.
     */
    QLength getLength(const DistancePos pos);
    /**
     * @brief 以获取当前到传感器的测量距离加上偏移量
     * 
     * @param pos 传感器编号 FRONT BACK LEFT RIGHT
     * @param offset 偏移量 默认0.375_in 就是加上场地围板的厚度
     * @return QLength 可以转换成毫米 厘米 米等.
     */
    QLength getLengthOffset(const DistancePos pos, const QLength &offset = 0.375_in);
    /**
     * @brief 获取角度偏移量 SLAM使用
     * 
     * @param pos 传感器编号 FRONT BACK LEFT RIGHT
     * @return QAngle 返回预设的偏移量 前0 后180 左90 右-90 
     */
    QAngle getAngleOffset(const DistancePos pos);
    int getDistanceNums();
    void showDistanceInfo(std::ostringstream &ostr);

  protected:
    std::array<Distance *, 4> _distanceList = {nullptr};
    const std::string _name; //组名
};
} // namespace ncrapi