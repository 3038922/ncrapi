#include "ncrapi/system/visionData.hpp"
#include "ncrapi/system/logger.hpp"
#include "ncrapi/system/sysUser.hpp"
#include "ncrapi/util/util.hpp"
#include "pros/rtos.hpp"

namespace ncrapi {

VisionData::VisionData(const json &pragma) : _visionF("视觉传感器前", pragma)
{
    _isFlip = true; //默认翻转
    _maxW = clamp<int>(pragma["最大识别宽"].get<int>(), 1, 100, "最大识别宽");
    _maxH = clamp<int>(pragma["最大识别高"].get<int>(), 1, 100, "最大识别高");
    pros::delay(700);
    _visionF.set_wifi_mode(false);
    _visionF.set_auto_white_balance(true);
    _visionF.set_exposure(clamp<int>(pragma["曝光度"].get<int>(), 0, 150, "曝光度")); //设置曝光度
    int count = 1;                                                                    //计数器 从1开始
    for (auto it : COLOR_DATA)
    {
        if (_visionF.get_signature(count).id != count || _visionF.get_signature(count).id == 255 || _visionF.get_signature(1).u_min == _visionF.get_signature(2).u_min)
        {
            logger->warning({"视觉传感器内置数据[", std::to_string(count), "]错误,使用程序内置数据"});
            it.back() = _visionF.get_signature(count);
            pros::delay(10);
        }
        count++;
    }
    setSig(sysData->jsonVal, pragma["数据"].get<int>());
    int expJson = pragma["曝光度"].get<int>();
    int expVision = _visionF.get_exposure();
    if (std::abs(expJson - expVision) > 4)
        logger->warning({"视觉传感器构建错误!json曝光度:", std::to_string(expJson), " vision值:", std::to_string(expVision)});
    else
        logger->info({"视觉数据类构造成功"});
}
void VisionData::setSig(json &pragma, const size_t group)
{
    if (!pragma["自动赛"]["红方&蓝方"]) //注意 红方蓝方打旗是打相反的颜色
    {
        COLOR_DATA[BLUE][group].range = pragma["视觉信息"]["颜色范围"]["蓝色"][group];
        _visionF.set_signature(1, &COLOR_DATA[BLUE][group]);
    }
    else
    {
        COLOR_DATA[RED][group].range = pragma["视觉信息"]["颜色范围"]["红色"][group];
        _visionF.set_signature(1, &COLOR_DATA[RED][group]);
    }
    COLOR_DATA[GREEN][group].range = pragma["视觉信息"]["颜色范围"]["绿色"][group];
    COLOR_DATA[ORANGE][group].range = pragma["视觉信息"]["颜色范围"]["橙色"][group];
    COLOR_DATA[VIOLET][group].range = pragma["视觉信息"]["颜色范围"]["紫色"][group];
    _visionF.set_signature(2, &COLOR_DATA[GREEN][group]);
    _visionF.set_signature(3, &COLOR_DATA[ORANGE][group]);
    _visionF.set_signature(4, &COLOR_DATA[VIOLET][group]);
}
void VisionData::setExposure(uint8_t x)
{
    _visionF.set_exposure(x); //设置曝光
}
int16_t VisionData::getExposure()
{
    return _visionF.get_exposure();
}
int32_t VisionData::getWhiteBalance()
{
    return _visionF.get_white_balance();
}
void VisionData::showSignature(std::string &str)
{
    for (size_t i = 1; i <= COLOR_DATASUM; i++)
    {
        sprintf(_buf, "%u,%.1f,%ld,%ld,%ld,%ld,%ld,%ld,%lu,%lu\n", // sprintf(_buf, "%u,{%u,%u,%u},%.1f,%d,%d,%d,%d,%d,%d,%u,%u\n",
                _visionF.get_signature(i).id,                      // _visionF.get_signature(i)._pad[0], _visionF.get_signature(i)._pad[1], _visionF.get_signature(i)._pad[2],
                _visionF.get_signature(i).range, _visionF.get_signature(i).u_min, _visionF.get_signature(i).u_max, _visionF.get_signature(i).u_mean,
                _visionF.get_signature(i).v_min, _visionF.get_signature(i).v_max, _visionF.get_signature(i).v_mean, _visionF.get_signature(i).rgb, _visionF.get_signature(i).type);
        str += _buf;
    }
}
void VisionData::showBaseData(std::string &str)
{
    for (auto &it : _data)
        if (it.signature != 255)
        {
            sprintf(_buf, "sig:%u,宽:%d高:%d,x_top:%d,y_top:%d,x_mid:%d,y_mid:%d\n",
                    it.signature, it.width, it.height, it.left_coord, it.top_coord, it.x_middle_coord, it.y_middle_coord);
            str += _buf;
        }
}

/**
 *获取原始数据 
 * @return const std::array<pros::vision_object_s_t, 7>& 返回原始数据_data的引用
 */
const std::array<pros::vision_object_s_t, BASE_DATASUM> &VisionData::getData()
{
    return _data;
}

/**
     * 设置视觉传感器中心点模式
     * @param zero_point  左上 E_VISION_ZERO_TOPLEFT 中心 E_VISION_ZERO_CENTER
     */
void VisionData::setzero_point(pros::vision_zero_e_t zero_point)
{
    _visionF.set_zero_point(zero_point);
}

/**
     *返回当前对象的数量 
     * @return int 返回当前对象的数量 
     */
int VisionData::getObjCount()
{
    return objCount;
}
void VisionData::loop()
{
    uint32_t now = pros::millis();
    while (true)
    {
        _visionF.read_by_size(0, BASE_DATASUM, _data.data()); //这里有点问题啊 如果远距离 他直接就检测四个蓝色或者红色的旗子了
        if (_isFlip)
            for (auto it : _data)
            {
                //TODO 待验证
                it.left_coord = it.left_coord * -1 - it.width;
                it.top_coord = it.top_coord * -1 - it.height;
                it.x_middle_coord *= -1;
                it.y_middle_coord *= -1;
            }
        objCount = _visionF.get_object_count();
        pros::Task::delay_until(&now, 20);
    }
}

} // namespace ncrapi

//融合算法 仅供后续参考
// void VisionData::loop()
// {
//     uint32_t now = pros::millis();
//     while (true)
//     {
//         _shootData.fill({0});
//         if (_mode == 0)
//         {
//             _visionF.read_by_size(0, BASE_DATASUM, _data.data()); //这里有点问题啊 如果远距离 他直接就检测四个蓝色或者红色的旗子了
//             objCount = 0;
//             size_t fuseCount = 0;  //融合数据量
//             for (auto &it : _data) //遍历视觉识别到的数据
//             {
//                 if (it.signature != 255 && it.height <= _maxH && it.width <= _maxH) // && it.height >= _minH && it.width >= _minW) //如果检测颜色匹配 且面积大于50
//                 {
//                     if (it.signature == 1 && it.width * 2 > it.height && objCount < SHOOT_DATASUM) //宽度*2 大于 高度 主要是防止误识别已经打过去的旗子
//                     {
//                         _shootData[objCount] = it;
//                         objCount++;
//                     }
//                     else if (it.signature == 2 && objCount != 0) //防止没有SHOOT DATA的情况 如果有个超大的绿色物体 TEMPDATA里并没东西!
//                     {
//                         for (auto &itRedBlue : _shootData) //遍历已有的SHOOT DATA
//                         {
//                             int yFalg = abs(itRedBlue.y_middle_coord - it.y_middle_coord);
//                             int xFalg = abs(itRedBlue.x_middle_coord - it.x_middle_coord);
//                             if (yFalg <= itRedBlue.height / 2 &&               //蓝色或者红色的Y坐标-绿色的Y坐标 小于蓝色红色的高度 说明他们差不多平行
//                                 xFalg < (itRedBlue.width + it.width) * 0.83 && //蓝色或者红色的X坐标 - 绿色的坐标比绿色的宽度*1.3倍要小
//                                 fuseCount < SHOOT_DATASUM)                     //融合数据最大不能超过设定的总数
//                             {
//                                 itRedBlue.width += it.width;              //总宽度累加
//                                 if (it.left_coord < itRedBlue.left_coord) //如果绿色左边坐标 比 红色或者蓝色坐标小
//                                     itRedBlue.left_coord = it.left_coord; //那以小的为基准 画图用
//                                 {
//                                     itRedBlue.x_middle_coord = (itRedBlue.x_middle_coord + it.x_middle_coord) / 2;
//                                 }
//                                 fuseCount++;
//                             }
//                         }
//                     }
//                 }
//             }
//             std::sort(_shootData.begin(), _shootData.end(), _comp);
//         }
//         else
//         {
//             _data[0] = _visionF.get_by_code(0, 10);
//             _data[1] = _visionF.get_by_code(1, 10);
//             _data[2] = {255};
//             _data[3] = {255};
//             //   _visionF.read_by_code(0, 10, DATASUM, _data.data());
//             objCount = 0;
//             for (auto &it : _data)
//             {
//                 //   logger->info({std::to_string(it.signature)});
//                 if (it.signature != 255 && it.height <= _maxH && it.width <= _maxH && objCount < SHOOT_DATASUM) // && it.height >= _minH && it.width >= _minW) //如果检测颜色匹配 且面积大于50
//                 {
//                     _shootData.at(objCount) = it;
//                     objCount++;
//                 }
//             }
//         }
//         pros::Task::delay_until(&now, 20);
//     }
// }