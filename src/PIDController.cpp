#include "PIDController.hpp"

PIDController::PIDController(float p, float i, float d, int max_prev_errors, float min, float max)
    : p(p), i(i), d(i), max_prev_errors(max_prev_errors), min(min), max(max) {
}

void PIDController::reset() {
  prev_error = 0;
  cum_error = 0;
  prev_errors.clear();
}

float PIDController::input(float value, float setpoint) {
  float error = value - setpoint;
  cum_error += error;
  prev_errors.push_back(error);
  if (prev_errors.size() > max_prev_errors) {
    cum_error -= prev_errors.front();
    prev_errors.pop_front();
  }
  float result = error * p + (error - prev_error) * d + cum_error * i;
  prev_error = error;
  if (result < min) {
    return min;
  }
  if (result > max) {
    return max;
  }
  return result;
}
