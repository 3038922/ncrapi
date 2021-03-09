#pragma once
#include "ncrapi/device/optical.hpp"
namespace ncrapi {

class OpticalGroup
{
  public:
    OpticalGroup(const std::string name, const std::vector<Optical> &opticalList);
    OpticalGroup(const std::string name, const json &pragma);
    /**
     * @brief 设置所有光学传感器亮度
     * 
     * @param pwm 亮度 0-100
     */
    void setLedPwm(const int pwm);
    /**
     * @brief 设置所有光学传感器亮度
     * 
     * @param nums 传感器编号
     * @param pwm pwm 亮度 0-100
     */
    void setLedPwm(const int nums, const int pwm);
    /**
     * @brief 设置手势识别状态
     * 
     * @param flag false 关闭手势识别 true 打开手势识别
     */
    void setGestureMode(const bool flag = false);
    /**
     * @brief 设置手势识别状态
     * 
     * @param nums 传感器编号
     * @param flag false 关闭手势识别 true 打开手势识别
     */
    void setGestureMode(const int nums, const bool flag = false);
    /**
     * @brief 获取色相
     * 
     * @param nums 传感器编号
     * @return double 返回色相 0 - 359.999
     */
    virtual double getHue(const int nums);
    /**
     * @brief 获取饱和度
     * 
     * @param nums 传感器编号
     * @return double 返回饱和度 0-1.0
     */
    double getSaturation(const int nums);
    /**
     * @brief 获取亮度
     * 
     * @param nums 传感器编号
     * @return double  返回亮度 0 to 1.0
     */
    double getBrightness(const int nums);
    /**
     * @brief 返回距离
     * 
     * @param nums 传感器编号
     * @return std::int32_t  返回检测到的物体的距离 0 to 255.
     */
    std::int32_t getProximity(const int nums);
    /**
     * @brief 获取LED 亮度
     * 
     * @param nums 传感器编号
     * @return std::int32_t 返回LED亮度 0-100
     */
    std::int32_t getLedPwm(const int nums);
    /**
     * @brief 获取光学传感器个数
     * 
     * @return int 返回光学传感器个数
     */
    int getOpticalNums();
    /**
     * @brief 打印调式信息
     * 
     */
    void printDebug();
    /**
     * @brief 打印单个光学传感器调试信息
     * 
     * @param nums 传感器编号
     */
    void printDebug(const int nums);
    /**
     * @brief 把光学传感器信息传入到流里
     * 
     * @param ostr 要引用的流
     */

    void showOpticalInfo(std::ostringstream &ostr);
    /**
     * @brief  把单个光学传感器信息传入到流里
     * 
     * @param nums 传感器编号
     * @param ostr 要引用的流
     */
    void showOpticalInfo(const int nums, std::ostringstream &ostr);

  protected:
    std::vector<Optical> _opticalList;
    const std::string _name; //组名
};
} // namespace ncrapi