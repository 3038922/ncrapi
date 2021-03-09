#pragma once
#include "ncrapi/system/json.hpp"
#include "pros/rotation.hpp"
namespace ncrapi {
// V5旋转传感器可测量轴的旋转位置，总旋转和转速。旋转位置是从0°到360°测量的，精度为0.088。
// 该角度是绝对确定的，并且在关闭机器人电源时不会丢失。零位可以在仪表板上设置或通过代码设置。旋转数是前进或后退的转数，可以根据需要将其重置为零。
// 旋转不存储，并在每次执行程序时以当前角度重新开始。轴速度以每秒度数为单位。
// 该传感器与1/8”和1/4”标准VEX轴兼容。
class Rotation : public pros::Rotation
{
  public:
    /**
   * @brief 编码器构造函数
   * 
   * @param name 编码器名字
   * @param port 编码器端口
   */
    Rotation(const std::string &name, const int &port);
    /**
   * @brief 编码器构造函数
   * 
   * @param name 编码器名字
   * @param json 编码器JSON配置
   */
    Rotation(const std::string &name, const json &pragma);
    /**
     * @brief 获取编码器名字
     * 
     * @return std::string 返回编码器名字
     */

    std::string getName();

  private:
    const std::string _name;
};
} // namespace ncrapi