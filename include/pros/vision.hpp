/**
 * \file pros/vision.hpp
 *
 * Contains prototypes for the VEX Vision Sensor-related functions in C++.
 *
 * Visit https://pros.cs.purdue.edu/v5/tutorials/topical/vision.html to learn
 * more.
 *
 * This file should not be modified by users, since it gets replaced whenever
 * a kernel upgrade occurs.
 *
 * Copyright (c) 2017-2018, Purdue University ACM SIGBots.
 * All rights reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _PROS_VISION_HPP_
#define _PROS_VISION_HPP_

#include "pros/vision.h"

#include <cstdint>

namespace pros
{
class Vision
{
  public:
    /**
	*在给定端口上创建Vision Sensor对象。
   	 * \ param端口
   	 * V5端口号从1-21开始
   	 * \ param zero_point
   	 * vision_zero_e_t之一，用于设置FOV的（0,0）坐标
	 */
    Vision(std::uint8_t port, vision_zero_e_t zero_point = E_VISION_ZERO_TOPLEFT);

    /**
	*清除视觉传感器LED颜色，将其重置为默认值
	*行为，显示最突出的对象签名颜色。
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	* \如果操作成功则返回1或如果操作则返回PROS_ERR
	*失败，设置错误。
	 */
    std::int32_t clear_led(void) const;

    /**
	*创建表示给定签名组合的颜色代码
	* ID。
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EINVAL  - 提供的签名少于两个，或者其中一个签名
	*签名超出其[1-7]范围。
	* \ param sig_id1
	*添加到颜色代码的第一个签名ID [1-7]
	* \ param sig_id2
	*第二个签名id [1-7]添加到颜色代码中
	* \ param sig_id3
	*第三个签名ID [1-7]添加到颜色代码中
	* \ param sig_id4
	*第四个签名ID [1-7]添加到颜色代码中
	* \ param sig_id5
	*第五个签名id [1-7]添加到颜色代码中
	*
	* \ return包含颜色代码信息的vision_color_code_t对象。
	 */
    vision_color_code_t create_color_code(const std::uint32_t sig_id1, const std::uint32_t sig_id2,
                                          const std::uint32_t sig_id3 = 0, const std::uint32_t sig_id4 = 0,
                                          const std::uint32_t sig_id5 = 0) const;

    /**
	*根据size_id获取第n个最大的对象。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	* EDOM  -  size_id大于可用对象的数量。
	* EHOSTDOWN  - 读取视觉传感器失败的原因不明。
	*
	* \ param size_id
	*从按列表粗略排序的列表中读取的对象
	*（0是最大项目，1是第二大项目等）
	*
	* \ return对应于给定大小id的vision_object_s_t对象，或
	*如果发生错误，则为PROS_ERR。
	 */
    vision_object_s_t get_by_size(const std::uint32_t size_id) const;

    /**
	*根据size_id获取给定签名的第n个最大对象。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	* EDOM  -  size_id大于可用对象的数量。
	* EHOSTDOWN  - 读取视觉传感器失败的原因不明。
	*
	* \ param size_id
	*从按列表粗略排序的列表中读取的对象
	*（0是最大项目，1是第二大项目等）
	* \ param签名
	*对象将成为的vision_signature_s_t签名
	* 回。
	*
	* \ return与给定签名对应的vision_object_s_t对象
	*和size_id，或PROS_ERR如果发生错误。
	 */
    vision_object_s_t get_by_sig(const std::uint32_t size_id, const std::uint32_t sig_id) const;

    /**
	*根据size_id获取给定颜色代码的第n个最大对象。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	* EAGAIN  - 因未知原因读取视觉传感器失败。
	*
	* \ param size_id
	*从按列表粗略排序的列表中读取的对象
	*（0是最大项目，1是第二大项目等）
	* \ param color_code
	*将返回对象的vision_color_code_t
	*
	* \ return对应于给定颜色代码的vision_object_s_t对象
	*和size_id，或PROS_ERR如果发生错误。
	 */
    vision_object_s_t get_by_code(const std::uint32_t size_id, const vision_color_code_t color_code) const;

    /**
	*获取视觉传感器的曝光参数。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	*
	* \ return来自[0,100]的当前曝光百分比参数，
	*如果发生错误，则为PROS_ERR
	 */
    std::int32_t get_exposure(void) const;

    /**
	*获取视觉传感器当前检测到的对象数。
   	 *
   	 *当错误状态为时，此函数使用以下errno值
   	 * 到达：
   	 * EACCES  - 另一个资源目前正在尝试访问该端口。
   	 *
   	 * \ return在指定的视觉传感器上检测到的物体数量。
   	 *如果端口无效或发生错误，则返回PROS_ERR。
	 */
    std::int32_t get_object_count(void) const;

    /**
	*获取具有给定id号的对象检测签名。
	*
	* \ param signature_id
	*要读取的签名ID
	*
	* \ return包含有关签名信息的vision_signature_s_t。
	 */
    vision_signature_s_t get_signature(const std::uint8_t signature_id) const;

    /**
	*获取视觉传感器的白平衡参数。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	*
	* \ return传感器的当前RGB白平衡设置
	 */
    std::int32_t get_white_balance(void) const;

    /**
	*将object_count对象描述符读入object_arr。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	* EDOM  -  size_id大于可用对象的数量。
	*
	* \ param size_id
	*从按列表粗略排序的列表中读取的对象
	*（0是最大项目，1是第二大项目等）
	* \ param object_count
	*要读取的对象数
	* \ param [out] object_arr
	*将对象复制到的指针
	*
	* \ return复制的对象签名数。这个数字将小于
	* object_count如果视觉传感器检测到的物体较少。
	*如果端口无效，发生错误或更少的对象，则返回PROS_ERR
	*比找到size_id。object_arr中未找到的所有对象都是
	*将VISION_OBJECT_ERR_SIG作为其签名。
	 */
    std::int32_t read_by_size(const std::uint32_t size_id, const std::uint32_t object_count,
                              vision_object_s_t *const object_arr) const;

    /**
	*将object_count对象描述符读入object_arr。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	* EDOM  -  size_id大于可用对象的数量。
	* EHOSTDOWN  - 读取视觉传感器失败的原因不明。
	*
	* \ param object_count
	*要读取的对象数
	* \ param size_id
	*从按列表粗略排序的列表中读取的对象
	*（0是最大项目，1是第二大项目等）
	* \ param签名
	*对象将成为的vision_signature_s_t签名
	* 回。
	* \ param [out] object_arr
	*将对象复制到的指针
	*
	* \ return复制的对象签名数。这个数字将小于
	* object_count如果视觉传感器检测到的物体较少。
	*如果端口无效，发生错误或更少的对象，则返回PROS_ERR
	*比找到size_id。object_arr中未找到的所有对象都是
	*将VISION_OBJECT_ERR_SIG作为其签名。
	 */
    std::int32_t read_by_sig(const std::uint32_t size_id, const std::uint32_t sig_id, const std::uint32_t object_count,
                             vision_object_s_t *const object_arr) const;

    /**
	*将object_count对象描述符读入object_arr。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EINVAL  - 找到的对象数量少于object_count。
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	*
	* \ param object_count
	*要读取的对象数
	* \ param size_id
	*从按列表粗略排序的列表中读取的对象
	*（0是最大项目，1是第二大项目等）
	* \ param color_code
	*将返回对象的vision_color_code_t
	* \ param [out] object_arr
	*将对象复制到的指针
	*
	* \ return复制的对象签名数。这个数字将小于
	* object_count如果视觉传感器检测到的物体较少。
	*如果端口无效，发生错误或更少的对象，则返回PROS_ERR
	*比找到size_id。object_arr中未找到的所有对象都是
	*将VISION_OBJECT_ERR_SIG作为其签名。
	 */
    int32_t read_by_code(const std::uint32_t size_id, const vision_color_code_t color_code,
                         const std::uint32_t object_count, vision_object_s_t *const object_arr) const;

    /**
	*将签名的内容作为初始化列表打印到终端。
	*
	* \ param sig
	*将打印内容的签名
	*
	* \如果没有错误则返回1，否则返回PROS_ERR
	 */
    static std::int32_t print_signature(const vision_signature_s_t sig);

    /**
	*启用/禁用视觉传感器上的自动白平衡。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	*
	* \ param已启用
	*传递0表示禁用，1表示启用
	*
	* \如果操作成功则返回1或如果操作则返回PROS_ERR
	*失败，设置错误。
	 */
    std::int32_t set_auto_white_balance(const std::uint8_t enable) const;

    /**
	*设置视觉传感器的曝光参数。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	*
	* \ param％
	* [0,100]的新曝光百分比
	*
	* \如果操作成功则返回1或如果操作则返回PROS_ERR
	*失败，设置错误。
	 */
    std::int32_t set_exposure(const std::uint8_t percent) const;

    /**
	*设置视觉传感器LED颜色，覆盖自动行为。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	*
	* \ param rgb
	*用于设置LED的RGB代码
	*
	* \如果操作成功则返回1或如果操作则返回PROS_ERR
	*失败，设置错误。
	 */
    std::int32_t set_led(const std::int32_t rgb) const;

    /**
	*将提供的物体检测特征存储在视觉传感器上。
	*
	*注意：这将签名保存在易失性存储器中，签名将是
	*传感器断电后丢失。
	*
	* \ param signature_id
	*要存储的签名ID
	* \ param [in] signature_ptr
	*指向要保存的签名的指针
	*
	* \如果没有错误则返回1，否则返回PROS_ERR
	 */
    std::int32_t set_signature(const std::uint8_t signature_id, vision_signature_s_t *const signature_ptr) const;

    /**
	*设置视觉传感器的白平衡参数。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	*
	* \ param rgb
	*传感器的新RGB白平衡设置
	*
	* \如果操作成功则返回1或如果操作则返回PROS_ERR
	*失败，设置错误。
	 */
    std::int32_t set_white_balance(const std::int32_t rgb) const;

    /**
	*设置视野的（0,0）坐标。
	*
	*这将影响为每个请求返回的坐标
	*来自传感器的vision_object_s_t，所以建议使用此功能
	*仅用于在使用开始时配置传感器。
	*
	*当错误状态为时，此函数使用以下errno值
	* 到达：
	* EACCES  - 另一个资源目前正在尝试访问该端口。
	*
	* \ param zero_point
	* vision_zero_e_t之一，用于设置FOV的（0,0）坐标
	*
	* \如果操作成功则返回1或如果操作则返回PROS_ERR
	*失败，设置错误。
	 */
    std::int32_t set_zero_point(vision_zero_e_t zero_point) const;

  private:
    std::uint8_t _port;
};
} // namespace pros
#endif // _PROS_VISION_HPP_
