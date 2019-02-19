#include "ncrapi/generic/generic.hpp"
#include "ncrapi/system/sysBase.hpp"
#include <algorithm>
#include <cmath>
namespace ncrapi
{
/**
 * 机器人功能基础类 开环控制
 * @param a    马达对象
 * @param hold 悬停值
 */

Generic::Generic(const std::string &name, const json &pragma) : _name(name)
{

    for (auto &[key, value] : pragma["马达"].items())
        _motorList.push_back({key, value});
    _holdVal = clamp<int>(pragma["参数"]["悬停值"].get<int>(), 0, 20, "悬停值");
    pros::delay(100);
    _nums = _motorList.size();
    if (_nums == 0)
        sysData->addDebugData({"部件类马达数量不能为0"});
    _gearing = _motorList.begin()->getGearSpeed();
    sysData->addObj(this);
    resetEnc();
    std::cout << "部件 " << _name << "构造成功" << std::endl;
}
/**
     * 初始化函数
     * @param lab 进度条 
     * @param str 文字
     * @param pwm 占空比
     */
void Generic::init(lv_obj_t *lab, const char *str, const int pwm)
{
    lv_label_set_text(lab, str);
    for (auto &it : _motorList)
        it.move(pwm);
    pros::delay(500);
    while ((_encNow != _encLast) && _encLast != 0)
        pros::delay(20);
    for (auto &it : _motorList)
        it.move(0);
    resetEnc();
    _isInit = true;
}

/**
     * 普通的占空比控制  开环
     * @param pwm 占空比+-127
     */
void Generic::set(const int pwm)
{
    _encLast = _encNow;
    _openLoopVal = std::clamp(pwm, -127, 127);
    if (isSafeMode())
        _openLoopVal = _holdVal * _holdingFlag;
    for (auto &it : _motorList)
        it.move(_openLoopVal);
    if (getTemperature() >= 50 && _timerTemp.getDtFromMark() >= 15000)
    {
        sysData->addDebugData({_name, ":马达过热!"});
        _timerTemp.placeMark();
    }
    _encNow = getEnc();
}
/**
     * 普通的电压控制 开环     
     * @param vol +-120
     */
void Generic::moveVoltage(const double vol)
{
    _openLoopVal = static_cast<int>(vol);
    if (isSafeMode())
        _openLoopVal = _holdVal * _holdingFlag;
    for (auto &it : _motorList)
        it.move_voltage(static_cast<int32_t>(_openLoopVal * 100));
}
void Generic::stop()
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
void Generic::moveAbsolute(const double position, const std::int32_t velocity)
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
void Generic::moveRelative(const double position, const std::int32_t velocity)
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
void Generic::moveVelocity(const std::int32_t velocity)
{
    setMode(2);
    for (auto &it : _motorList)
        it.move_velocity(velocity);
}
void Generic::holding()
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
void Generic::setMode(const int flag)
{
    _mode = flag;
}
/**
     * 获取当前模式状态      
     * @return 1是系统正转状态 0悬停状态 -1 是系统反转状态
     */
int Generic::getMode()
{
    return _mode;
}

void Generic::joyControl(std::shared_ptr<pros::Controller> joy, pros::controller_digital_e_t up, pros::controller_digital_e_t down)
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
void Generic::resetEnc()
{
    for (auto &it : _motorList)
        it.tare_position();
}
/**
 * @brief 设置编码器值
 * 
 * @param pos 要设置的值
 */
void Generic::setEnc(const double pos)
{
    for (auto &it : _motorList)
        it.set_zero_position(pos);
}
int Generic::getOpenLoopVal()
{
    return _openLoopVal;
}

/**
     * 获取编码器值
     * @return 弹射编码器的值
     */
double Generic::getEnc()
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
double Generic::getSpeed()
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
double Generic::getPower()
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
int32_t Generic::getCurrent()
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
int32_t Generic::getVoltage()
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
double Generic::getEfficiency()
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
double Generic::getTorque()
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
double Generic::getTemperature()
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
bool Generic::isSafeMode()
{
    if (fabs(getSpeed()) <= _gearing / 10 && abs(_openLoopVal) > _holdVal)
    {
        _safeModeFlags++;
        if (_safeModeFlags > 10)
        {
            sysData->addDebugData({_name, ":进入热保模式!请注意操作"});
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
void Generic::resetAllSensors()
{
    resetEnc();
}

/**
     * 设置马达制动模式 使用会导致马达端口烧掉
     * @param mode 马达制动的模式 
     */
void Generic::setBrakeMode(pros::motor_brake_mode_e_t mode)
{
    std::cout << "设置" << _name << "马达制动模式" << mode << std::endl;
    for (auto &it : _motorList)
        it.set_brake_mode(mode);
}
/**
     * 显示传感器数据到屏幕 ostringstream ostr流
     */
void Generic::showSensor()
{
    userDisplay->ostr << _name << ":编码器:" << getEnc() << "温度:" << getTemperature()
                      << "电压:" << getVoltage() << "电流:" << getCurrent() << std::endl;
}
const std::string Generic::showName() const
{
    return _name;
}
void Generic::showDetailedInfo()
{
    userDisplay->ostr << _name << "pwm:" << _openLoopVal << "\n"
                      << "编码器:" << getEnc() << "速度:" << getSpeed() << "\n"
                      << "温度:" << getTemperature() << "电压:" << getVoltage() << "电流:" << getCurrent() << std::endl;
}

} // namespace ncrapi