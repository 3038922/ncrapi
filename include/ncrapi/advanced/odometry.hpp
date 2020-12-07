#pragma once
#include "ncrapi/advanced/filter/kalman.hpp"
#include "ncrapi/device/adi.hpp"
#include "ncrapi/device/imu.hpp"
#include "ncrapi/system/json.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/units/QAngle.hpp"
#include "ncrapi/units/QLength.hpp"
#include "ncrapi/units/QMass.hpp"

namespace ncrapi {
enum {
    X = 0,
    Y,
    THETA
} ODOMSTATE;
enum {
    LENGTH = 0,    //机器人长度
    WIDTH,         //机器人宽度
    WEIGHT,        //机器人重量
    WHEELDIAMETER, //车轮直径
    WHEELSPACING,  //轮间距
    SITESIZE,      //赛场边长
} ODOMINFO;
class Odometer
{
  public:
    static Odometer *initOdometer(const json &param);
    static Odometer *getOdometer();
    /**
 * @brief  计算从给定的测距状态到给定点的距离和角度。
 * 
 * @param ix x点坐标
 * @param iy Y点坐标
 * @return std::pair<QLength, QAngle> first 计算后所得行驶距离 计算后所需转向的角度
 */
    std::pair<QLength, QAngle> computeDistanceAndAngleToPoint(const QLength ix, const QLength iy);
    /**
     * @brief 获取机器人长度
     * 
     * @return QLength 
     */
    QLength getLength();
    /**
     * @brief 获取机器人宽度
     * 
     * @return QLength 
     */
    QLength getWidth();
    /**
     * @brief 获取机器人重量
     * 
     * @return QMass 
     */
    QMass getWeight();
    /**
     * @brief 获取机器人轮子直径
     * 
     * @return QLength 
     */
    QLength getWheelDiameter();
    /**
     * @brief 获取机器人轮间距
     * 
     * @return QLength 
     */
    QLength getWheelSpacing();
    /**
     * @brief 获取车轮直径中间参数
     * 
     * @return double 
     */
    QLength getScale();
    /**
     * @brief 获取编码器测量角度中间参数
     * 
     * @return double 
     */
    double getTurnScale();
    /**
     * @brief 获取里程计X Y坐标最小值
     * 
     * @return QLength 
     */
    QLength getOdomMin();
    /**
     * @brief 获取里程计X Y坐标最大值
     * 
     * @return QLength 
     */
    QLength getOdomMax();
    /**
     * @brief 获取当前里程计的所有坐标
     * 
     * @return std::tuple<QLength, QLength, QAngle> 0是X轴坐标 1是Y轴坐标 2是角度(弧度)
     */
    std::tuple<QLength, QLength, QAngle> getOdomState();
    /**
     * @brief 获取X轴坐标
     * 
     * @return QLength 返回X轴坐标
     */
    QLength &getXState();
    /**
     * @brief 获取Y轴坐标
     * 
     * @return QLength 返回Y轴坐标
     */
    QLength &getYState();
    /**
     * @brief 获取当前角度
     * 
     * @return QAngle 返回THETA
     */

    QAngle &getThetaState();
    /**
     * @brief 设置里程计坐标
     *
     * @param theta 机器人当前角度
     */
    void setOdomState(QAngle theta);
    /**
     * @brief 设置里程计坐标
     * 
     * @param x X轴坐标
     * @param y Y轴坐标
     */
    void setOdomState(QLength x, QLength y);
    /**
     * @brief 设置里程计坐标
     * 
     * @param x X轴坐标
     * @param y Y轴坐标
     * @param theta 机器人当前角度
     */
    void setOdomState(QLength x, QLength y, QAngle theta);
    /**
     * @brief 设置里程计X轴坐标
     * 
     * @param x X轴坐标
     */
    void setXState(QLength x);
    /**
     * @brief 设置里程计Y轴坐标
     * 
     * @param y Y轴坐标
     */
    void setYState(QLength y);
    /**
     * @brief 设置里程计THETA角度
     * 
     * @param theta  THETA角度
     */
    void setThetaState(QAngle theta);
    /**
     * @brief 重置里程计 所有数据归0
     * 
     */
    void resetOdomState();
    // QAngle gyroCalc(QAngle encAngle, QAngle &gyroDriftAvg);
    // double getGyroBalance();
    // QAngle getGyroRealNow();
    /**
     * @brief 获取IMU绕z轴旋转的度数总数。
     * 
     * @return QAngle 返回自初始化后IMU的总度数
     */
    QAngle getRotation();
    /**
     * @brief 获取惯性传感器相对于其x轴初始方向的航向。
     *        此值的范围是（-360,360）。顺时针旋转用正度值表示，而逆时针旋转用负度值表示。
     * @return QAngle 返回 （-360,360）
     */
    QAngle getHeading();

  private:
    static Odometer *_odoMeter; //单例对象
    Odometer(const QLength length, const QLength width, const QMass weight,
             const QLength wheelDiameter, const QLength wheelSpacing, const QLength siteSize, const double ticksPerRev = 360.0);
    Odometer(const json &param);

    std::tuple<QLength, QLength, QAngle> _odomState = {0_mm, 0_mm, 0_deg}; //0 x 1 y 2 theta

    //0 机器人长度 1 机器人宽度 2 机器人重量 3 车轮直径 4轮间距 5 赛场边长
    std::tuple<QLength, QLength, QMass, QLength, QLength> _odomInfo = {0_mm, 0_mm, 0_kg, 0_mm, 0_mm};
    QLength odomMin = 0_mm;
    QLength odomMax = 0_mm;
    QLength _scale = 0.0_mm;             //车轮直径中间参数 单位mm
    double _turnScale = 0.0;             //编码器测量角度中间参数 单位mm
                                         //陀螺仪相关
                                         // std::shared_ptr<ADIAnalogIn> _gyro = nullptr;                           //陀螺仪
                                         // double _gyroMultiplier = 1.0, _gyroBalance = 1.0, _gyroBenchmark = 0.0; //陀螺仪平衡 陀螺仪乘数系数
                                         // QAngle _gyroRealNow = 0_deg;                                            //陀螺仪当前数据
                                         // KalmanFilter<double> _gyroFilter;                                       //陀螺仪滤波
                                         // bool _isGyroReady = true;
                                         //IMU相关
    std::shared_ptr<Imu> _imu = nullptr; //惯性传感器
};
#define odoMeter Odometer::getOdometer()
} // namespace ncrapi
