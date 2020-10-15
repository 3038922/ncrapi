#pragma once
#include "chassis.hpp"
#include "ncrapi/pid/velPid.hpp"
#include "ncrapi/units/QAngularSpeed.hpp"
#include "ncrapi/util/odometry.hpp"

namespace ncrapi {
// namespace ncrapi
class ChassisOdom : public Chassis
{
  public:
    ChassisOdom(const json &pragma, const std::array<int, 128> *frspeed = nullptr, const std::array<int, 128> *routerSpeed = nullptr, const std::array<int, 128> *translationSpeed = nullptr);

    /**
     * 重置底盘所有红色编码器
     */
    void resetEnc() override;

    /**
 * @获取左边或者右边编红色码器值
 * @param side 0左边 1右边
 * @return double 返回左边或者右边编码器值
 */
    virtual double getEnc(const bool side) override;

    /**
     *重置底盘上所有传感器 
    */
    virtual void resetAllSensors() override;

    /**
     * 机器人速度控制FOR PID
     * @param itarget 目标速度
     * return 返回程序运行时间
    */
    std::pair<QTime, double> speedPid(QSpeed leftSpd, QSpeed rightSPd);
    /**
     * 机器人前后控制FOR PID
     * @param itarget 前后运动的距离
     * return 返回程序运行时间
    */
    std::pair<QTime, double> forwardPid(QLength itarget);
    /**
    * 旋转PID 将机器人转动到适当的角度idegTarget。角度行程的单位通常是底盘两侧之间编码器刻度的差异。
    * @param idegTarget 左转为+ 右转为-
      * return 返回程序运行时间
    */

    std::pair<QTime, double> rotatePid(QAngle idegTarget);
    /**
         * 开车到现场框架中的点（ix，iy）。如果需要，机器人将首先转向目标点。
         * @param ix 目的地的X坐标
         * @param iy 目的地的Y坐标
         * @param ibackwrds 是否倒着开回来
         * @param ioffset 离目的地有多远停止
         */
    void driveToPoint(QLength ix, QLength iy, const bool ibackwards = false, const QLength ioffset = 0_mm);

    /**
 * 转到iangle场框中的角度
 * @param 转向的角度 相对于XY坐标系内
 */
    void driveToAngle(const QAngle iangle);

    /**
     * 手动设置当前机器人姿态
     * @param x     X点坐标
     * @param y     Y点坐标
     * @param theta 机器人当前角度
     */
    void setState(const QLength x, const QLength y, const QAngle theta);
    /**
     * 单独设置X点坐标
     * @param x X点坐标
     */
    void setX(const QLength x);
    /**
     * 单独设置Y点坐标
     * @param y Y点坐标
     */
    void setY(const QLength y);
    /**
     * 单独设置机器人当前角度坐标
     * @param theta 机器人相对场地的角度
     */
    void setTheta(const QAngle theta);

    /**
    * 返回机器人的当前姿态
    * @return 返回姿态类型
    */
    const OdomState &getState();
    /**
     * 显示传感器数据到屏幕 ostringstream ostr流
     */
    virtual void showSensor() override;
    virtual void showDetailedInfo() override;
    virtual void showOdom(bool isColor = false);
    /**
 * @重置PID
 * @pragma name 名字
 */
    virtual void resetPid(std::string_view name);
    /**
     * @brief 是否强制跳出PID 循环
     * 
     * @param isStopPid  false 不跳出循环 true 跳出循环
     */
    virtual void isStopPid(const bool isStopPid);
    /**
     *  独立线程的机器人姿态测算循环
     */
    void odomLoop();

  protected:
    /**
     * @brief 读取陀螺仪原始值
     * 
     * @return double 返回陀螺仪原始值
     */
    QAngle getGyro();
    bool _isGyroReady = true;
    std::vector<ADIEncoder> _redEncList; //红盒子 0左 1 右
    KalmanFilter<double> _gyroFilter;
    const int _moveThreshold = 50;                //最小移动长度
    double _multiplier = 1.0, _gyroBalance = 1.0; //乘数
    std::vector<ADIAnalogIn> _gyroList;
    int _gyroBenchmark[2] = {0};
    VelPid _speedPid[2]; //速度环 0左 1右
    Pid _distancePid;    //距离
    Pid _anglePid;       //转弯
                         //KalmanFuse filterAngle; //融合滤波
                         //ODOMERTY
    OdomState _state;
    QAngle _filterAngle = 0_deg, _gyroNow[2] = {0_deg, 0_deg}, _encAngle = 0_deg, _gyroDriftAvg = 0_deg; //陀螺仪飘零平均值 10MS
    QAngularSpeed _nowAngleSpeed[2] = {0_rpm, 0_rpm}, _maxAngleSpeed[2] = {0_rpm, 0_rpm};                //0编码测的的旋转速度 1 陀螺仪测的的旋转速度
    double _nowAccelAngle[2] = {0}, _maxAccelAngle[2] = {0};                                             //0编码测的的旋转加速度 1 陀螺仪测的的旋转加速度

  private:
    bool _isRedEndMode = false; //是否红盒子模式
    size_t _gyroNums = 0;
    bool _isStopPid = 0;
}; // namespace ncrapi
} // namespace ncrapi
