#pragma once
#include "ncrapi/device/opticalGroup.hpp"
#include "ncrapi/system/circular_buffer.hpp"
#include "userconfig.hpp"
namespace ncrapi {

const int CacheTotal = 7; //缓存总量
class OpticalSystem : public OpticalGroup
{
  public:
    enum OpticalPragma {
        RED = 0,
        BLUE,
        LEDPWM,    //灯的亮度
        PROXIMITY, //距离
        SATURATION //饱和度
    };
    //光学系统初始化
    static OpticalSystem *initOpticalSystem(const json &pragma);
    //但获取单例指针
    static OpticalSystem *getOpticalSystem();
    /**
     * @brief 从机器人射球口加入一个球
     * 
     * @param ball 红球写 'R' 蓝球写 'B' 未知写 'N'
     */

    void pushFrontBall(const char ball) { _ballBuffer.push_front(ball); };
    /**
     * @brief 从机器人吸球口加入一个球
     * 
     * @param ball 红球写 'R' 蓝球写 'B' 未知写 'N'
     */
    void pushBackBall(const char ball) { _ballBuffer.push_back(ball); };
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
     * @brief 获取新进球的颜色
     * 
     * @return char R红球 B蓝球 N 不颜色
     */
    char getNewBallColor()
    {
        return _ballBuffer.back();
    }
    /**
     * @brief 清空存球阵列
     * 
     */
    void clearBallBuffer()
    {
        _ballBuffer.clear();
    }

    /**
     * @brief 删除最后一个球
     * 
     */
    void popBackBall()
    {
        if (_ballBuffer.size() > 0)
            _ballBuffer.pop_back(); //剔除最后的
    }

  private:
    /**
   * @brief 球数波形匹配算法
   * 
   * @param pragmaList 自定义参数列表的引用
   * @param nums 0表示下光学 1表示上光学
   * @return true 符合波形返回TRUE
   * @return false 不符合返回FALSE
   */
    bool inWaveformMatching(const std::vector<std::tuple<double, double, int, int, double>> &pragmaList,
                            const int nums);
    bool outWaveformMatching(const std::vector<std::tuple<double, double, int, int, double>> &pragmaList,
                             const int nums);
    static void opticalLoop(void *param);
    circular_buffer<int, CacheTotal> _proximityBuffer[2]; //0 低位光学传感器 1高位  距离缓存队列

    circular_buffer<char, MaxBallNums> _ballBuffer; //存球队列 N表示没球 R 红球 B 蓝球
    static OpticalSystem *_opticalSystem;           // 单例对象
    OpticalSystem(const json &pragma);
    std::vector<std::tuple<double, double, int, int, double>> _pragmaList; //参数列表
    int _outBallNums = 0;                                                  //射掉了几个球 要注意清零
    int _inBallNums = 0;                                                   //吸入了几个球 要注意清零
};
#define opticalSystem OpticalSystem::getOpticalSystem()
} // namespace ncrapi