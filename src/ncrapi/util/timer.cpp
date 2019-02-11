/**
 * @Author: 陈昱安
 * @Date:   2018-06-05T12:03:16+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-09-08T22:37:58+08:00
 */

#include "ncrapi/util/timer.hpp"
namespace ncrapi {

Timer::Timer() : firstCalled(pros::millis()), lastCalled(0), mark(0), hardMark(-1), repeatMark(-1) {}

/**
     * 返回自上次调用此函数以来以ms为单位传递的时间。
     * @返回自上次调用此函数以来以ms为单位传递的时间。
     */
uint32_t Timer::getDt()
{
    const uint32_t currTime = pros::millis();
    const uint32_t dt = currTime - lastCalled;
    lastCalled = currTime;
    return dt;
}
/**
     * @返回当前时间
     * 
     * @return uint32_t 返回当前时间
     */
uint32_t Timer::getNowTime() const
{
    return pros::millis();
}

/**
     * 返回构造计时器的时间（以毫秒为单位）。
     * @返回构造计时器的时间（以毫秒为单位）。
     */
uint32_t Timer::getStartingTime() const
{
    return firstCalled; //构造函数创建的时间
}

/**
     * 返回自定时器构建以来以ms为单位传递的时间。
     * @返回自定时器构建以来以ms为单位传递的时间。
     */
uint32_t Timer::getDtFromStart() const
{
    return pros::millis() - firstCalled;
}

/**
     * 放置时间标记。放置另一个标记将覆盖前一个标记
     * one
     */
void Timer::placeMark()
{
    mark = pros::millis();
}

/**
     * 放置一个困难的时间标记。放置另一个硬标记不会覆盖前一个标记;
     * 相反，clearHardMark()必须在另一个人被放置之前调用。
     */
void Timer::placeHardMark()
{
    if (hardMark == -1)
        hardMark = pros::millis();
}

/**
     * 清除并返回当前的硬标记
     * @返回老的时间标记
     */
uint32_t Timer::clearHardMark()
{
    const uint32_t old = hardMark;
    hardMark = -1;
    return old;
}

/**
     * 返回自标记放置以来的毫秒数。
     * @返回自标记放置以来的毫秒数。
     */
uint32_t Timer::getDtFromMark() const
{
    return pros::millis() - mark;
}

/**
     *放置硬标记后，以毫秒为单位返回时间。
     * @放置硬标记后，以毫秒为单位返回时间。
     */
uint32_t Timer::getDtFromHardMark() const
{
    return hardMark == -1 ? 0 : pros::millis() - hardMark;
}

/**
     * 当输入时间段已过时返回true，然后重置。意味着
     * 在循环中使用，以便在不阻塞的情况下运行许多毫秒的动作。
     * @param  ms 毫秒
     * @return    当输入时间段通过时返回true，false后阅读真实，直到时间又过去了
     */
bool Timer::repeat(uint32_t ms)
{
    if (repeatMark == -1)
        repeatMark = pros::millis();
    if (pros::millis() - repeatMark >= ms)
    {
        repeatMark = -1;
        return true;
    }
    return false;
}
} // namespace ncrapi