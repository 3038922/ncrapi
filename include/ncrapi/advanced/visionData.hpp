#pragma once
#include "ncrapi/advanced/filter/kalman.hpp"
#include "ncrapi/device/vision.hpp"
#include "pros/rtos.hpp"

namespace ncrapi {
const size_t BASE_DATASUM = 6;  //原始检测的数据
const size_t COLOR_DATASUM = 3; //总颜色数量
class VisionData
{
  public:
    enum COLOR_NAME { RED = 0,
                      BLUE,
                      GREEN,
                      ORANGE,
                      PURPLE };
    std::array<std::array<pros::vision_signature_s_t, 6>, COLOR_DATASUM> COLOR_DATA =
        {{
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
    VisionData(const std::string &name, const json &pragma);
    void setSig(json &pragma, const size_t group);
    void setExposure(uint8_t x);
    int16_t getExposure();
    int32_t getWhiteBalance();
    void showSignature(std::string &str);
    void showBaseData(std::string &str);

    /**
 *获取原始数据 
 * @return const std::array<pros::vision_object_s_t, 7>& 返回原始数据_data的引用
 */
    const std::array<pros::vision_object_s_t, BASE_DATASUM> &getData();

    /**
     * 设置视觉传感器中心点模式
     * @param zero_point  左上 E_VISION_ZERO_TOPLEFT 中心 E_VISION_ZERO_CENTER
     */
    void setzero_point(pros::vision_zero_e_t zero_point);
    /**
     *返回当前对象的数量 
     * @return int 返回当前对象的数量 
     */
    int getObjCount();
    static void loop(void *param);

  private:
    const std::string _name;
    bool _isFlip = 0;
    const Vision _vision;
    char _buf[256];
    int objCount = 0; //对象数量

    std::array<pros::vision_object_s_t, BASE_DATASUM> _data{}; //原始数据 这么写就是默认初始化为0

    static bool _comp(const pros::vision_object_s_t &data1, const pros::vision_object_s_t &data2) //重载set的排序运算符
    {
        return data1.width > data2.width;
    }
    int _maxW, _maxH;
};
} // namespace ncrapi
extern std::unique_ptr<ncrapi::VisionData> visionData;