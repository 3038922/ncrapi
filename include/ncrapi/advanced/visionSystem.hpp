#pragma once
#include "ncrapi/advanced/filter/kalman.hpp"
#include "ncrapi/device/vision.hpp"
#include "pros/rtos.hpp"

namespace ncrapi {
const size_t BASE_DATASUM = 6;                                        //原始检测的数据量
using VisionData = std::array<pros::vision_object_s_t, BASE_DATASUM>; //重命名视觉传感器的返回数据类
class VisionSystem
{
  public:
    //视觉系统初始化
    static VisionSystem *initVisionSystem(const json &pragma);
    //但获取单例指针
    static VisionSystem *getVisionSystem();
    /**
     * @brief 设置传感器签名
     * 
     * @param pragma JSON数据
     * @param group 签名组
     */
    void setSig(json &pragma, const size_t group) { _vision->setSig(pragma, group); }
    /**
     * @brief 设置曝光度
     * 
     * @param x 
     */
    void setExposure(uint8_t x);
    /**
     * @brief 获取曝光度
     * 
     * @return int16_t 返回曝光度值
     */
    int16_t getExposure();
    /**
     * @brief 把基础数据打印到字符串
     * 
     * @param str 要打印的字符串
     */
    void showBaseData(std::string &str);
    /**
     * @brief 获取视觉处理后的数据
     * 
     * @return const VisionData& 返回数据的引用
     */
    const VisionData &getData() { return _visionData; }
    /**
     * @brief 把签名打印到字符串里 注意是引用
     * 
     * @param str 要打印的字符串
     */
    void showSignature(std::string &str) { _vision->showSignature(str); }
    /**
     * @brief 返回视觉传感器.hpp里的colorData数组.
     * 
     * @return const ColorData& 返回colorData的引用.
     */
    ColorData &getColorData() { return _vision->getColorData(); }

  private:
    static void visionLoop(void *param);
    VisionData _visionData{}; //原始数据 这么写就是默认初始化为0
    VisionSystem(const json &pragma);
    static VisionSystem *_visionSystem; // 单例对象

    std::shared_ptr<Vision> _vision;
    bool _isFlip = 0; //是否翻转
    int objCount = 0;
};
#define visionSystem VisionSystem::getVisionSystem()
} // namespace ncrapi
