#pragma once
#include "ncrapi/device/distance.hpp"
namespace ncrapi {

class DistanceGroup
{
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
     * @param nums  传感器编号
     * @return QLength 可以转换成毫米 厘米 米等.
     */
    QLength getLength(const int nums);
    /**
     * @brief 以获取当前到传感器的测量距离加上偏移量
     * 
     * @param nums 传感器编号
     * @return QLength 可以转换成毫米 厘米 米等.
     */
    QLength getLengthOffset(const int nums);
    int getDistanceNums();
    void showDistanceInfo(std::ostringstream &ostr);

  protected:
    std::vector<Distance> _distanceList;
    const std::string _name; //组名
};
} // namespace ncrapi