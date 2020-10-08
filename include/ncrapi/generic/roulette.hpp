#pragma once
#include "ncrapi/device/adi.hpp"
#include "shooter.hpp"

namespace ncrapi {

class Roulette : public Generic
{

  public:
    /**
   * @brief 吸吐类构造函数
   * 
   * @param pragma JSON参数
   * @param shooter 链接的发射器模块指针
   */
    Roulette(const json &pragma, std::shared_ptr<Generic> shooter = nullptr);
    virtual void joyControl(std::shared_ptr<pros::Controller> joy, pros::controller_digital_e_t up, pros::controller_digital_e_t down) override;
    virtual void joyControl(std::shared_ptr<pros::Controller> joy, pros::controller_digital_e_t up, pros::controller_digital_e_t down, pros::controller_digital_e_t eatShit);

  protected:
  private:
    std::shared_ptr<Generic> _shooter = nullptr; //发射系统
    Timer _joyTimer;
};
} // namespace ncrapi
