#ifndef SRC_PIDCONTROLLER_HPP_
#define SRC_PIDCONTROLLER_HPP_

#include <list>

#define MAX_FITNESS_ERRORS 200

template<class T>
class PIDController {
 public:
  PIDController(T p, T i, T d, int max_prev_errors, T min_output, T max_output)
    : p(p), i(i), d(i), max_prev_errors(max_prev_errors), min_output(min_output), max_output(max_output) {
  }

  void reset() {
    prev_error = cum_error = 0;
    prev_errors.clear();
    fitness = 0;
    prev_errors_squared.clear();
  }

  T input(T value, T setpoint) {
    T error = setpoint - value;
    cum_error += error;
    prev_errors.push_back(error);
    if (prev_errors.size() > max_prev_errors) {
      cum_error -= prev_errors.front();
      prev_errors.pop_front();
    }
    T result = error * p + (error - prev_error) * d + cum_error * i;
    prev_error = error;

    T error_squared = error * error;
    fitness += error_squared;
    prev_errors_squared.push_back(error_squared);
    if (prev_errors_squared.size() > MAX_FITNESS_ERRORS) {
      fitness -= prev_errors_squared.front();
      prev_errors_squared.pop_front();
    }

    if (result < min_output) {
      return min_output;
    }
    if (result > max_output) {
      return max_output;
    }
    return result;
  }

  /**
   * Measure of how good controller is.
   *
   * @return Non-negative value. The smaller the better.
   */
  T get_fitness() {
    return fitness;
  }

 private:
  T p, i, d;
  T min_output, max_output;
  T prev_error;
  T cum_error;
  int max_prev_errors;
  std::list<T> prev_errors;
  T fitness;
  std::list<T> prev_errors_squared;
};

#endif  // SRC_PIDCONTROLLER_HPP_
