#pragma once
#include "ncrapi/system/json.hpp"
#include "pros/optical.hpp"
namespace ncrapi {
// V5光学传感器是环境光，颜色，接近度和手势传感器的组合。颜色信息包括RGB，色相和饱和度或灰度。
// 当物体靠近100mm时，色彩检测效果最佳。接近传感器测量反射光强度，因此，该值将随环境光和物体反射率而变化。
// 手势传感器可以检测四种可能的手势之一，即在传感器上方向上，向下，向左或向右移动的对象（手或其他）。
// 光学传感器具有一个白色LED，可帮助在弱光条件下进行颜色检测。
class Optical : public pros::Optical
{
  public:
    /**
   * @brief 光学传感器构造函数
   * 
   * @param name 光学传感器名字
   * @param port 端口
   */
    Optical(const std::string &name, const int &port);
    /**
     * @brief 光学传感器构造函数
     * 
     * @param name 光学传感器名字
     * @param pragma 光学传感器JSON配置
     */
    Optical(const std::string &name, const json &pragma);
    /**
     * @brief 获取光学传感器名字
     * 
     * @return std::string 返回光学传感器名字
     */

    std::string getName();
    /**
     * @brief 打印自己需求的调试数据到控制台
     * 
     */
    void printDebug();
    /**
     * @brief 打印滤波后的颜色数据
     * 
     */
    void printRGB();
    /**
     * @brief 打印原始的颜色数据
     * 
     */
    void printRAW();
    /**
     * @brief 打印光学传感器信息到流
     * 
     * @param ostr 要打印的流的引用.
     */
    void showOpticalInfo(std::ostringstream &ostr);

  private:
    const std::string _name;
};
} // namespace ncrapi