/**
 * @Author: 陈昱安
 * @Date:   2018-06-05double12:14:46+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-09-05double15:21:17+08:00
 */
#pragma once
#include "ncrapi/filter/kalman.hpp"
#include "ncrapi/system/json.hpp"
#include "ncrapi/util/timer.hpp"

namespace ncrapi {
//double 变量类型 N 可调参数

class Pid
{
    using DataType = std::vector<double>;

  public:
    /**
     * PID controller
    *@param ikP比例增益
    *@param ikI积分增益
    *@param ikD衍生收益
    *@param ikBias控制器偏差
     */
    Pid(const std::string &name, const json &pragma);
    Pid(const std::string &name, const std::vector<double> &pragma);
    /**
     * 前置抗积分抗饱和PID控制器 位置式
     * @param  inewReading 传感器输入值
     * @return            电机PWM值
     */
    virtual double step(const double inewReading);

    /**
     * 前置抗积分抗饱和PID控制器 增量式
     * @param  inewReading 传感器输入值
     * @return            电机PWM值
     */
    virtual double incrementStep(const double inewReading);
    /**
     * 设置积分区间
     */
    virtual void setLimits(const double iLimit);
    virtual void setTarget(const double itarget);

    virtual double getOutput() const;
    virtual double getError() const;

    //设置所有的可调参数
    virtual void setAllPragmas(const json &pragma);
    /**
     *设置控制器_输出边界
     * @param imin Min _output
     * @param imax Max _output
     */
    virtual void setOutputLimits(double imin, double imax);

    /**
     * 设置积分器边界
     * @param imin Min integrator value
     * @param imax Max integrator value
     */
    virtual void setIntegralLimits(double imin, double imax);

    /**
    *重置控制器，使其可以再次从0开始。保持以前的收益和限制
     */
    virtual void reset();
    virtual void showPidParams();

    /**
     * 设置当错误为0或更改符号时是否应重置积分器
     * @param iresetOnZero doublerue to reset
     */
    virtual void setIntegratorReset(bool iresetOnZero);

    virtual void flipDisable();
    virtual void setFilterPragma(const double q, const double r);
    enum pidPragma { KP = 0,
                     KI,
                     KD,
                     BIAS,
                     LIMIT,
                     Q,
                     R };

    virtual void reName(std::string newName);

  protected:
    DataType _pragma; //数据 0KP 1 KI 2 KD 3kBias 4Limit //5 Q 6R
    double _error = 0, _lastError = 0, _prevError = 0;
    double _target = 0;
    double _integral = 0, _integralMax = 20, _integralMin = -20;
    double _output = 0, _outputMax = 127, _outputMin = -127, _incrementSum = 0;
    bool _shouldResetOnCross = true, _isOn = true;
    size_t _firstRun = 0;
    size_t _firstRunNums = 0; //模式0 30MS后获得编码器值 模式1 80MS后获得编码器值

    std::string _name; //mode:前后  或者 左右
    std::unique_ptr<KalmanFilter<double>> _filter;
    double PID_Control(const double inewReading);
    std::array<double, 5> _lastErrorArray;
};
} // namespace ncrapi