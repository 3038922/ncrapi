#pragma once
#include "../userDisplay/userDisplay.hpp"
#include "api.h"
#include "ncrapi/system/systemData.hpp"

namespace ncrapi
{
constexpr int realSpeed[128] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72,
    73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96,
    97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
    121, 122, 123, 124, 125, 126, 127};

/**
 * 基础开环控制(无传感器马达类)
 * @param motorList 马达类别 注意加{}
 */
class Chassis : public Obj
{

  public:
    explicit Chassis(const std::vector<pros::Motor> &motorList);
    void set(const int left, const int right);

    /**
    设定电机的速度。
    *
    *
    *当达到错误状态时，此函数使用以下errno值：
    *EACCES  -另一个资源目前正在尝试访问该端口。
    *
    *\ param left right 新电机速度为 - +  - 100，+ -200或+ -600，具体取决于电机的齿轮组
    *
    *\如果操作成功则返回1或如果操作则返回PROS_ERR
    *失败，设置错误。
     */
    void moveVelocity(const std::int32_t left, const std::int32_t right);

    /**
*设置要移动到的电机的相对目标位置。
*此运动与pros :: Motor :: motor_get_position（）中给出的电机当前位置有关。提供10.0作为位置参数将导致电机顺时针移动10个单位，无论当前位置如何。
*注意此功能只是设置电机的目标，它不会阻止程序执行直到运动结束。
*@ param leftPos rightPos 左右电机的要移动的距离 相对于上次resetEnc()
*@ param velocity 以RPM为单位的最大运动速度
*/
    void moveRelative(const double leftPos, const double rightPos, const std::int32_t velocity);

    /**
     * 普通前进后退 开环控制
     * @param pwm  前进+ 后退- 范围:+-127
     */
    void forward(const int pwm);
    /**
     * 使用速度环控制底盘前进后退
     * @param velocity 设定的速度 上限红齿轮+-100 绿齿轮+-200 蓝齿轮+-600
     */
    void forwardVelocity(const int32_t velocity);
    /**
     *使用电机内置闭环 让底盘前进或者后退        
     * @param pos 前进的距离 电机一圈360 自己换算
     * @param velocity 速度 红尺寸+100 绿齿轮最大速度+200 蓝齿轮+600 
     */
    void forwardRelative(const double pos, const std::int32_t velocity);
    /**
     * 普通旋转 开环控制
     * @param pwm 左转- 右转+ 范围:+-127
     */
    void rotate(const int pwm);
    /**
     * 使用速度环控制底盘左转右转 左转- 右转+ 
     * @param velocity 设定的速度 上限红齿轮+-100 绿齿轮+-200 蓝齿轮+-600
     */
    void rotateVelocity(const int32_t velocity);
    /**
     *使用电机内置闭环 让底盘左转或者右转 
     * @param pos 转动的位置 电机一圈360 自己换算 左转- 右转+
     * @param velocity 速度 红尺寸+100 绿齿轮最大速度+200 蓝齿轮+600 
     */
    void rotateReative(const double pos, const std::int32_t velocity);
    /**
     * 底盘马达停转
     */
    void stop() override;
    /**
     * 矢量控制 开弧线
     * @param distPwm  前进的力度 前进+ 后退-
     * @param anglePwm 弧线的力度 左开- 右开+
     * 例:    chassis.driveVector(127, 80);
     */
    void driveVector(const int distPwm, const int anglePwm, const int *speedMode = realSpeed); //开弧线
    /**
     * 遥控模块 单摇杆双摇杆都用这个
     * @param verticalVal     前后通道
     * @param horizontalVal    左右通道
     * @param threshold 遥控器矫正阀值
     */
    void arcade(pros::Controller *joy, pros::controller_analog_e_t verticalVal, pros::controller_analog_e_t horizontalVal, const int rotateMaxSpeed = 127, const int threshold = 10, const int *speedMode = realSpeed);

    void tank(pros::Controller *joy, pros::controller_analog_e_t left, pros::controller_analog_e_t right, const int threshold = 10);
    /**
     * 重置底盘所有马达编码器
     */
    void resetEnc();

    /**
     * 重置底盘相关传感器
     */
    virtual void resetAllSensors() override;
    /**
 * @获取左边或者右边编码器值
 * @param side 0左边 1右边
 * @return double 返回左边或者右边编码器值
 */
    virtual double getEnc(const bool side);
    /**
     * 获取电机当前速度  
     * @param side  0左边 1右边
     * @return double 返回左边或者右边编码器值
     */
    virtual double getSpeed(const bool side);

    /**
 * 获取左边或者右边的温度
 * @param side 左边0 右边1
 * @return const double 返回左边或者右边的温度
 */
    virtual const double getTemperature(const bool side) const;
    /**
 * 获取左边或者右边的电压
 * @param side 左边0 右边1
 * @return int32_t 返回左边或者右边的电压
 */
    virtual const int32_t getVoltage(const bool side) const;
    virtual const int32_t getCurrent(const bool side) const;

    /**
     * 显示传感器数据到屏幕 ostringstream ostr流
     */
    virtual void showSensor() override;
    virtual const std::string showName() const override;
    virtual void showDetailedInfo() override;

  protected:
    const std::string _name;
    const std::vector<pros::Motor> _motorList;
    size_t _sideNums = 0; //半边马达数量
    int _pwm[2];          //0 左边pwm 1 右边pwm
};
} // namespace ncrapi
