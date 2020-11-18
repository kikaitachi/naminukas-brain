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
};

#endif  // NAMINUKAS_BRAIN_MODEL_H_
