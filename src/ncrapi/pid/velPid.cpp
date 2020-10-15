

#include "ncrapi/pid/velPid.hpp"
#include "ncrapi/system/sysUser.hpp"
namespace ncrapi {

VelPid::VelPid(const std::string &name, const json &pragma) : Pid(name, pragma)
{
    _integralMin = -30;
    _integralMax = 30;
    logger->info({_name, " 速度环类构造成功"});
}

double VelPid::step(const double inewReading)
{
    if (_isOn)
    {
        _error = _target - inewReading;                                         //距离目标的差值 =期望的目标值-当前传感器的值
        double derivative = _filter->filter(_error - _lastError) * _pragma[KD]; ////这里可以加入滤波 让刹车更线性                           //误差值小于阀值的时候进行积分
        double kpsum = _pragma[KP] * _error;                                    //这个是方便我PRINdoubleF的
        _integral += _pragma[KI] * _error;
        if (_shouldResetOnCross && ncrapi::copySign(_error) != ncrapi::copySign(_lastError)) //如果距离还很远的时间积分值归零 这个可以自己按自己的工程随便写
            _filter->reset();
        _lastError = _error;
        _integral = clamp(_integral, _integralMin, _integralMax);
        _output = clamp(kpsum + _integral + derivative + _pragma[BIAS], _outputMin, _outputMax);
        logger->info({_name, " targe:", std::to_string(_target), " error:", std::to_string(_error), " nowSensor:", std::to_string(inewReading), " output:", std::to_string(static_cast<int>(_output)),
                      " kp:", std::to_string(kpsum), " ki:", std::to_string(_integral), " kd:", std::to_string(derivative)});
        // #pragma GCC diagnostic push
        // #pragma GCC diagnostic ignored "-Wno-psabi"
        if (sysData->test != 0 && fabs(_error) < fabs(_target) * 0.2)
            sysData->pidDebugData.push_back(std::make_tuple(_error, _output, kpsum, _integral, derivative));
        // #pragma GCC diagnostic pop
    }
    else
        _output = 0; //Controller is off so write 0
    return _output;
}
} // namespace ncrapi
