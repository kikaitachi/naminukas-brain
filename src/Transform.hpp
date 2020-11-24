#ifndef NAMINUKAS_BRAIN_TRANSFORM_H_
#define NAMINUKAS_BRAIN_TRANSFORM_H_

const int TRANSFORM_TYPE_ROTATE = 0;
const int TRANSFORM_TYPE_MOVE = 1;

class Transform {
  public:
    int type;
    int axis;
    float value;

    Transform(int type, int axis, float value);
};

#endif  // NAMINUKAS_BRAIN_TRANSFORM_H_
