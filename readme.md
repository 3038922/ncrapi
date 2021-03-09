# 新世纪机器人社图形界面 API

## 构建说明

- 1 本项目大量使用 c++17 特性 故必须使用 `GNU Tools for Arm Embedded Processors 8-2018-q4-major` 以上的版本编译.早期版本必然无法通过编译
- 2 本项目使用了 `pros库文件.` 但并未使用`pros`的开发环境.开发环境用户可以根据喜好自行定制.
- 3 建议给 V5 主控装上 SD 卡,本项目采用 JSON 数据格式记录所有数据,实现在主控器上更改所有配置.

## 使用说明

- 方法一:
  直接 `git clone https://github.com/3038922/ncrapi.git` 下载此项目
- 方法二:
  下载
  1. 从release里下载最新的ncrapix.x.x 压缩包 到你的 pros 项目目录.
  2. `prosv5 c f ./ncrapi@10.x.x.zip`
  3. `prosv5 c a ncrapi@10.x.x`

# 希望更多的 Vexer 一起开发图形界面

![cover5](https://github.com/3038922/ncrapi/blob/release9.0.0/pic/TIM%E5%9B%BE%E7%89%8720190219231717.jpg)

![cover1](https://github.com/3038922/ncrapi/blob/release9.0.0/pic/5bbda0227bef6_IMG_20181010_144029.jpg)

![cover2](https://github.com/3038922/ncrapi/blob/release9.0.0/pic/5bbda02298f5c_IMG_20181010_144111.jpg)

![cover3](https://github.com/3038922/ncrapi/blob/release9.0.0/pic/IMG_20181122_233957.jpg)

![cover4](https://github.com/3038922/ncrapi/blob/release9.0.0/pic/IMG_20181122_234111.jpg)
