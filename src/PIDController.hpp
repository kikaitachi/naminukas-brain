#ifndef NAMINUKAS_BRAIN_PID_CONTROLLER_H_
#define NAMINUKAS_BRAIN_PID_CONTROLLER_H_

#include <list>

class PIDController {
  public:
    PIDController(float p, float i, float d, int max_prev_errors, float min, float max);
    void reset();
    float input(float value, float setpoint);

    /**
     * Measure of how good controller is.
     *
     * @return Non-negative value. The smaller the better.
     */
    float get_fitness();

  private:
    float p, i, d;
    float min, max;
    float prev_error;
    float cum_error;
    int max_prev_errors;
    std::list<float> prev_errors;
    float fitness;
    std::list<float> prev_errors_squared;
};

#endif // NAMINUKAS_BRAIN_PID_CONTROLLER_H_
