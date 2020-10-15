#include "main.hpp"
namespace ncrapi {
SysUser::SysUser(const json &paragam) : SysBase(paragam)
{
    robotLength = QLength(clamp<int>(jsonVal["系统信息"]["机器人长度"].get<int>(), 300, 750, "机器人长度") * millimeter);
    robotWidth = QLength(clamp<int>(jsonVal["系统信息"]["机器人宽度"].get<int>(), 300, 500, "机器人宽度") * millimeter);
    std::tuple temp = OdomMath::guessScales(QLength(clamp<double>(jsonVal["系统信息"]["轮间距"].get<double>(), 200, 1000, "轮间距") * millimeter),
                                            QLength(clamp<double>(jsonVal["系统信息"]["车轮直径"].get<int>(), 60, 110, "车轮直径") * millimeter));
    wheelSpacing = std::get<0>(temp);                                                                                                                                    //轮间距
    diameter = std::get<1>(temp);                                                                                                                                        //车轮直径
    siteSize = QLength(clamp<int>((jsonVal["系统信息"]["赛场边长"].get<int>() * diameter + robotLength.convert(millimeter) + 40), 3500, 3700, "赛场边长") * millimeter); //场地边长 编码器的测量值*常数+机器人长度
    logger->info({"计算得赛场边长为:", std::to_string(siteSize.convert(millimeter))});
    matSize = siteSize / 6.0;
    odomMin = (robotWidth / 2.0 + 20_mm);
    odomMax = (siteSize - 20_mm - (robotWidth / 2.0));
    logger->info({"用户系统类构造成功"});
}
void SysUser::recoder(std::vector<double> &data)
{
    _recoderData.push_back(data);
}
void SysUser::showRecoder()
{
    for (auto &it1 : _recoderData)
    {
        for (auto &it2 : it1)
            std::cout << it2 << ",";
        std::cout << std::endl;
    }
}
void SysUser::testAction()
{
    if (!pidDebugData.empty()) //empty()是空的返回1 不是空的返回0
    {
        std::vector<PidDebugTuple>().swap(pidDebugData); //用这这个你方式比 clean()释放内存效果好!
        logger->debug({"清除pid调试日志"});
    }
    switch (test)
    {
        case 0:
            chassis->resetPid("速度pid");
            pros::delay(100);
            pidData[0] = chassis->speedPid(QSpeed(pidTestTarget * mps), QSpeed(pidTestTarget * mps));
            std::cout << std::endl;
            pros::delay(1000);
            pidData[1] = chassis->speedPid(-QSpeed(pidTestTarget * mps), -QSpeed(pidTestTarget * mps));
            break;
        case 1:
            chassis->resetPid("前后pid");
            pros::delay(100);
            pidData[0] = chassis->forwardPid(QLength(pidTestTarget * millimeter));
            std::cout << std::endl;
            pros::delay(1000);
            pidData[1] = chassis->forwardPid(-QLength(pidTestTarget * millimeter));
            break;
        case 2:
            chassis->resetPid("左右pid");
            pros::delay(100);
            pidData[0] = chassis->rotatePid(QAngle(pidTestTarget * degree));
            std::cout << std::endl;
            pros::delay(1000);
            pidData[1] = chassis->rotatePid(-QAngle(pidTestTarget * degree));
            break;
        case 3:
            chassis->resetPid("自瞄pid");
            pros::delay(100);
            //      pidData[0] = chassis->rotateAutoAiming(pidTestTarget);
            std::cout << std::endl;
            pros::delay(1000);
            //pidData[1] = chassis->rotateAutoAiming(-pidTestTarget);
            break;
        case 4:
            pros::delay(100);
            for (int i = 0; i < 4; i++)
            {
                chassis->forwardPid(matSize);
                chassis->rotatePid(90_deg);
            }
            pros::delay(2000);
            for (int i = 0; i < 4; i++)
            {
                chassis->forwardPid(matSize);
                chassis->rotatePid(-90_deg);
            }
            break;
        case 5:
            pros::delay(100);
            chassis->setState(matSize * 1.5, matSize * 2.5, 0_deg);
            chassis->driveToPoint(matSize * 2.5, matSize * 2.5);
            chassis->driveToPoint(matSize * 2.5, matSize * 1.5);
            chassis->driveToPoint(matSize * 1.5, matSize * 1.5);
            chassis->driveToPoint(matSize * 1.5, matSize * 2.5);
            chassis->driveToAngle(0_deg);
            pros::delay(2000);
            chassis->driveToPoint(matSize * 2.5, matSize * 2.5);
            chassis->driveToPoint(matSize * 2.5, matSize * 3.5);
            chassis->driveToPoint(matSize * 1.5, matSize * 3.5);
            chassis->driveToPoint(matSize * 1.5, matSize * 2.5);
            chassis->driveToAngle(0_deg);
            break;
        case 6:
            joy1->rumble(". . .");
            pros::delay(3000);
            autonomous();
            break;
        case 7:
            joy1->rumble(". . .");
            pros::delay(3000);
            customTest();
            break;
        default:
            logger->warning({"变量:test 数值错误!"});
            break;
    }
}

} // namespace ncrapi