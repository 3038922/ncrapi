/**
 * @Author: 陈昱安
 * @Date:   2018-06-05T12:03:16+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-09-08T22:37:58+08:00
 */

#pragma once
#include "ncrapi/units/QTime.hpp"
#include "pros/rtos.hpp"

namespace ncrapi {
class Timer
{

  public:
    Timer();

    /**
     * 返回自上次调用此函数以来以ms为单位传递的时间。
     * @返回自上次调用此函数以来以ms为单位传递的时间。
     */
    QTime getDt();
    /**
     * @返回当前时间
     * 
     * @return uint32_t 返回当前时间
     */
    QTime getNowTime() const;
    /**
     * 返回构造计时器的时间（以毫秒为单位）。
     * @返回构造计时器的时间（以毫秒为单位）。
     */
    QTime getStartingTime() const;

    /**
     * 返回自定时器构建以来以ms为单位传递的时间。
     * @返回自定时器构建以来以ms为单位传递的时间。
     */
    QTime getDtFromStart() const;

    /**
     * 放置时间标记。放置另一个标记将覆盖前一个标记
     * one
     */
    void placeMark();
    /**
     * 放置一个困难的时间标记。放置另一个硬标记不会覆盖前一个标记;
     * 相反，clearHardMark()必须在另一个人被放置之前调用。
     */
    void placeHardMark();

    /**
     * 清除并返回当前的硬标记
     * @返回老的时间标记
     */
    QTime clearHardMark();

    /**
     * 返回自标记放置以来的毫秒数。
     * @返回自标记放置以来的毫秒数。
     */
    QTime getDtFromMark() const;
    /**
     *放置硬标记后，以毫秒为单位返回时间。
     * @放置硬标记后，以毫秒为单位返回时间。
     */
    QTime getDtFromHardMark() const;

    /**
     * 当输入时间段已过时返回true，然后重置。意味着
     * 在循环中使用，以便在不阻塞的情况下运行许多毫秒的动作。
     * @param  ms 毫秒
     * @return    当输入时间段通过时返回true，false后阅读真实，直到时间又过去了
     */
    bool repeat(QTime ms);

  private:
    QTime firstCalled, lastCalled, mark, hardMark, repeatMark; //很长，所以我们可以使用-1，即使millis（）返回uint32_t
};
} // namespace ncrapi