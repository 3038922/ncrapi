#pragma once
#include "ncrapi/system/json.hpp"
#include "pros/motors.hpp"

namespace ncrapi {
class Motor : public pros::Motor
{
  public:
    Motor(const std::string &name, const int &port, const bool &isReverse, const int &gearset);
    Motor(const std::string &name, const json &pragma);

    /**
     *获取马达的名字     
     * @return std::string 返回马达的名字 
     */
    std::string getName();

    /**
     * @获取齿轮中文名字
     * 
     * @return std::string 返回齿轮的中文名字
     */
    std::string getGearName();
    /**
     *获取当前齿轮的速度 
     * @return int 范湖齿轮的速度 红齿轮100 绿齿轮200 蓝齿轮600
     */
    int getGearSpeed();
    virtual double getEncVal();

  private:
    void init(int gear);
    const std::string _name; //名字
    int _speed;
};
} // namespace ncrapi