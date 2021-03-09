#pragma once
#include "generic.hpp"
#include "ncrapi/advanced/opticalSystem.hpp"
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
    Shooter(const json &pragma, std::shared_ptr<Generic> roulette);
    /**
     * @brief 设置自动吸球模式
     * 
     * @param flag  true 打开  false 关闭
     */
    virtual void setAutoMoveBall(bool flag);
    /**
     * @brief 开始自动上膛
     * 
     * @param flag false关闭  true 打开
     */
    void setAutoLoader(bool flag);

    /**
  * @brief 遥控函数
  * 
  * @param joy 指向遥控器的指针
  * @param upL 
  * @param downL 
  * @param upR 
  * @param downR 
  * @param mode 是否打开自动吸吐
  * @param clean 清空内存
  * @param autoMove 自动上膛&&发射
  */
    virtual void joyControl(std::shared_ptr<pros::Controller> joy,
                            pros::controller_digital_e_t upL, pros::controller_digital_e_t downL,
                            pros::controller_digital_e_t upR, pros::controller_digital_e_t downR,
                            pros::controller_digital_e_t mode, pros::controller_digital_e_t clean,
                            pros::controller_digital_e_t autoMove);
    /**
     * @brief 联动式发射器自动吸球
    */
    virtual void autoMoveBall(const int pwm);
    /**
    * @brief 联动式发射器悬停  3强制拉屎 2 单转发射转 1 发射 0悬停(自动吸) -1 倒回 -2使用原厂PID控制
    */
    virtual void holding() override;

    /**
     * @brief 是否打开低位自动吸球
     * 
     * @return true 自动吸球打开
     * @return false 自动吸球模式关闭
     */
    virtual bool isAutoMoveBall();
    /**
     * @brief 自动赛用的换球函数
     * 
     * @param inBall 要吸几个 不吸填0
     * @param outBall 要发射几个 不发射填0
     * @param breakTime 该程序最大运行时间 单位  _ms
     * @param frTime 前后前后时间调整,不前后前后 默认0_ms
     */
    virtual void changeBall(const int inBall, const int outBall, const QTime breakTime, QTime frTime = 0_ms);

  protected:
    using Generic::joyControl;          //消除重载警告
    Timer _autoloadedTimer;             //自动上膛计时器
    bool _isAutoMoveBall = true;        //是否打开低位自动吸球模式
    bool _isAutoLoaded = false;         //自动上膛的标志
    int _autoLoadedStatus = 0;          //自动上膛标志 0 结束 1上膛 2发射
    int _autoMoveMotorPWM = 127;        //自动上球设置值
    std::shared_ptr<Generic> _roulette; //吸吐的指针

  private:
};

/************不联动发射器构造函数****************/
class DistributionShooter : public Shooter
{
  public:
    /**
    * @brief 不联动发射器构造函数
    * @param pragma JSON 参数
    */
    DistributionShooter(const json &pragma, std::shared_ptr<Generic> roulette);
    /**
    * @brief 3强制拉屎 2 单转发射转 1 发射 0悬停(自动吸) -1 倒回 -2使用原厂PID控制
    */
    virtual void holding() override;
    /**
     * @brief 自动赛用的换球函数
     * 
     * @param inBall 要吸几个 不吸填0
     * @param outBall 要发射几个 不发射填0
     * @param breakTime 该程序最大运行时间 单位  _ms
     * @param frTime 前后前后时间调整,不前后前后 默认0_ms
     */

    virtual void changeBall(const int inBall, const int outBall, const QTime breakTime, QTime frTime = 0_ms) override;

  protected:
    void autoMoveBall(const int pwm) override;
    int _maxRegionBallNums = 3; //吸吐发射区间存球数

  private:
};
} // namespace ncrapi
