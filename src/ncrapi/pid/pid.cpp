/**
 * @Author: 陈昱安
 * @Date:   2018-06-05double12:14:46+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-09-05double15:21:17+08:00
 */
#include "ncrapi/pid/pid.hpp"
#include "ncrapi/system/sysUser.hpp"

namespace ncrapi {
//double 变量类型 N 可调参数
/**
     * PID controller
    *@param ikP比例增益
    *@param ikI积分增益
    *@param ikD衍生收益
    *@param ikBias控制器偏差
     */
Pid::Pid(const std::string &name, const json &pragma) : _name(name)
{
    //遍历录入pid
    std::string mode;
    if (sysData->jsonVal["底盘"]["参数"]["模式"] == 0)
    {
        mode = "模式0";
        _firstRunNums = 3;
    }
    else
    {
        mode = "模式1";
        _firstRunNums = 8;
    }
    for (auto &it : pragma[mode][name])
        for (auto &it1 : it)
            _pragma.push_back(it1.get<double>());
    _filter = std::make_unique<KalmanFilter<double>>(_pragma.at(5), _pragma.at(6));
    logger->info({_name, " 类构造成功"});
}
Pid::Pid(const std::string &name, const std::vector<double> &pragma) : _pragma(pragma)
{
    //遍历录入pid
    std::string mode;
    if (sysData->jsonVal["底盘"]["参数"]["模式"] == 0)
    {
        mode = "模式0";
        _firstRunNums = 3;
    }
    else
    {
        mode = "模式1";
        _firstRunNums = 8;
    }
    _filter = std::make_unique<KalmanFilter<double>>(_pragma.at(5), _pragma.at(6));
    logger->info({_name, " 类构造成功"});
}

/**
     * 前置抗积分抗饱和PID控制器 转弯用
     * @param  inewReading 传感器输入值
     * @return            电机PWM值
     */
double Pid::step(const double inewReading)
{
    if (_isOn)
    {
        _error = _target - inewReading;                                         //距离目标的差值 =期望的目标值-当前传感器的值
        double derivative = _filter->filter(_error - _lastError) * _pragma[KD]; ////这里可以加入滤波 让刹车更线性                           //误差值小于阀值的时候进行积分
        double kpsum = _pragma[KP] * _error;                                    //这个是方便我PRINdoubleF的
        if (fabs(_error) <= _pragma[LIMIT])                                     //如果误差小于距离限制 进入PID 那机器会有个抬头问题.
            _integral += _pragma[KI] * _error;
        if (_shouldResetOnCross && ncrapi::copySign(_error) != ncrapi::copySign(_lastError)) //如果距离还很远的时间积分值归零 这个可以自己按自己的工程随便写
        {
            _integral = 0;
            _filter->reset();
        }
        _lastError = _error;

        _integral = clamp(_integral, _integralMin, _integralMax);
        _output = clamp(kpsum + _integral + derivative + _pragma[BIAS], _outputMin, _outputMax);
        // if (_name != "左右pid")
        // logger->info({"targe:", std::to_string(_target), " error:", std::to_string(_error), " output:", std::to_string(static_cast<int>(_output)),
        //               " kp:", std::to_string(kpsum), " ki:", std::to_string(_integral), " kd:", std::to_string(derivative)});
        // // #pragma GCC diagnostic push
        // #pragma GCC diagnostic ignored "-Wno-psabi"
        if (sysData->test != 0 && fabs(_error) < fabs(_target) * 0.2)
            sysData->pidDebugData.push_back(std::make_tuple(_error, _output, kpsum, _integral, derivative));
        // #pragma GCC diagnostic pop
    }
    else
        _output = 0; //Controller is off so write 0
    if (_firstRun < _firstRunNums)
    {
        _lastError = _error;
        _firstRun++;
        return (_outputMax * copySign(_output));
    }
    else
        return _output;
}

double Pid::incrementStep(const double inewReading)
{
    if (_isOn)
    {
        _error = _target - inewReading;                                                          //距离目标的差值 =期望的目标值-当前传感器的值
        double derivative = _filter->filter(_error - 2 * _lastError + _prevError) * _pragma[KD]; ////这里可以加入滤波 让刹车更线性
        if (fabs(_error) <= _pragma[LIMIT])                                                      //误差值小于阀值的时候进行积分
            _integral = clamp(_pragma[KI] * _error, _integralMin, _integralMax);
        double kpsum = _pragma[KP] * (_error - _lastError);                  //这个是方便我PRINdoubleF的
        if (_shouldResetOnCross && copySign(_error) != copySign(_lastError)) //如果距离还很远的时间积分值归零 这个可以自己按自己的工程随便写
        {
            _integral = 0;
            _filter->reset();
        }
        _prevError = _lastError;
        _lastError = _error;
        _incrementSum += (kpsum + _integral + derivative + _pragma[BIAS]);
        _output = clamp(_incrementSum, _outputMin, _outputMax);
        //  if (_name != "左右pid")
        // logger->info({"targe:", std::to_string(_target), " error:", std::to_string(_error), " output:", std::to_string(static_cast<int>(_output)), " incrementSum:",
        //               std::to_string(static_cast<int>(_incrementSum)),
        //               " kp:", std::to_string(kpsum), " ki:", std::to_string(_integral), " kd:", std::to_string(derivative)});
        //   std::cout << "millis:" << now - _lastTime << " targe:" << _target << " error:" << _error << " output" << _output << " kp:" << _kpsum << " ki:" << _integral << " kd:" << _derivative << std::endl;

        if (sysData->test != 0 && fabs(_error) < fabs(_target) * 0.2)
            sysData->pidDebugData.push_back(std::make_tuple(_error, _output, kpsum, _integral, derivative));
    }
    else
    {
        _output = 0; //Controller is off so write 0
    }
    if (_firstRun < _firstRunNums)
    {
        _lastError = _error;
        return (_outputMax * copySign(_output));
        _firstRun++;
    }
    else
        return _output;
}
void Pid::setFilterPragma(const double q, const double r)
{
    _filter->setPragmas(q, r);
}
void Pid::reName(std::string newName)
{
    _name = newName;
}
void Pid::showPidParams()
{
    std::cout << _name << "\tp\ti\td\tgains\tlimit\tQ\tR\t" << std::endl;
    std::cout << _name << "\t";
    for (auto &it : _pragma)
        std::cout << it << "\t";
    std::cout << std::endl;
}
/**
     * 设置积分区间
     */
void Pid::setLimits(const double iLimit)
{
    _pragma[LIMIT] = iLimit;
}
void Pid::setTarget(const double itarget)
{
    _target = itarget;
}

double Pid::getOutput() const { return _output; }

double Pid::getError() const { return _error; }

//设置所有的可调参数
void Pid::setAllPragmas(const json &pragma)
{
    DataType().swap(_pragma); //用这这个你方式比 clean()释放内存效果好!
                              //遍历录入pid
    for (auto &it1 : pragma)
        _pragma.push_back(it1.get<double>());
    logger->info({"重置:", _name, "PID参数"});
    showPidParams();
}

/**
     *设置控制器_输出边界
     * @param imin Min _output
     * @param imax Max _output
     */
void Pid::setOutputLimits(double imin, double imax)
{
    if (imin > imax)
    {
        const double temp = imax;
        imax = imin;
        imin = temp;
    }
    _outputMax = imax;
    _outputMin = imin;
    //Fix integral
}

/**
     * 设置积分器边界
     * @param imin Min integrator value
     * @param imax Max integrator value
     */
void Pid::setIntegralLimits(double imin, double imax)
{
    if (imin > imax)
    {
        const double temp = imax;
        imax = imin;
        imin = temp;
    }
    _integralMax = imax;
    _integralMin = imin;
}

/**
    *重置控制器，使其可以再次从0开始。保持以前的收益和限制
     */
void Pid::reset()
{
    _error = 0;
    _lastError = 0;
    _prevError = 0;
    _integral = 0;
    _output = 0;
    _incrementSum = 0;
    _firstRun = 0;
    _filter->reset();
}
void Pid::setIntegratorReset(bool iresetOnZero) { _shouldResetOnCross = iresetOnZero; }
//惯性克制算法：

double Pid::PID_Control(const double inewReading)
{
    if (_isOn)
    {
        _error = _target - inewReading;
        _integral = _error;
        for (auto &it : _lastErrorArray)
            _integral += it;
        _integral *= _pragma[KI];
        double kpsum = _pragma[KP] * _error;                                 //计算比例分量(P)=比例系数*本次位置差    
        double derivative = _pragma[KD] * (_error - _lastErrorArray.back()); //计算微分分量(D)=微分系数*(本次位置差-前3次的位置差) 
        for (std::array<double, 5>::reverse_iterator it = _lastErrorArray.rbegin(); it != _lastErrorArray.rend(); it++)
            if (it != _lastErrorArray.rend() - 1)
                *it = *(it - 1);
            else
                *it = _error;
        _output = kpsum + _integral + derivative; //P分量和D分量相加，得到控制量。
    }
    else
        _output = 0; //Controller is off so write 0
    if (_firstRun < _firstRunNums)
    {
        _lastError = _error;
        return (_outputMax * copySign(_output));
        _firstRun++;
    }
    else
        return _output;
}
/**
     * 设置当错误为0或更改符号时是否应重置积分器
     * @param iresetOnZero doublerue to reset
     */

void Pid::flipDisable() { _isOn = !_isOn; }

} // namespace ncrapi