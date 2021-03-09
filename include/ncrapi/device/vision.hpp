#pragma once
#include "ncrapi/system/json.hpp"
#include "pros/vision.hpp"

namespace ncrapi {
using ColorData = std::vector<std::array<pros::vision_signature_s_t, 6>>; //重命名颜色数据

class Vision : public pros::Vision
{

  public:
    enum COLOR_NAME { RED = 0,
                      BLUE,
                      GREEN,
                      ORANGE,
                      PURPLE };
    explicit Vision(const std::string &name, const json &pragma);
    /**
     * @brief 获取视觉传感器名字
     * 
     * @return std::string 返回名字
     */
    virtual std::string getName();
    /**
     * @brief 设置传感器签名
     * 
     * @param pragma JSON数据
     * @param group 签名组
     */
    virtual void setSig(const json &pragma, const size_t group);
    /**
     * @brief 设置物体的最大识别
     * 
     * @param maxW 最大识别宽
     * @param maxH 最大识别高
     */
    virtual void setMaxSize(const int maxW, const int maxH);
    /**
     * @brief 设置物体的最小识别
     * 
     * @param minW 最小识别宽
     * @param minH 最小识别高
     */
    virtual void setMinSize(const int minW, const int minH);
    /**
     * @brief 把签名打印到字符串里 注意是引用
     * 
     * @param str 要打印的字符串
     */
    virtual void showSignature(std::string &str);
    /**
     * @brief 返回.hpp里的colorData数组.
     * 
     * @return const ColorData& 返回colorData的引用.
     */
    virtual ColorData &getColorData() { return _colorData; }

  private:
    const std::string _name;

    int _maxW, _maxH, _minW, _minH;

    static bool _comp(const pros::vision_object_s_t &data1, const pros::vision_object_s_t &data2) //重载set的排序运算符
    {
        return data1.width > data2.width;
    }

    ColorData _colorData = {{
        //视觉数据
        {{{1, {1, 0, 0}, 11.000000, 9919, 11271, 10594, -979, -487, -732, 11618883, 1},
          {1, {1, 0, 0}, 11.000000, 9563, 10979, 10270, -1075, 1, -536, 6761512, 1},
          {1, {1, 0, 0}, 11.000000, 11111, 12303, 11706, -1431, -767, -1098, 0, 1},
          {1, {1, 0, 0}, 11.000000, 8639, 9751, 9194, -873, -361, -618, 0, 1},
          {1, {1, 0, 0}, 10.000000, 8665, 9985, 9326, -1229, -363, -796, 0, 1},
          {1, {1, 0, 0}, 11.000000, 9563, 10979, 10270, -1075, 1, -536, 0, 1}}}, //红色

        {{{2, {1, 0, 0}, 11.000000, -4537, -3733, -4136, 10515, 12007, 11260, 1254743, 1},
          {2, {1, 0, 0}, 11.000000, -4161, -2827, -3494, 10505, 11757, 11132, 1319504, 1},
          {2, {1, 0, 0}, 11.000000, -4339, -3591, -3964, 13873, 15863, 14868, 0, 1},
          {2, {1, 0, 0}, 11.000000, -3215, -2557, -2886, 6533, 7339, 6936, 0, 1},
          {2, {1, 0, 0}, 6.500000, -3739, -3085, -3412, 6401, 8171, 7286, 0, 1},
          {2, {1, 0, 0}, 11.000000, -4161, -2827, -3494, 10505, 11757, 11132, 0, 1}}}, //蓝色

        {{{3, {1, 0, 0}, 9.500000, -4105, -3529, -3818, -6795, -6129, -6462, 9421418, 1},
          {3, {1, 0, 0}, 6.400000, -2543, -1189, -1866, -7423, -6361, -6892, 14087804, 1},
          {3, {1, 0, 0}, 6.400000, -2543, -1189, -1866, -7423, -6361, -6892, 0, 1},
          {3, {1, 0, 0}, 7.000000, -3775, -2525, -3150, -7551, -6355, -6952, 0, 1},
          {3, {1, 0, 0}, 8.900000, -263, -155, -208, -4379, -3985, -4182, 0, 1},
          {3, {1, 0, 0}, 9.500000, -4105, -3529, -3818, -6795, -6129, -6462, 0, 1}}}, //绿色
    }};
};
} // namespace ncrapi