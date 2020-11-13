#pragma once

#include "ncrapi/system/json.hpp"
#include "pros/imu.hpp"

namespace ncrapi {
class Imu : public pros::Imu
{
  public:
    Imu(const std::string &name, const int &port);
    Imu(const std::string &name, const json &pragma);
    /**
     * @brief 获取自上次reset()以来的角度
     * 
     * @return double 返回旋转角度的累加 +-无穷大
     */
    virtual double getRotation();
    /**
     * @brief 获取自上次reset()以来的角度的相对角度 左转+ 右转-
     * 
     * @return double 返回旋转角度的累加 区间[-360,+360]
     */

    virtual double getHeading();
    /**
     * @brief 重置传感器 注意会导致机器悬空等待2.5秒
     * 
     * @return std::int32_t 如果操作成功，则返回PROS_ERR *失败，设置errno。
     */
    virtual std::int32_t reset() const override;

    std::string getName();

  private:
    const std::string _name;
};
} // namespace ncrapi