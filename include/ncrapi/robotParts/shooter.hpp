#pragma once
#include "generic.hpp"
#include "ncrapi/device/adi.hpp"

namespace ncrapi {

/************联动式发射器构造函数****************/
class Shooter : public Generic
{

  public:
    /**
    * @brief 联动式发射器构造函数 
    *  1 发射  0悬停 -1 倒回 -2使用原厂PID控制 
    * @param pragma JSON 参数
    */
    Shooter(const json &pragma);
    /**
     * @brief 联动式发射器遥控
     * 
     * @param joy 遥控器指针
     * @param up 吸
     * @param down 倒回
     * @param mode 切换到自动送球
     */

    virtual void joyControl(std::shared_ptr<pros::Controller> joy, pros::controller_digital_e_t up,
                            pros::controller_digital_e_t down, pros::controller_digital_e_t mode);
    /**
     * @brief 联动式发射器自动吸球
    */
    virtual void autoLowMoveBall();
    /**
    * @brief 联动式发射器悬停  1 发射  0悬停 -1 倒回 -2使用原厂PID控制 
    */
    virtual void holding() override;
    /**
     * @brief 设置自动吸球模式
     * 
     * @param flag  true 打开  false 关闭
     */
    virtual void setAutoLowMoveBall(bool flag);
    /**
     * @brief 是否打开低位自动吸球
     * 
     * @return true 自动吸球打开
     * @return false 自动吸球模式关闭
     */
    virtual bool isAutoLowMoveBall();
    /**
     * @brief 获取低位巡线传感器值
     * 
     * @return int 返回低位巡线值
     */
    virtual int getLowLineVal();
    /**
     * @brief 获取高位巡线传感器值
     * 
     * @return int 返回高位巡线值
     */
    virtual int getHighLineVal();

  protected:
    using Generic::joyControl; //消除重载警告

    /**
   * @brief 简约面板信息显示
   * 
   */
    virtual void showSensor() override;
    /**
   * @brief 详细面板信息显示
   * 
   */
    virtual void showDetailedInfo() override;
    ADILine _lineLow;     //底层巡线
    ADILine _lineHigh;    //高层巡线
    int _lineLowVal = 0;  //巡线低的阈值
    int _lineHighVal = 0; //巡线高的阈值
    Timer _joyTimer;
    bool _isAutoLowMoveBall = true; //是否打开低位自动吸球模式

  private:
};

/************分离式发射器构造函数****************/
class SeparateShooter : public Shooter
{

  public:
    /**
    * @brief 分离式发射器构造函数 
    *  3上送球系统 2下送球系统 1 双送球系统发射  0悬停 -1 倒回 -2使用原厂PID控制 
    * @param pragma JSON 参数
    */
    SeparateShooter(const json &pragma);

    /**
     * @brief 分离式发射的遥控
     * 
     * @param joy 遥控器指针
     * @param up 吸
     * @param down 倒回
     * @param mode 切换到自动送球
     */
    virtual void joyControl(std::shared_ptr<pros::Controller> joy, pros::controller_digital_e_t up,
                            pros::controller_digital_e_t down, pros::controller_digital_e_t mode) override;

    /**
     * @brief 分离式发射器自动吸球
    */
    virtual void autoLowMoveBall() override;

    /**
    * @brief 悬停函数 2下送球系统 1 双送球系统发射  0悬停 -1 倒回 -2使用原厂PID控制 
    */
    virtual void holding() override;

  private:
};

/************分球式发射器构造函数****************/
class DistributionShooter : public Shooter
{
  public:
    /**
    * @brief 分球式发射器构造函数 
    * 6吃屎 5拉屎 4上送球系统 3中送球系统 2下送球系统 1 双送球系统发射  0悬停 -1 倒回 -2使用原厂PID控制 
    * @param pragma JSON 参数
    */
    DistributionShooter(const json &pragma);
    /**
     * @brief 分离式发射的遥控
     * 
     * @param joy 遥控器指针
     * @param up 发射
     * @param down 倒回
     * @param mode 自动分球手动分球切换
     * @param distribution 手动分球
     */
    virtual void joyControl(std::shared_ptr<pros::Controller> joy,
                            pros::controller_digital_e_t up, pros::controller_digital_e_t down,
                            pros::controller_digital_e_t mode, pros::controller_digital_e_t distribution);
    /**
     * @brief 分球式发射器自动吸球
    */
    virtual void autoLowMoveBall() override;
    /**
    * @brief 悬停函数
    */
    virtual void holding() override;

  protected:
    using Shooter::joyControl; //消除重载警告
    int _middleMotorVal = 0;   //三马达第二层自动吸设置值

  private:
};
} // namespace ncrapi
