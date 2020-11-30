#include "Model.hpp"

Model::Model(telemetry::Items& telemetryItems) {
  telemetry::ItemString* parts = new telemetry::ItemString(telemetry::ROOT_ITEM_ID, "Parts", "2");
  telemetryItems.add_item(parts);

  left_foot = new telemetry::ItemSTL(parts->getId(),
    "Left foot", "../model/suction-cup-connector.stl",
    0xDC143C, left_foot_transforms());
  telemetryItems.add_item(left_foot);

  right_foot = new telemetry::ItemSTL(parts->getId(),
    "Right foot", "../model/suction-cup-connector.stl",
    0x1E90FF, right_foot_transforms());
  telemetryItems.add_item(right_foot);

  reset();
}

Model::~Model() {
  delete left_foot;
  delete right_foot;
}

void Model::reset() {
  pos_x = pos_y = pos_z = 0;
  rot_x = rot_y = rot_z = 0;
  left_foot->update(left_foot_transforms());
  right_foot->update(right_foot_transforms());
}

void Model::move(double delta_x, double delta_y, double delta_z) {
  pos_x += delta_x;
  pos_y += delta_y;
  pos_z += delta_z;
  left_foot->update(left_foot_transforms());
  right_foot->update(right_foot_transforms());
}

std::vector<Transform> Model::left_foot_transforms() {
  return {
    { TRANSFORM_TYPE_ROTATE, 0, -90 },
    { TRANSFORM_TYPE_MOVE, 0, -75 + pos_x },
    { TRANSFORM_TYPE_MOVE, 1, pos_y },
    { TRANSFORM_TYPE_MOVE, 2, 20 + pos_z }
  };
}

std::vector<Transform> Model::right_foot_transforms() {
  return {
    { TRANSFORM_TYPE_ROTATE, 0, -90 },
    { TRANSFORM_TYPE_MOVE, 0, 75 + pos_x },
    { TRANSFORM_TYPE_MOVE, 1, pos_y },
    { TRANSFORM_TYPE_MOVE, 2, 20 + pos_z }
  };
}
