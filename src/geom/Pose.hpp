#ifndef NAMINUKAS_BRAIN_GEOM_POSE_H_
#define NAMINUKAS_BRAIN_GEOM_POSE_H_

#include "Orientation.hpp"
#include "Point3D.hpp"

class Pose {
  public:
    Point3D location;
    Orientation orientation;
};

#endif // NAMINUKAS_BRAIN_GEOM_POSE_H_
