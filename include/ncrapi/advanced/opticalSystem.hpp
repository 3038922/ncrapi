#pragma once
#include "ncrapi/advanced/filter/kalman.hpp"
#include "ncrapi/device/adi.hpp"
#include "ncrapi/device/opticalGroup.hpp"
#include "ncrapi/system/circular_buffer.hpp"
namespace ncrapi {

const int CacheTotal = 7; //缓存总量
class OpticalSystem : public OpticalGroup
{
  public:
    enum OpticalPragma {
        RED = 0,
        BLUE,
        LEDPWM,     //灯的亮度
        PROXIMITY,  //距离
        SATURATION, //饱和度
        LIMIT       //hue取值范围

    };
    using PARADATA = std::vector<std::tuple<double, double, int, int, double, int>>;

    //光学系统初始化
    static OpticalSystem *initOpticalSystem(const json &pragma);
    //但获取单例指针
    static OpticalSystem *getOpticalSystem();
    void showLineInfo(std::ostringstream &ostr)
    {
        _line.showADIAnalogInInfo(ostr);
    }
    /**
     * @brief 获取色相
     * 
     * @param nums 传感器编号
     * @return double 返回色相 如果是红色 择返回负数 
     */
    double getHue(const int nums) override;
    /**
     * @brief 获取发射巡线的值
     * 
     * @return int 返回发射巡线的值
     */
    int getLineVal()
    {
        return _line.get_value();
    }
    /**
     * @brief 获取巡线设定的阈值
     * 
     * @return int 
     */
    int getLineTarget()
    {
        return _line.getTargetVal();
    }
    /**
     * @brief  设置巡线设定的阈值
     * 
     * @param val 
     */
    void setLineTarget(const int val)
    {
        _line.setTargetVal(val);
    }
    /**
     * @brief 从机器人射球口加入一个球
     * 
     * @param ball 红球写 'R' 蓝球写 'B' 未知写 'N'
     */

    void pushFrontBall(const char ball);
    /**
     * @brief 从机器人吸球口加入一个球
     * 
     * @param ball 红球写 'R' 蓝球写 'B' 未知写 'N'
     */
    void pushBackBall(const char ball);
    /**
     * @brief 获取用户自己定义设定的红色HUE目标值
     * 
     * @param nums 0 下光学传感器 1 上光学传感器
     * @return double 返回JSON设置的阈值
     */
    double getRedTarget(const int nums);
    /**
     * @brief 获取用户自己定义设定的蓝色HUE目标值
     * 
     * @param nums 0 下光学传感器 1 上光学传感器
     * @return double 返回JSON设置的阈值
     */
    double getBlueTarget(const int nums);
    /**
     * @brief 获取用户要吸的球的HUE目标值
     * 
     * @return double 返回JSON设置的阈值
     */
    double getBallTarget();
    /**
     * @brief 获取用户要拉屎的HUE目标值
     * 
     * @return double 返回JSON设置的阈值
     */
    double getPoopTarget();
    /**
     * @brief 设置用户的要吸的球的HUE目标值
     * 
     * @param char  红方'R' 蓝方'B'
     */
    void setBallTarget(char side);
    /**
     * @brief 设置用户的拉屎HUE目标值
     * 
     * @param char  红方'R' 蓝方'B'
     */
    void setPoopTarget(char side);

    /**
     * @brief 获取用户自定义的LED亮度
     * 
     * @param nums 0 下光学传感器 1 上光学传感器
     * @return int 返回JSON设置的亮度
     */
    int getLedPwmTarget(const int nums);
    /**
     * @brief 获取用户自己定义设定的距离阈值
     * 
     * @param nums 0 下光学传感器 1 上光学传感器
     * @return int 返回JSON设置的阈值
     */
    int getProximityTarget(const int nums);
    /**
     * @brief 用户自己定义设定的饱和度
     * 
     * @param nums 0 下光学传感器 1 上光学传感器
     * @return double 返回JSON设置的阈值
     */
    double getSaturationTarget(const int nums);

    /**
    * @brief 打印存球队列到引用流
    * 
    * @param ostr 要打印的流
    */
    void showBallBuffer(std::ostringstream &ostr);
    /**
     * @brief 打印存球队列到引用的str类型
     * 
     * @param str 要打印的str;
     */
    void showBallBUffer(std::string &str);
    /**
     * @brief 获取球的数量
     * 
     * @return int 返回机器内球的数量
     */
    int getBallNums()
    {
        return _ballBuffer.size();
    }
    /**
     * @brief 获取我方球数,没识别的颜色算成我方
     * 
     * @return int 返回我包球数,没识别的颜色算成我方
     */
    int getWeBallNums();
    /**
     * @brief 获取自重置以来吸入了几个球
     * 
     * @return int 吸入的球数
     */
    int getInBallNums()
    {
        return _inBallNums;
    }
    /**
     * @brief 获取自重置以来发射器里的球数
     * 
     * @return int 发射器中的球数
     */
    int getShootBallNums()
    {
        return _shootBallNums;
    }
    /**
     * @brief 获取自重置以来吐出了几个球
     * 
     * @return int 吐出的球数
     */
    int getOutBallNums()
    {
        return _outBallNums;
    }
    /**
     * @brief 重置吸入的球数
     * 
     */
    void resetInBallNums()
    {
        _inBallNums = 0;
    }

    /**
     * @brief 重置吐出的球数
     * 
     */
    void resetOutBallNums()
    {
        _outBallNums = 0;
    }
    /**
     * @brief 重置发射器里的球数
     * 
     */
    void resetShootBallNums()
    {
        _shootBallNums = 0;
    }
    /**
     * @brief 获取新进球的颜色
     * 
     * @return char R红球 B蓝球 N 不颜色
     */
    char getNewBallColor()
    {
        return _ballBuffer.back();
    }
    /**
     * @brief 获取机器发射口球的颜色
     * 
     * @return char 返回 R B N三种颜色
     */
    char getFrontBall()
    {
        return _ballBuffer.front();
    }
    /**
     * @brief 清空存球阵列
     * 
     */
    void clearBallBuffer()
    {
        _ballBuffer.clear();
        resetShootBallNums();
    }
    /**
     * @brief 删除最前面的一个球
     * 
     */
    void popFrontBall()
    {
        if (_ballBuffer.size() > 0)
            _ballBuffer.pop_front(); //剔除最后的
    }
    /**
     * @brief 当前是否需要拉屎
     * 
     * @return true 需要拉屎
     * @return false 不需要
     */
    bool isPoopBall()
    {
        return _isPoopBall;
    }

  private:
    /**
   * @brief 球数光学波形匹配算法
   * 
   * @param pragmaList 自定义参数列表的引用
   * @param nums 选择第几个缓存队列进行计算
   * @return true 符合波形返回TRUE
   * @return false 不符合返回FALSE
   */
    bool opticalWaveformMatching(const PARADATA &pragmaList, const int nums);
    /**
   * @brief 球数巡线波形匹配算法
   * @param nums 选择第几个缓存队列进行计算
   * @return true 符合波形返回TRUE
   * @return false 不符合返回FALSE
   */
    bool lineWaveformMatching(const int nums);
    static void opticalLoop(void *param);
    circular_buffer<int, CacheTotal> _proximityBuffer[3]; //0 低位光学传感器 1高位巡线  距离缓存队列

    circular_buffer<char, 5> _ballBuffer; //存球队列 N表示没球 R 红球 B 蓝球
    static OpticalSystem *_opticalSystem; // 单例对象
    OpticalSystem(const json &pragma);
    PARADATA _pragmaList;                              //参数列表
    int _outBallNums = 0;                              //射掉了几个球 要注意清零
    int _shootBallNums = 0;                            //发射器里的球数 要注意清零
    int _inBallNums = 0;                               //吸入了几个球 要注意清零
    ADILine _line;                                     //分球巡线
    bool _isPoopBall = false;                          //是否需要拉屎
    double _poopTarget = 0, _ballTarget = 0;           //当前用户需要拉屎的HUE值
    std::shared_ptr<KalmanFilter<double>> _lineFilter; //巡线滤波
};
#define opticalSystem OpticalSystem::getOpticalSystem()
} // namespace ncrapi