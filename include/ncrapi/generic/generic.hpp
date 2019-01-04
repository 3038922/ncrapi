#pragma once
#include "../userDisplay/userDisplay.hpp"
#include "api.h"
#include "ncrapi/system/systemData.hpp"
#include <memory>
#include <vector>

namespace ncrapi
{
/**
 * 机器人功能基础类 开环控制
 * @param a    马达对象
 * @param hold 悬停值
 */

class Generic : public Obj
{

  public:
    explicit Generic(const std::vector<pros::Motor> &motorList, const std::string name, const int hold = 0) : _motorList(motorList), _name(name), _holdVal(hold)
    {
        _nums = _motorList.size();
        if (_nums == 0)
            std::cerr << "chassis side nums error" << std::endl;
        pros::delay(100);
        resetEnc();
        size_t temp = _motorList.begin()->get_gearing();
        if (temp == 0)
            _gearing = 100;
        else if (temp == 1)
            _gearing = 200;
        else if (temp == 2)
            _gearing = 600;
        else
            std::cerr << "get gearing error" << std::endl;
        sysData->addObj(this);
    }
    /**
     * 初始化函数
     * @param lab 进度条 
     * @param str 文字
     * @param pwm 占空比
     */
    void init(lv_obj_t *lab, const char *str, const int pwm)
    {
        lv_label_set_text(lab, str);
        for (auto &it : _motorList)
            it.move(pwm);
        pros::delay(500);
        while (getSpeed() != 0)
            pros::delay(20);
        for (auto &it : _motorList)
            it.move(0);
        resetEnc();
    }
    /**
     *获取占空比 
     * @return int +-127; 
     */
    int getPwm()
    {
        return _pwm;
    }
    virtual void set(const int pwm)
    {
        _pwm = pwm;
        bool temp = isSafeMode();
        if (temp)
            _pwm = _holdVal * _holdingFlag;
        for (auto &it : _motorList)
            it.move(_pwm);
    }
    virtual void stop()
    {
        for (auto &it : _motorList)
            it.move(0);
    }
    /**
    *设置要移动到的电机的目标绝对位置。
    *
    *此运动相对于初始化时电机的位置或最近使用pros :: Motor :: set_zero_position（）复位的位置。
    *\注意此功能只是设置电机的目标，它不会阻止程序执行直到运动结束。
   * @param position 要移动的位置 注意电机一圈360
   * @param velocity 速度 红齿轮上限100 绿齿轮200 蓝齿轮600 
   */
    virtual void moveAbsolute(const double position, const std::int32_t velocity)
    {
        setMode(2);
        for (auto &it : _motorList)
            it.move_absolute(position, velocity);
    }

    /**
    *设置要移动到的电机的相对目标位置。
    *
    *此运动与pros :: Motor :: motor_get_position（）中给出的电机当前位置有关。提供10.0作为位置参数将导致电机顺时针移动10个单位，无论当前位置如何。
    *
    *\注意此功能只是设置电机的目标，它不会阻止
    *程序执行直到运动结束。
   * @param position 要移动的位置 注意电机一圈360
   * @param velocity 速度 红齿轮上限100 绿齿轮200 蓝齿轮600 
     */
    virtual void moveRelative(const double position, const std::int32_t velocity)
    {
        setMode(2);
        for (auto &it : _motorList)
            it.move_relative(position, velocity);
    }
    /**
    设定电机的速度。
    *
    *
    *当达到错误状态时，此函数使用以下errno值：
    *EACCES  -另一个资源目前正在尝试访问该端口。
    *
    *\ param velocity 新电机速度为 - +  - 100，+ -200或+ -600，具体取决于电机的齿轮组
    *
    *\如果操作成功则返回1或如果操作则返回PROS_ERR
    *失败，设置错误。
     */
    virtual void moveVelocity(const std::int32_t velocity)
    {
        setMode(2);
        for (auto &it : _motorList)
            it.move_velocity(velocity);
    }
    virtual void holding()
    {

        if (_mode == 1)
        {
            set(127);
            _holdingFlag = _mode;
        }
        else if (_mode == -1)
        {
            set(-127);
            _holdingFlag = _mode;
        }
        else if (_mode == 0)
            set(_holdVal * _holdingFlag);

        else
            ;
    }

    /**
     *设置模式   
     * @param flag 1:系统正转模式 0:马达悬停 -1:系统反转模式
     */
    virtual void setMode(const int flag)
    {
        _mode = flag;
    }
    /**
     * 获取当前模式状态      
     * @return 1是系统正转状态 0悬停状态 -1 是系统反转状态
     */
    virtual int getMode()
    {
        return _mode;
    }
    virtual void joyControl(pros::Controller *joy, pros::controller_digital_e_t up, pros::controller_digital_e_t down)
    {

        if (joy->get_digital(up))
        {
            set(127);
            setMode(0); //打开悬停状态
            _holdingFlag = 1;
        }
        else if (joy->get_digital(down))
        {
            set(-127);
            setMode(0); //打开悬停状态
            _holdingFlag = -1;
        }
        else
            holding();
    }
    /**
    * 重置马达编码器
    */
    virtual void resetEnc()
    {
        for (auto &it : _motorList)
            it.tare_position();
    }
    /**
     * 获取编码器值
     * @return 弹射编码器的值
     */
    virtual double getEnc()
    {
        double temp = 0;
        for (auto &it : _motorList)
            temp += it.get_position();
        return temp / _nums;
    }
    /**
     * 获取速度
     * @return 返回速度
     */
    virtual double getSpeed()
    {
        double temp = 0;
        for (auto &it : _motorList)
            temp += it.get_actual_velocity();
        return temp / _nums;
    }
    /**
     * 获取电机瓦数
     * @return 返回电机瓦数
     */
    virtual double getPower()
    {
        double temp = 0;
        for (auto &it : _motorList)
            temp += it.get_power();
        return temp / _nums;
    }
    /**
     * 获取电机以mA为单位的电流。
     * @return 获取电机以mA为单位的电流。
     */
    virtual int32_t getCurrent()
    {
        int32_t temp = 0;
        for (auto &it : _motorList)
            temp += it.get_current_draw();
        return temp / _nums;
    }
    /**
     * 以毫伏为单位获取输送到电机的电压。
     * @return如果操作失败，则以mV或PROS_ERR_F为单位返回电机电压，设置errno。
     */
    virtual int32_t getVoltage()
    {
        int32_t temp = 0;
        for (auto &it : _motorList)
            temp += it.get_voltage();
        return temp / _nums;
    }
    /**
     * 获取电机效率
     * @return 以百分比形式返回电机效率
     */
    virtual double getEfficiency()
    {
        double temp = 0;
        for (auto &it : _motorList)
            temp += it.get_efficiency();
        return temp / _nums;
    }
    /**
     * 获取电机扭矩
     * @return 返回电机扭矩
     */
    virtual double getTorque()
    {
        double temp = 0;
        for (auto &it : _motorList)
            temp += it.get_torque();
        return temp / _nums;
    }
    /**
     * 获取电机平均温度
     * @return 返回电机平均温度
     */
    virtual double getTemperature()
    {
        double temp = 0;
        for (auto &it : _motorList)
            temp += it.get_temperature();
        return temp / _nums;
    }
    /**
     * 检测部件是否堵转
     * @return 返回false 没有堵转,返回true 堵转了
     */
    virtual bool isSafeMode()
    {
        if (fabs(getSpeed()) <= _gearing / 10 && abs(_pwm) > _holdVal)
        {
            _safeModeFlags++;
            if (_safeModeFlags > 10)
            {
                std::cerr << _name << ":isSafeMode" << std::endl;
                return true;
            }
            else
                return false;
        }
        else
        {
            _safeModeFlags = 0;
            return false;
        }
    }
    /**
     * 重置所有相关传感器
     */
    virtual void resetAllSensors() override
    {
        resetEnc();
    }

    /**
     * 设置马达制动模式 使用会导致马达端口烧掉
     * @param mode 马达制动的模式 
     */
    virtual void setBrakeMode(pros::motor_brake_mode_e_t mode)
    {
        for (auto &it : _motorList)
            it.set_brake_mode(mode);
    }
    /**
     * 显示传感器数据到屏幕 ostringstream ostr流
     */
    virtual void showSensor() override
    {
        userDisplay->ostr << _name << ":编码器:" << getEnc() << "温度:" << getTemperature()
                          << "电压:" << getVoltage() << "电流:" << getCurrent() << std::endl;
    }
    virtual const std::string showName() const override
    {
        return _name;
    }
    virtual void showDetailedInfo() override
    {
        userDisplay->ostr << _name << "pwm:" << _pwm << "\n"
                          << "编码器:" << getEnc() << "速度:" << getSpeed() << "\n"
                          << "温度:" << getTemperature() << "电压:" << getVoltage() << "电流:" << getCurrent() << std::endl;
    }

  protected:
    const std::vector<pros::Motor> _motorList;
    const std::string _name;
    const int _holdVal;
    float _holdingFlag = 0;
    int _pwm = 0;
    size_t _safeModeFlags = 0;
    int _state = 0; //-1降 0 悬停 1 升
    int32_t _isMotorReady = 0;
    uint8_t _motorCount = 0;
    int _mode = 0;    //1 系统正传 0悬停 -1 系统反转 2使用原厂PID控制
    size_t _gearing;  //齿轮最大速度
    size_t _nums = 0; //马达总数
};                    // namespace ncrapi
} // namespace ncrapi