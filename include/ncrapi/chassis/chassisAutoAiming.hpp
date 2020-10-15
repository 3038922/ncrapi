#pragma once
#include "chassisOdom.hpp"

namespace ncrapi {
class ChassisAutoAiming : public ChassisOdom
{
  public:
    ChassisAutoAiming(const json &pragma, const std::array<int, 128> *frspeed = nullptr, const std::array<int, 128> *routerSpeed = nullptr, const std::array<int, 128> *translationSpeed = nullptr);
    /**
     * @brief 带自瞄的旋转.
     * @param idegTarget 左转为+ 右转为-
     */
    std::pair<QTime, double> rotateAutoAiming(QAngle idegTarget);
    /**
     * @brief 遥控用的自瞄模块  
     * 
     */
    void autoAimingCalc();
    /**
     * @brief 遥控模块
     * 
     * @param joy 遥控器的指针
     * @param verticalVal 前后通道的按钮
     * @param horizontalVal 左右通道的按钮
     * @param autoAimingBTN 自瞄的按钮
     */
    void arcade(std::shared_ptr<pros::Controller> joy, pros::controller_analog_e_t verticalVal, pros::controller_analog_e_t horizontalVal, pros::controller_digital_e_t autoAimingBTN);
    /**
     * 重新设定前后PID参数

     */
    void resetPid(std::string_view name);
    /**
     * @brief  转到相对于场地的角度 快到的时候会有自瞄介入矫正
     * 
     * @param iangle 你要转到的角度
     */
    void driveToAngleAutoAiming(const QAngle iangle);

  private:
    bool _isResetPid = true;
    double _shootPosVal = 1.0; //左右自瞄比例
    Pid _autoAimingPid;
};
} // namespace ncrapi