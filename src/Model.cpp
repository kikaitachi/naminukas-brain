#include "Model.hpp"

Model::Model(telemetry::Items& telemetryItems) {
  telemetry::ItemString* parts = new telemetry::ItemString(telemetry::ROOT_ITEM_ID, "Parts", "2");
  telemetryItems.add_item(parts);

  telemetry::ItemSTL* left_foot = new telemetry::ItemSTL(parts->getId(),
    "Left foot", "../model/suction-cup-connector.stl",
    0xDC143C,
    { { TRANSFORM_TYPE_ROTATE, 0, -90 }, { TRANSFORM_TYPE_MOVE, 0, -75 }, { TRANSFORM_TYPE_MOVE, 2, 20 } });
  telemetryItems.add_item(left_foot);

  telemetry::ItemSTL* right_foot = new telemetry::ItemSTL(parts->getId(),
    "Right foot", "../model/suction-cup-connector.stl",
    0x1E90FF,
    { { TRANSFORM_TYPE_ROTATE, 0, -90 }, { TRANSFORM_TYPE_MOVE, 0, 75 }, { TRANSFORM_TYPE_MOVE, 2, 20 } });
  telemetryItems.add_item(right_foot);
}

void Model::reset() {
  // TODO: implement
}
