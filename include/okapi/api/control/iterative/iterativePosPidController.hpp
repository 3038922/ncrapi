/**
 * Based on the Arduino PID controller: https://github.com/br3ttb/Arduino-PID-Library
 *
 * @author Ryan Benasutti, WPI
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "okapi/api/control/iterative/iterativePositionController.hpp"
#include "okapi/api/control/util/settledUtil.hpp"
#include "okapi/api/filter/filter.hpp"
#include "okapi/api/filter/passthroughFilter.hpp"
#include "okapi/api/util/logging.hpp"
#include "okapi/api/util/timeUtil.hpp"
#include <limits>
#include <memory>

namespace okapi {
class IterativePosPIDController : public IterativePositionController<double, double> {
  public:
  struct Gains {
    double kP{0};
    double kI{0};
    double kD{0};
    double kBias{0};
  };

  /**
   * Position PID controller.
   *
   * @param ikP the proportional gain
   * @param ikI the integration gain
   * @param ikD the derivative gain
   * @param ikBias the controller bias
   * @param itimeUtil see TimeUtil docs
   * @param iderivativeFilter a filter for filtering the derivative term
   */
  IterativePosPIDController(
    double ikP,
    double ikI,
    double ikD,
    double ikBias,
    const TimeUtil &itimeUtil,
    std::unique_ptr<Filter> iderivativeFilter = std::make_unique<PassthroughFilter>());

  /**
   * Position PID controller.
   *
   * @param igains the controller gains
   * @param itimeUtil see TimeUtil docs
   * @param iderivativeFilter a filter for filtering the derivative term
   */
  IterativePosPIDController(
    const Gains &igains,
    const TimeUtil &itimeUtil,
    std::unique_ptr<Filter> iderivativeFilter = std::make_unique<PassthroughFilter>());

  /**
   * Do one iteration of the controller. Returns the reading in the range [-1, 1] unless the
   * bounds have been changed with setOutputLimits().
   *
   * @param inewReading new measurement
   * @return controller output
   */
  double step(double inewReading) override;

  /**
   * Sets the target for the controller.
   *
   * @param itarget new target position
   */
  void setTarget(double itarget) override;

  /**
   * Writes the value of the controller output. This method might be automatically called in another
   * thread by the controller. The range of input values is expected to be [-1, 1].
   *
   * @param ivalue the controller's output in the range [-1, 1]
   */
  void controllerSet(double ivalue) override;

  /**
   * Gets the last set target, or the default target if none was set.
   *
   * @return the last target
   */
  double getTarget() override;

  /**
   * Returns the last calculated output of the controller. Output is in the range [-1, 1]
   * unless the bounds have been changed with setOutputLimits().
   */
  double getOutput() const override;

  /**
   * Get the upper output bound.
   *
   * @return  the upper output bound
   */
  double getMaxOutput() override;

  /**
   * Get the lower output bound.
   *
   * @return the lower output bound
   */
  double getMinOutput() override;

  /**
   * Returns the last error of the controller.
   */
  double getError() const override;

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
   * Set controller gains.
   *
   * @param ikP proportional gain
   * @param ikI integral gain
   * @param ikD derivative gain
   * @param ikBias bias (constant offset added to the output)
   */
  virtual void setGains(double ikP, double ikI, double ikD, double ikBias = 0);

  /**
   * Set time between loops in ms.
   *
   * @param isampleTime time between loops
   */
  void setSampleTime(QTime isampleTime) override;

  /**
   * Set controller output bounds. Default bounds are [-1, 1].
   *
   * @param imax max output
   * @param imin min output
   */
  void setOutputLimits(double imax, double imin) override;

  /**
   * Set integrator bounds. Default bounds are [-1, 1].
   *
   * @param imax max integrator value
   * @param imin min integrator value
   */
  virtual void setIntegralLimits(double imax, double imin);

  /**
   * Set the error sum bounds. Default bounds are [0, std::numeric_limits<double>::max()]. Error
   * will only be added to the integral term when its absolute value is between these bounds of
   * either side of the target.
   *
   * @param imax max error value that will be summed
   * @param imin min error value that will be summed
   */
  virtual void setErrorSumLimits(double imax, double imin);

  /**
   * Resets the controller's internal state so it is similar to when it was first initialized, while
   * keeping any user-configured information.
   */
  void reset() override;

  /**
   * Set whether the integrator should be reset when error is 0 or changes sign.
   *
   * @param iresetOnZero true to reset
   */
  virtual void setIntegratorReset(bool iresetOnZero);

  /**
   * Changes whether the controller is off or on. Turning the controller on after it was off will
   * cause the controller to move to its last set target, unless it was reset in that time.
   */
  void flipDisable() override;

  /**
   * Sets whether the controller is off or on. Turning the controller on after it was off will
   * cause the controller to move to its last set target, unless it was reset in that time.
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
   * Get the last set sample time.
   *
   * @return sample time
   */
  QTime getSampleTime() const override;

  protected:
  Logger *logger;
  double kP, kI, kD, kBias;
  QTime sampleTime{10_ms};
  double target{0};
  double lastReading{0};
  double error{0};
  double lastError{0};
  std::unique_ptr<Filter> derivativeFilter;

  // Integral bounds
  double integral{0};
  double integralMax{1};
  double integralMin{-1};

  // Error will only be added to the integral term within these bounds on either side of the target
  double errorSumMin{0};
  double errorSumMax{std::numeric_limits<double>::max()};

  double derivative{0};

  // Output bounds
  double output{0};
  double outputMax{1};
  double outputMin{-1};

  // Reset the integrated when the controller crosses 0 or not
  bool shouldResetOnCross{true};

  bool controllerIsDisabled{false};

  std::unique_ptr<AbstractTimer> loopDtTimer;
  std::unique_ptr<SettledUtil> settledUtil;
};
} // namespace okapi
