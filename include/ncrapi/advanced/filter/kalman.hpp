/**
 * @Author: 陈昱安
 * @Date:   2018-07-21T12:11:51+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-09-08T22:38:09+08:00
 */
#pragma once
#include "ncrapi/i18n/chinese.hpp"
#include "ncrapi/system/logger.hpp"

namespace ncrapi {
template <class T>
class KalmanFilter
{

  public:
    KalmanFilter(const double Q, const double R) : _fQ(Q), _fR(R)
    {
        logger->info({I18N_KALMAN_FILTER I18N_CREATE_SUCCESSFUL});
    }

    virtual const T &filter(const T &ireading)
    {
        _fMid[0] = _fLast[0];                               //将上一次的系统最优赋值给一个中间变量
        _fMid[1] = _fLast[1] + _fQ;                         //将上一次的COVARIANCE值加上系统噪声（This->Q）赋值给一个中间变量（系统白噪声在你调试的过程中 换着变量赋值，但绝对不能是零，在不断的测试过程中你修改这个值，得到你要的最佳效果）
        _fKg = _fMid[1] / (_fMid[1] + _fR);                 // 计算kg的公式
        _fNow[0] = _fMid[0] + _fKg * (ireading - _fMid[0]); //通过计算得到这次的系统最优nMersure为系统采样
        _fNow[1] = (1.0 - _fKg) * _fMid[1];                 // 计算当前的covariance值
        _fKlmFlag = _fNow[0] - _fLast[0];
        _fLast[0] = _fNow[0]; //更新系统最优
        _fLast[1] = _fNow[1];
        return _fNow[0];
    }
    virtual const T &filter(const T &sensor1, const T &sensor2)
    {
        _sensor2Now = sensor2;
        _fMid[0] = _fLast[0] + _sensor2Now - _sensor2Last; //将上一次的系统最优赋值给一个中间变量
        _fMid[1] = _fLast[1] + _fQ;                        //将上一次的COVARIANCE值加上系统噪声（This->Q）赋值给一个中间变量（系统白噪声在你调试的过程中 换着变量赋值，但绝对不能是零，在不断的测试过程中你修改这个值，得到你要的最佳效果）
        _fKg = _fMid[1] / (_fMid[1] + _fR);                // 计算kg的公式
        _fNow[0] = _fMid[0] + _fKg * (sensor1 - _fMid[0]); //通过计算得到这次的系统最优nMersure为系统采样
        _fNow[1] = (1 - _fKg) * _fMid[1];                  // 计算当前的covariance值
        _fLast[0] = _fNow[0];                              //更新系统最优
        _fLast[1] = _fNow[1];
        _sensor2Last = _sensor2Now;
        return _fNow[0];
    }
    // virtual void setFlast(const T *fLast)
    // {
    //     _fLast = fLast;
    // }
    virtual const T &getOutput() const
    {
        return _fNow[0];
    }
    //设置Q R
    virtual void setPragmas(const double q, const double r)
    {
        _fQ = q;
        _fR = r;
    }
    virtual void reset()
    {
        _fLast[0] = 0;
        _fLast[1] = 0;
        _fMid[0] = 0;
        _fMid[1] = 0;
        _fNow[0] = 0;
        _fNow[1] = 0;
        _fKg = 0;
        _fKlmFlag = 0;
    }

  protected:
    T _fLast[2] = {0};
    T _fMid[2] = {0};
    T _fNow[2] = {0};

    double _fQ = 0; // Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
    double _fR = 0; // R:测量噪声，R增大，动态响应变慢，收敛稳定性变好
    T _fKg = 0;
    T _fKlmFlag = 0;
    T _sensor2Now = 0;
    T _sensor2Last = 0;
};
} // namespace ncrapi