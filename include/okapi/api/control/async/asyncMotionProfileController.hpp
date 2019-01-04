/**
 * @author Ryan Benasutti, WPI
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "okapi/api/chassis/controller/chassisScales.hpp"
#include "okapi/api/chassis/model/skidSteerModel.hpp"
#include "okapi/api/control/async/asyncPositionController.hpp"
#include "okapi/api/units/QAngle.hpp"
#include "okapi/api/units/QAngularSpeed.hpp"
#include "okapi/api/units/QLength.hpp"
#include "okapi/api/units/QSpeed.hpp"
#include "okapi/api/util/logging.hpp"
#include "okapi/api/util/timeUtil.hpp"
#include <atomic>
#include <map>

extern "C" {
#include "okapi/pathfinder/include/pathfinder.h"
}

namespace okapi {
struct Point {
  QLength x;    // X coordinate relative to the start of the movement
  QLength y;    // Y coordinate relative to the start of the movement
  QAngle theta; // Exit angle relative to the start of the movement
};

class AsyncMotionProfileController : public AsyncPositionController<std::string, Point> {
  public:
  /**
   * An Async Controller which generates and follows 2D motion profiles. Throws a
   * std::invalid_argument exception if the gear ratio is zero.
   *
   * @param imaxVel The maximum possible velocity in m/s.
   * @param imaxAccel The maximum possible acceleration in m/s/s.
   * @param imaxJerk The maximum possible jerk in m/s/s/s.
   * @param imodel The chassis model to control.
   * @param iwidth The chassis wheelbase width.
   */
  AsyncMotionProfileController(const TimeUtil &itimeUtil,
                               double imaxVel,
                               double imaxAccel,
                               double imaxJerk,
                               const std::shared_ptr<ChassisModel> &imodel,
                               const ChassisScales &iscales,
                               AbstractMotor::GearsetRatioPair ipair);

  AsyncMotionProfileController(AsyncMotionProfileController &&other) noexcept;

  ~AsyncMotionProfileController() override;

  /**
   * Generates a path which intersects the given waypoints and saves it internally with a key of
   * pathId. Call executePath() with the same pathId to run it.
   *
   * If the waypoints form a path which is impossible to achieve, an instance of std::runtime_error
   * is thrown (and an error is logged) which describes the waypoints. If there are no waypoints,
   * no path is generated.
   *
   * @param iwaypoints The waypoints to hit on the path.
   * @param ipathId A unique identifier to save the path with.
   */
  void generatePath(std::initializer_list<Point> iwaypoints, const std::string &ipathId);

  /**
   * Removes a path and frees the memory it used.
   *
   * @param ipathId A unique identifier for the path, previously passed to generatePath()
   */
  void removePath(const std::string &ipathId);

  /**
   * Gets the identifiers of all paths saved in this AsyncMotionProfileController.
   *
   * @return The identifiers of all paths
   */
  std::vector<std::string> getPaths();

  /**
   * Executes a path with the given ID. If there is no path matching the ID, the method will
   * return. Any targets set while a path is being followed will be ignored.
   *
   * @param ipathId A unique identifier for the path, previously passed to generatePath().
   */
  void setTarget(std::string ipathId) override;

  /**
   * Executes a path with the given ID. If there is no path matching the ID, the method will
   * return. Any targets set while a path is being followed will be ignored.
   *
   * @param ipathId A unique identifier for the path, previously passed to generatePath().
   * @param ibackwards Whether to follow the profile backwards.
   */
  void setTarget(std::string ipathId, bool ibackwards);

  /**
   * Writes the value of the controller output. This method might be automatically called in another
   * thread by the controller. This just calls setTarget().
   */
  void controllerSet(std::string ivalue) override;

  /**
   * Gets the last set target, or the default target if none was set.
   *
   * @return the last target
   */
  std::string getTarget() override;

  /**
   * Blocks the current task until the controller has settled. This controller is settled when
   * it has finished following a path. If no path is being followed, it is settled.
   */
  void waitUntilSettled() override;

  /**
   * Generates a new path from the position (typically the current position) to the target and
   * blocks until the controller has settled. Does not save the path which was generated.
   *
   * @param iwaypoints The waypoints to hit on the path.
   */
  void moveTo(std::initializer_list<Point> iwaypoints);

  /**
   * Returns the last error of the controller. This implementation always returns zero since the
   * robot is assumed to perfectly follow the path. Subclasses can override this to be more
   * accurate using odometry information.
   *
   * @return the last error
   */
  Point getError() const override;

  /**
   * Returns whether the controller has settled at the target. Determining what settling means is
   * implementation-dependent.
   *
   * If the controller is disabled, this method must return true.
   *
   * @return whether the controller is settled
   */
  bool isSettled() override;

  /**
   * Resets the controller so it can start from 0 again properly. Keeps configuration from
   * before. This implementation also stops movement.
   */
  void reset() override;

  /**
   * Changes whether the controller is off or on. Turning the controller on after it was off will
   * NOT cause the controller to move to its last set target.
   */
  void flipDisable() override;

  /**
   * Sets whether the controller is off or on. Turning the controller on after it was off will
   * NOT cause the controller to move to its last set target, unless it was reset in that time.
   *
   * @param iisDisabled whether the controller is disabled
   */
  void flipDisable(bool iisDisabled) override;

  /**
   * Returns whether the controller is currently disabled.
   *
   * @return whether the controller is currently disabled
   */
  bool isDisabled() const override;

  /**
   * Starts the internal thread. This should not be called by normal users. This method is called
   * by the AsyncControllerFactory when making a new instance of this class.
   */
  void startThread();

  protected:
  struct TrajectoryPair {
    Segment *left;
    Segment *right;
    int length;
  };

  Logger *logger;
  std::map<std::string, TrajectoryPair> paths{};
  double maxVel{0};
  double maxAccel{0};
  double maxJerk{0};
  std::shared_ptr<ChassisModel> model;
  ChassisScales scales;
  AbstractMotor::GearsetRatioPair pair;
  TimeUtil timeUtil;

  std::string currentPath{""};
  std::atomic_bool isRunning{false};
  std::atomic_int direction{1};
  std::atomic_bool disabled{false};
  std::atomic_bool dtorCalled{false};
  CrossplatformThread *task{nullptr};

  static void trampoline(void *context);
  void loop();

  /**
   * Follow the supplied path. Must follow the disabled lifecycle.
   */
  virtual void executeSinglePath(const TrajectoryPair &path, std::unique_ptr<AbstractRate> rate);

  /**
   * Converts linear chassis speed to rotational motor speed.
   *
   * @param linear chassis frame speed
   * @return motor frame speed
   */
  QAngularSpeed convertLinearToRotational(QSpeed linear) const;
};
} // namespace okapi
