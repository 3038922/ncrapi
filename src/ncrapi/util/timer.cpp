/**
 * @Author: 陈昱安
 * @Date:   2018-06-05T12:03:16+08:00
 * @Email:  31612534@qq.com
 * @Last modified by:   陈昱安
 * @Last modified time: 2018-09-08T22:37:58+08:00
 */

#include "ncrapi/util/timer.hpp"
namespace ncrapi {

Timer::Timer() : firstCalled(QTime(pros::millis() * millisecond)),
                 lastCalled(0_ms), mark(0_ms), hardMark(-1_ms), repeatMark(-1_ms) {}

/**
     * 返回自上次调用此函数以来以ms为单位传递的时间。
     * @返回自上次调用此函数以来以ms为单位传递的时间。
     */
QTime Timer::getDt()
{
    const QTime currTime = QTime(pros::millis() * millisecond);
    const QTime dt = currTime - lastCalled;
    lastCalled = currTime;
    return dt;
}
/**
     * @返回当前时间
     * 
     * @return QTime 返回当前时间
     */
QTime Timer::getNowTime() const
{
    return QTime(pros::millis() * millisecond);
}

/**
     * 返回构造计时器的时间（以毫秒为单位）。
     * @返回构造计时器的时间（以毫秒为单位）。
     */
QTime Timer::getStartingTime() const
{
    return firstCalled; //构造函数创建的时间
}

/**
     * 返回自定时器构建以来以ms为单位传递的时间。
     * @返回自定时器构建以来以ms为单位传递的时间。
     */
QTime Timer::getDtFromStart() const
{
    return QTime(pros::millis() * millisecond) - firstCalled;
}

/**
     * 放置时间标记。放置另一个标记将覆盖前一个标记
     * one
     */
void Timer::placeMark()
{
    mark = QTime(pros::millis() * millisecond);
}

/**
     * 放置一个困难的时间标记。放置另一个硬标记不会覆盖前一个标记;
     * 相反，clearHardMark()必须在另一个人被放置之前调用。
     */
void Timer::placeHardMark()
{
    if (hardMark == -1_ms)
        hardMark = QTime(pros::millis() * millisecond);
}

/**
     * 清除并返回当前的硬标记
     * @返回老的时间标记
     */
QTime Timer::clearHardMark()
{
    const QTime old = hardMark;
    hardMark = -1_ms;
    return old;
}

/**
     * 返回自标记放置以来的毫秒数。
     * @返回自标记放置以来的毫秒数。
     */
QTime Timer::getDtFromMark() const
{
    return QTime(pros::millis() * millisecond) - mark;
}

/**
     *放置硬标记后，以毫秒为单位返回时间。
     * @放置硬标记后，以毫秒为单位返回时间。
     */
QTime Timer::getDtFromHardMark() const
{
    return hardMark == -1_ms ? 0_ms : QTime(pros::millis() * millisecond) - hardMark;
}

/**
     * 当输入时间段已过时返回true，然后重置。意味着
     * 在循环中使用，以便在不阻塞的情况下运行许多毫秒的动作。
     * @param  ms 毫秒
     * @return    当输入时间段通过时返回true，false后阅读真实，直到时间又过去了
     */
bool Timer::repeat(QTime ms)
{
    if (repeatMark == -1_ms)
        repeatMark = QTime(pros::millis() * millisecond);
    if (QTime(pros::millis() * millisecond) - repeatMark >= ms)
    {
        repeatMark = -1_ms;
        return true;
    }
    return false;
}
} // namespace ncrapi