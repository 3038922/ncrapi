#pragma once
#include "ncrapi/advanced/odometry.hpp"
#include "ncrapi/advanced/pid/posPid.hpp"
#include "ncrapi/advanced/pid/regulationPid.hpp"
#include "ncrapi/device/adi.hpp"
#include "ncrapi/device/motor.hpp"
#include "ncrapi/device/vision.hpp"
#include "ncrapi/system/circular_buffer.hpp"
#include "ncrapi/system/object.hpp"
#include "ncrapi/units/QAcceleration.hpp"
#include "ncrapi/units/QAngularSpeed.hpp"
#include "ncrapi/units/QSpeed.hpp"
#include "ncrapi/util/timer.hpp"
#include "pros/misc.hpp"

namespace ncrapi {
/**
 * 基础开环控制(无传感器马达类)
 * @param motorList 马达类别 注意加{}
 */
class Chassis : public Obj
{
  public:
    //饿汉模式单例实现.线程安全
    static Chassis *initChassis(const json &pragma);
    static Chassis *initChassis(const json &pragma, Odometer *odometry);
    //但获取单例指针
    static Chassis *getChassis();

    /**
     * @brief 开环控制底盘左右轮速度
     * @param left 左轮速度+-127 +前进 -后退
     * @param right  右轮速度+-127 +前进 -后退
     */
    void set(const int left, const int right);
    /**
     * @brief 开环控制底盘左右轮和中轮速度
     * @param left 左轮速度+-127 +前进 -后退
     * @param right  右轮速度+-127 +前进 -后退
     * @param middle 中轮速度+-127 +左 -右
     */
    void set(const int left, const int right, int middle);
    /**
     * @brief 开环控制麦轮底盘左右轮速度
     * @param LF  左前速度+-127 +前进 -后退
     * @param LR  左后速度+-127 +前进 -后退
     * @param RF  右前速度+-127 +前进 -后退
     * @param RR  右后速度+-127 +前进 -后退
     */
    void set(const int LF, const int lB, const int RF, const int RB);
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
     * @param pwm 左转+ 右转- 范围:+-127
     */
    void rotate(const int pwm);
    /**
     * 使用速度环控制底盘左转右转 左转为+ 右转为-
     * @param velocity 设定的速度 上限红齿轮+-100 绿齿轮+-200 蓝齿轮+-600
     */
    void rotateVelocity(const int32_t velocity);
    /**
     *使用电机内置闭环 让底盘左转或者右转 
     * @param pos 转动的位置 电机一圈360 自己换算 左转- 右转+
     * @param velocity 速度 红尺寸+100 绿齿轮最大速度+200 蓝齿轮+600 
     */
    void rotateRelative(const double pos, const std::int32_t velocity);
    /**
     * 平移  只限麦轮
     * @param pwm 左平移+ 右平移- 范围:+-127
     */
    void translation(const int pwm);

    /**
     * 底盘马达停转
     */
    void stop() override;
    /**
     * 矢量控制 开弧线
     * @param distPwm  前进的力度 前进+ 后退-
     * @param anglePwm 弧线的力度 左开+ 右开-
     * 例:    chassis.driveVector(127, 80);
     */
    void driveVector(const int distPwm, const int anglePwm);

    /**
     * 遥控模块 单摇杆双摇杆都用这个
     * @param verticalVal     前后通道
     * @param horizontalVal   左右通道
     * @param threshold 遥控器矫正阀值
     */
    void arcade(std::shared_ptr<pros::Controller> joy, pros::controller_analog_e_t verticalVal, pros::controller_analog_e_t horizontalVal);

    /**
     * 5马达底盘遥控模块 单摇杆双摇杆都用这个
     * @param verticalVal     前后通道
     * @param horizontalVal   左右通道
     * @param translationVal  平移通道
     */
    void h_arcade(std::shared_ptr<pros::Controller> joy, pros::controller_analog_e_t verticalVal, pros::controller_analog_e_t horizontalVal, pros::controller_analog_e_t translationVal);

    /**
     * 遥控模块 麦轮遥控
     * @param verticalVal     前后通道
     * @param horizontalVal   左右通道
     * @param translationVal  平移通道
     */
    void mecanum(std::shared_ptr<pros::Controller> joy, pros::controller_analog_e_t verticalVal, pros::controller_analog_e_t horizontalVal, pros::controller_analog_e_t translationVal);
    /**
     * 遥控模块 坦克遥控
     * @param left      左通道
     * @param right     右通道
     */
    void tank(std::shared_ptr<pros::Controller> joy, pros::controller_analog_e_t left, pros::controller_analog_e_t right);

    /**
     * 重置底盘所有马达编码器
     */
    void resetEnc();
    /**
      * @获取左边或者右边编码器值
      * @param side 0左边 1右边
      * @return QAngle 返回左边或者右边编码器值 一圈360°
      */
    double getEnc(const bool side);
    /**
     * 获取电机当前速度  
     * @param side  0左边 1右边
     * @return double 返回左边或者右边编码器值
     */
    double getSpeed(const bool side);
    /**
     * @brief 获取底盘当前速度 单位mm/s
     * 
     * @param side 0左边 1右边
     * @return double  返回机器人底盘速度
     */
    QSpeed getChassisSpeed(const bool side);
    /**
 * 获取左边或者右边的温度
 * @param side 左边0 右边1
 * @return const double 返回左边或者右边的温度
 */
    double getTemperature(const bool side) const;
    /**
 * 获取左边或者右边的电压
 * @param side 左边0 右边1
 * @return int32_t 返回左边或者右边的电压
 */
    int32_t getVoltage(const bool side) const;
    /**
 * 获取左边或者右边的电流
 * @param side 左边0 右边1
 * @return int32_t 返回左边或者右边的电流
 */
    int32_t getCurrent(const bool side) const;
    /**
 * 获取左边或者右边的扭矩
 * @param side 左边0 右边1
 * @return int32_t 返回左边或者右边的扭矩
 */
    double getTorque(const bool side) const;
    void setCurrentLimit(const int32_t limit);
    void setVelPid(const pros::motor_pid_full_s_t pid) const;

    /**
     * 设置马达制动模式 使用会导致马达端口烧掉
     * @param mode 马达制动的模式 
     */
    void setBrakeMode(pros::motor_brake_mode_e_t mode);
    /**
     * @brief 打印内置PID数据到控制台
     * 
     */
    void getVelPid();
    const std::string showName() const override;
    void setMode(const int mode) override;
    const std::vector<Motor> getMotorInfo() const override;

    bool isSafeMode() override;
    void isStopPid(const bool isStopPid);
    void showOdom(std::ostringstream &ostr, bool isColor = false);
    /************************************************************************************/
    /**
     *重置底盘上所有传感器 
    */
    void resetAllSensors() override;

    /**
     * 机器人速度控制FOR PID
     * @param itarget 目标速度 单位cm/s
     * @param isShowDebug 是否在屏幕打印PID实时数据
     * return 返回程序运行时间
    */
    std::pair<QTime, double> velPid(QSpeed leftSpd, QSpeed rightSPd,
                                    const bool isLeftShowDebug = false, const bool isRightShowDebug = false);
    /**
     * 机器人前后控制FOR PID
     * @param itarget 前后运动的距离
     * @param isShowDebug 是否在屏幕打印PID实时数据
     * return 返回程序运行时间 和 距离目标值的距离 单位MM
    */
    std::pair<QTime, double> forwardPid(QLength itarget, const bool isShowDebug = false);
    /**
    * 旋转PID 将机器人转动到适当的角度idegTarget。角度行程的单位通常是底盘两侧之间编码器刻度的差异。
    * @param idegTarget 左转为+ 右转为-
    * @param isShowDebug 是否在屏幕打印PID实时数据
    * return 返回程序运行时间 和 距离目标值的度数 单位°
    */

    std::pair<QTime, double> rotatePid(QAngle idegTarget, const bool isShowDebug = false);
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
     * @param theta 机器人当前角度
     */
    void setOdomState(const QAngle theta);
    /**
     * 手动设置当前机器人姿态
     * @param x     X点坐标
     * @param y     Y点坐标
     */
    void setOdomState(const QLength x, const QLength y);

    /**
     * 手动设置当前机器人姿态
     * @param x     X点坐标
     * @param y     Y点坐标
     * @param theta 机器人当前角度
     */
    void setOdomState(const QLength x, const QLength y, const QAngle theta);
    /**
     * 单独设置X点坐标
     * @param x X点坐标
     */
    void setOdomX(const QLength x);
    /**
     * 单独设置Y点坐标
     * @param y Y点坐标
     */
    void setOdomY(const QLength y);
    /**
     * @brief 返回里程计X值
     * 
     * @return const QLength  返回当前X值
     */
    const QLength getXState();
    /**
     * @brief 返回里程计Y值
     * 
     * @return const QLength 返回当前Y值
     */
    const QLength getYState();
    /**
     * @brief 获取里程计THETA值
     * 
     * @return const QAngle 返回当前THETA值
     */
    const QAngle getThetaState();
    /**
     * @brief 获取里程计X Y THETA的引用
     * 
     * @return Coordinate& 返回里程计三个值的引用
     */
    Coordinate &getOdomState();
    /**
     * @brief 获取机器人的理论最大速度
     * 
     * @return const QSpeed 返回速度
     */
    const QSpeed getTheoreticalMaxSpeed() { return _theoreticalMaxSpeed; }
    /**
     * 显示传感器数据到屏幕 ostringstream ostr流
     */
    void showSensor(std::ostringstream &ostr) override;
    void showDetailedInfo(std::ostringstream &ostr) override;
    /**
     *  独立线程的机器人姿态测算循环
     */
    static void odomLoop(void *param);
    /************************************************************************************/
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
    /*****************************************************************************/

  private:
    Chassis(const json &pragma);
    Chassis(const json &pragma, Odometer *odometry);
    static Chassis *_chassis; // 单例对象
    const std::string _name;
    std::vector<Motor> _motorList;
    int _joyThreshold = 10;
    int _minFrPWM = 0, _maxFrPWM = 127;         //最小最大前后速度
    int _minRotatePWM = 0, _maxRotatePWM = 127; //最小最大旋转速度
    size_t _sideNums = 0;                       //半边马达数量
    int _pwm[2];                                //0 左边pwm 1 右边pwm
    size_t _gearing;
    circular_buffer<double, 3> _encVal[2];                                                    //编码器值                                                    //齿轮最大速度                                                        // 编码器值
    double _gearRatio = 1.0;                                                                  //底盘齿轮比
    QSpeed _theoreticalMaxSpeed = 0.0_cmps;                                                   //理论最大速度
    QSpeed _chassisSpeed[2] = {0_mps, 0_mps}, _maxChassisSpeed[2] = {0_mps, 0_mps};           //记录底盘左右轮子当前速度和最大速度 单位mm/s
    QAcceleration _nowAccelSpeed[2] = {0_mps2, 0_mps2}, _maxAccelSpeed[2] = {0_mps2, 0_mps2}; //最大加速度 单位mm/s2
    int _robotState = 0;                                                                      //机器人状态 0停止 1 前后 2 后退 3 左转 4 右转
    bool _isSafeMode = false;

    //PID相关
    std::shared_ptr<PosPid> _distancePid = nullptr;        //距离
    std::shared_ptr<PosPid> _anglePid = nullptr;           //旋转pid
    std::shared_ptr<RegulationPid> _velPid[2] = {nullptr}; //速度环 0左 1右

    //里程计
    Odometer *_odoMeter = nullptr;
    QAngle _encAngle = 0_deg;
    circular_buffer<QAngle, 3> _imuRotation, _imuHeading;    //里程计
    circular_buffer<QAngularSpeed, 3> _nowAngleSpeed[2];     //0编码测的的旋转速度 1 IMU测的的旋转速度
    QAngularSpeed _maxAngleSpeed[2] = {0_rps, 0_rps};        //0编码测的的旋转速度 1 IMU测的的旋转速度
    double _nowAccelAngle[2] = {0}, _maxAccelAngle[2] = {0}; //0编码测的的旋转加速度 1 IMU测的的旋转加速度

    //自瞄相关
    bool _isResetPid = true;
    double _shootPosVal = 1.0; //左右自瞄比例
    std::shared_ptr<PosPid> _autoAimingPid = nullptr;
    bool _isStopPid = false; //false:开启PID true:关闭PID
};
#define chassis Chassis::getChassis()
} // namespace ncrapi
