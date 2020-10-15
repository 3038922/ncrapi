#include "ncrapi/pid/feedforwardPid.hpp"
#include "ncrapi/system/sysUser.hpp"
namespace ncrapi {

FeedForwardPid::FeedForwardPid(const std::string &name, const json &pragma) : Pid(name, pragma)
{
    logger->info({_name, " 类构造成功"});
}
FeedForwardPid::FeedForwardPid(const std::string &name, const std::vector<double> &pragma) : Pid(name, pragma)
{
    logger->info({_name, " 类构造成功"});
}
void FeedForwardPid::statemMachine(const double maxAccel, const double maxVel)
{
    double fferror = _target - _ffPos;                //注意整理直接是米 那边设置的目标是
    if (fabs(_ffVel) >= maxVel && _ffState != deccel) //如果达到max_v，则停止加速
        _ffState = cruise;
    if (_ffState == cruise && _lastffState == accel) //如果我们只是达到最大速度，锁定加速那里的距离
        _ffAccelDist = fabs(_ffPos);                 //从标记从哪里停止加速的 就从哪里开始减速
    switch (_ffState)                                //状态机来处理要使用的方程式
    {
        case accel:                                 //加速巡航
            if (fabs(_ffPos) > fabs((_target / 2))) //如果用于加速的一半距离减速，则跳过巡航
                _ffState = deccel;
            _ffVel += (maxAccel / 100) * copySign(_target);
            _ffPos += (_ffVel);
            break;
        case cruise: //没有加速度，以巡航速度巡航
            if (fabs(fferror) <= _ffAccelDist)
                _ffState = deccel; //减速停止
            _ffPos += (_ffVel);
            break;
        case deccel: //减速停止
            if ((_ffVel < 0 && _target > 0) || (_ffVel > 0 && _target < 0))
                _ffState = rest;
            _ffVel += (maxAccel / 100) * -copySign(_target);
            _ffPos += (_ffVel);
            break;
        case rest: //done
            _ffVel = 0;
            break;
        default: //默认加速
            logger->error({"前馈控制状态机模式错误"});
            break;
    }
    _lastffState = _ffState;
}
void FeedForwardPid::reset()
{
    _error = 0;
    _lastError = 0;
    _prevError = 0;
    _integral = 0;
    _output = 0;
    _incrementSum = 0;
    _filter->reset();
    _ffState = accel;
    _ffPos = _ffVel = _ffAccelDist = 0;
} // namespace ncrapi
double FeedForwardPid::step(const double inewReading)
{
    if (_isOn)
    {
        statemMachine(1000, 1200);                                              //加速度1000mm/s 最大速度1200mm/s
        _error = _ffPos - inewReading;                                          //距离目标的差值 =期望的目标值-当前传感器的值
        double derivative = _filter->filter(_error - _lastError) * _pragma[KD]; ////这里可以加入滤波 让刹车更线性                           //误差值小于阀值的时候进行积分
        double kpsum = _pragma[KP] * _error;                                    //这个是方便我PRINdoubleF的
        if (fabs(_error) <= _pragma[LIMIT])                                     //如果误差小于距离限制 进入PID 那机器会有个抬头问题.
            _integral += _pragma[KI] * _error;
        if (_shouldResetOnCross && copySign(_error) != copySign(_lastError)) //如果距离还很远的时间积分值归零 这个可以自己按自己的工程随便写
        {
            _integral = 0;
            _filter->reset();
        }
        _lastError = _error;
        _integral = clamp(_integral, _integralMin, _integralMax);
        _output = clamp(kpsum + _integral + derivative + _pragma[BIAS] + _ffVel, _outputMin, _outputMax);
        // if (_name != "左右pid")
        logger->info({"state:", std::to_string(_ffState), " enc:", std::to_string(inewReading), " error:", std::to_string(_error), " output:", std::to_string(static_cast<int>(_output)),
                      " kp:", std::to_string(kpsum), " ki:", std::to_string(_integral), " kd:", std::to_string(derivative), " ffvel:", std::to_string(_ffVel), " ffpos:", std::to_string(_ffPos)});
        // #pragma GCC diagnostic push
        // #pragma GCC diagnostic ignored "-Wno-psabi"
        if (sysData->test != 0 && fabs(_error) < fabs(_target) * 0.2)
            sysData->pidDebugData.push_back(std::make_tuple(_error, _output, kpsum, _integral, derivative));
        // #pragma GCC diagnostic pop
    }
    else
        _output = 0; //Controller is off so write 0
    return _output;
} // namespace ncrapi
} // namespace ncrapi