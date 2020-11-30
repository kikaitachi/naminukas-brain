#ifndef NAMINUKAS_BRAIN_MODEL_H_
#define NAMINUKAS_BRAIN_MODEL_H_

#include "Telemetry.hpp"

class Model {
  public:
    Model(telemetry::Items& telemetryItems);

    /**
     * Set robot location and rotation of parts to the initial position.
     */
    void reset();

    void move(double delta_x, double delta_y, double delta_z);

  private:
    double pos_x, pos_y, pos_z;
    double rot_x, rot_y, rot_z;
};

#endif  // NAMINUKAS_BRAIN_MODEL_H_
