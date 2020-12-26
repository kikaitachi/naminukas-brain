#include "PIDController.hpp"

#define MAX_FITNESS_ERRORS 200

PIDController::PIDController(float p, float i, float d, int max_prev_errors, float min, float max)
    : p(p), i(i), d(i), max_prev_errors(max_prev_errors), min(min), max(max) {
}

void PIDController::reset() {
  prev_error = cum_error = 0;
  prev_errors.clear();
  fitness = 0;
  prev_errors_squared.clear();
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

  float error_squared = error * error;
  fitness += error_squared;
  prev_errors_squared.push_back(error_squared);
  if (prev_errors_squared.size() > MAX_FITNESS_ERRORS) {
    fitness -= prev_errors_squared.front();
    prev_errors_squared.pop_front();
  }

  if (result < min) {
    return min;
  }
  if (result > max) {
    return max;
  }
  return result;
}

float PIDController::get_fitness() {
  return fitness;
}
