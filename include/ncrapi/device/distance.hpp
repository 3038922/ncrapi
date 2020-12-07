#pragma once
#include "ncrapi/system/json.hpp"
#include "ncrapi/units/QLength.hpp"
#include "pros/distance.hpp"

namespace ncrapi {
// V5距离传感器可测量到物体的距离，近似的物体尺寸和接近速度。
// 测距范围为20mm至2,000mm。低于200mm时，精度约为+/- 15mm，高于200mm时，精度约为5％。
// 报告的近似物体大小为小，中或大，可用于确定目标是墙壁还是游戏物体。接近速度测量传感器和物体的接近速度。
// V5距离传感器使用教室安全的1类激光，类似于现代手机中用于头部检测的激光。激光使传感器的视野非常狭窄，因此检测始终直接在传感器的前面。
class Distance : public pros::Distance
{
  public:
    /**
   * @brief 激光传感器构造函数
   * 
   * @param name 激光传感器名字
   * @param port 激光传感器端口
   */
    Distance(const std::string &name, const int &port, const QLength offset);
    /**
   * @brief 激光传感器构造函数
   * 
   * @param name 激光传感器名字
   * @param port 激光传感器JSON配置文件
   */

    Distance(const std::string &name, const json &pragma);
    /**
     * @brief 以获取当前到传感器的测量距离
     * 
     * @return QLength 可以转换成毫米 厘米 米等.
     */
    virtual QLength getLength();
    /**
     * @brief 以获取当前到传感器的测量距离加上偏移量 一般都用这个
     * 
     * @return QLength 可以转换成毫米 厘米 米等.
     */
    QLength getLengthOffset();
    /**
     * @brief 获取激光传感器名字
     * 
     * @return std::string 返回激光传感器名字
     */
    std::string getName();
    /**
     * @brief 获得对距离读数的信心
     * 
     * @return std::int32_t 该值的范围是0到100。100表示高置信度，较低的值表示信心不足。
     */
    std::int32_t getConfidence();
    /**
     * @brief 把激光传感器数据打印到流
     * 
     * @param ostr 要打印的流的引用
     */
    void showDistanceInfo(std::ostringstream &ostr);

  private:
    const std::string _name;
    QLength _offset = 0_mm;
};
} // namespace ncrapi