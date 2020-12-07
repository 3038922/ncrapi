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
    void setLedPwm(const int nums, const int pwm);
    void setGestureMode(const bool flag = false);
    void setGestureMode(const int nums, const bool flag = false);
    double getHue(const int nums);
    double getSaturation(const int nums);
    double getBrightness(const int nums);
    std::int32_t getProximity(const int nums);
    std::int32_t getLedPwm(const int nums);
    int getOpticalNums();
    void printDebug();
    void printDebug(const int nums);

    void showOpticalInfo(std::ostringstream &ostr);
    void showOpticalInfo(const int nums, std::ostringstream &ostr);

  protected:
    std::vector<Optical> _opticalList;
    const std::string _name; //组名
};
} // namespace ncrapi