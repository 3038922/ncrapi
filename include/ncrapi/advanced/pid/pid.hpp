#pragma once
#include "ncrapi/system/circular_buffer.hpp"
#include "ncrapi/system/json.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/system/ncrSystem.hpp"
#include "ncrapi/util/timer.hpp"
#include "ncrapi/util/util.hpp"

namespace ncrapi {

template <typename Input, typename Output>
class Pid
{
    enum pidPragma { KP = 0,
                     KI,
                     KD,
                     BIAS,
                     LIMIT };

  protected:
    // using PidDebugTuple = std::tuple<Input, Input, Input, Input, Input>; //PIDDATA的类型
    using DataType = std::vector<double>;

  public:
    /**
   * @brief PID模板类构造函数 JSON
   * 
   * @param name PID控制器名字 P:比例 I:积分 D:微分 
   * @param pragma JSON参数
   */
    Pid(const std::string &name, const json &pragma) : _name(name)
    {
        for (auto &it : pragma[name])
            for (auto &it1 : it)
                _pragma.push_back(it1.get<double>());
        logger->info({"pid基类构造成功"});
    }
    /**
     * @brief PID模板类构造函数 vector
     * 
     * @param name 
     * @param pragma 
     */
    Pid(const std::string &name, const DataType &pragma) : _name(name), _pragma(pragma)
    {
        logger->info({"pid基类构造成功"});
    }
    /**
    * @brief PID核心计算
    * 
    * @param inewReading 新读取的值
    * @return Output PID算法后的获取值
    */
    virtual Output step(const Input inewReading, const bool isShowDebug = false)
    {
        if (_isOn)
        {
            calcNowError(inewReading); //距离目标的差值 =期望的目标值-当前传感器的值
            calcNowKdSum();            //计算KD积分
            calcNowKpSum();            //这个是方便我PRINT doubleF的
            calcNowKiSum();
            calcNowOutput();
            if (isShowDebug)
                showDebugData();
        }
        else
            _output = 0; //Controller is off so write 0
        if (_error.size() < 3)
            return (_outputMax * copySign(_output));
        else
            return _output;
    }
    /**
   * @brief 设置积分值
   * 
   * @param iLimit  积分值
   */
    virtual void setLimits(const Input iLimit)
    {
        _pragma[LIMIT] = iLimit;
    }
    /**
     * @brief 设置目标值
     * 
     * @param itarget 目标值
     */
    virtual void setTarget(const Input itarget)
    {
        _target = itarget;
    }
    /**
     * @brief 设置所有的可调参数 json数据类型
     * 
     * @param pragma json的数据类型
     */
    virtual void setAllPragmas(const json &pragma)
    {
        DataType().swap(_pragma); //用这这个你方式比 clean()释放内存效果好!
                                  //遍历录入pid
        for (auto &it1 : pragma)
            _pragma.push_back(it1.get<double>());
        logger->info({"重置:", _name, "PID参数"});
        showPidParams();
    }

    /**
     * 设置积分器边界
     * @param imin Min integrator value
     * @param imax Max integrator value
     */
    virtual void setIntegralLimits(Input imin, Input imax)
    {
        if (imin > imax)
            std::swap(imin, imax);
        _integralMax = imax;
        _integralMin = imin;
    }
    /**
     *设置控制器_输出边界
     * @param imin Min _output
     * @param imax Max _output
     */
    virtual void setOutputLimits(Output imin, Output imax)
    {
        if (imin > imax)
            std::swap(imin, imax);
        _outputMax = imax;
        _outputMin = imin;
    }

    /**
     * @brief 获取输出值
     * 
     * @return 返回输出值
     */

    virtual Output getOutput() const { return _output; }
    /**
     * @brief 获取误差值
     * 
     * @return Input 返回误差值
     */
    virtual Input getError() const { return _error.back(); }

    /**
     * @brief 重置控制器，使其可以再次从0开始。保持以前的收益和限制
     */
    virtual void reset()
    {
        _error.clear();
        // _error.push_back(0); //clear 后 .back()依然会返回最后一次的值 所以要add 0;
        _kiSum = 0;
        _output = 0;
    }
    /**
     * @brief 显示所有Pid配置参数
     * 
     */
    virtual void showPidParams()
    {
        std::cout << _name << "\tp\ti\td\tgains\tlimit\t" << std::endl;
        std::cout << _name << "\t";
        for (auto &it : _pragma)
            std::cout << it << "\t";
        std::cout << std::endl;
    }
    // virtual circular_buffer<PidDebugTuple, 20> &getDebugData() { return _deBugData; }
    /**
     * @brief 是否禁用此PID算法
     * 
     */
    virtual void flipDisable(bool isOn) { _isOn = isOn; }
    virtual void reName(std::string newName) { _name = newName; }

  protected:
    /**
     * @brief 计算误差.把新误差加入队列
     * 
     * @param inewReading 
     */
    virtual void calcNowError(Input inewReading) { _error.push_back(_target - inewReading); }
    virtual void calcNowKdSum() { _kdSum = (_error.back() - _error[1]) * _pragma[KD]; } //这里可以加入滤波 让刹车更线性
    virtual void calcNowKpSum() { _kpSum = _pragma[KP] * _error.back(); }
    virtual void calcNowKiSum()
    {
        if (fabs(_error.back()) <= _pragma[LIMIT]) //如果误差小于距离限制 进入PID 那机器会有个抬头问题.
            _kiSum += _pragma[KI] * _error.back();
        if (ncrapi::copySign(_error.back()) != ncrapi::copySign(_error[1])) //如果距离还很远的时间积分值归零 这个可以自己按自己的工程随便写
            _kiSum = 0;
        _kiSum = clamp(_kiSum, _integralMin, _integralMax);
    }
    virtual void calcNowOutput() { _output = clamp(_kpSum + _kiSum + _kdSum + _pragma[BIAS], _outputMin, _outputMax); }
    // virtual void addDebugData()
    // {
    //     if (fabs(_error[1]) < fabs(_target) * 0.2) //如果开启测试模式 且 上次误差 小于 目标值*0.2
    //         _deBugData.push_back(std::make_tuple(_error.back(), _output, _kpsum, _integral, _derivative));
    // }
    virtual void showDebugData()
    {
        if (copySign(_error.back()) == copySign(_error[1]))
            logger->info({"targe:", std::to_string(_target), " error:", std::to_string(_error.back()),
                          " output:", std::to_string(static_cast<int>(_output)),
                          " kp:", std::to_string(_kpSum),
                          " ki:", std::to_string(_kiSum),
                          " kd:", std::to_string(_kdSum)});
        else
            logger->debug({"targe:", std::to_string(_target), " error:", std::to_string(_error.back()),
                           " output:", std::to_string(static_cast<int>(_output)),
                           " kp:", std::to_string(_kpSum),
                           " ki:", std::to_string(_kiSum),
                           " kd:", std::to_string(_kdSum)});
    }
    // circular_buffer<PidDebugTuple, 20> _deBugData;
    DataType _pragma;                                        //数据 0KP 1 KI 2 KD 3kBias 4Limit //5 Q 6R
    circular_buffer<Input, 3> _error;                        //误差队列
    Input _target = 0;                                       //目标值
    Input _kiSum = 0, _integralMax = 20, _integralMin = -20; //当前积分 最大积分 最小积分
    Input _kpSum = 0, _kdSum = 0;                            //kp的积分 kd的积分
    Output _output = 0, _outputMax = 127, _outputMin = -127; //当前输出值 最大输出 最小输出
    bool _isOn = true;                                       // 是否打开PID
    std::string _name;                                       //mode:前后  或者 左右
};
} // namespace ncrapi