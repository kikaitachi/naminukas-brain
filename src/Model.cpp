#include "Model.hpp"

Model::Model(telemetry::Items& telemetryItems, hardware::Kinematics& kinematics) {
  std::shared_ptr<telemetry::ItemString> parts = std::make_shared<telemetry::ItemString>(telemetry::ROOT_ITEM_ID, "Parts", "2");
  telemetryItems.add_item(parts);

  const char* model_dir_env = std::getenv("MODEL_DIR");
  std::string model_dir = model_dir_env == NULL ? "../model/" : model_dir_env;

  left_foot = std::make_shared<telemetry::Item3DModel>(parts->getId(),
    "Left foot", "model/stl", model_dir + "suction-cup-connector.stl",
    0xDC143C, left_foot_transforms());
  telemetryItems.add_item(left_foot);

  right_foot = std::make_shared<telemetry::Item3DModelRef>(parts->getId(),
    "Right foot", left_foot->getId(),
    0x1E90FF, right_foot_transforms());
  telemetryItems.add_item(right_foot);

  reset();

  kinematics.add_position_listener([&] (auto joints) {
    update_joints(joints);
  });
}

void Model::update_joints(std::vector<hardware::JointPosition> joints) {
  for (auto& joint: joints) {
    switch (joint.joint) {
      case hardware::Joint::left_wheel:
        left_wheel_pos = joint.degrees;
        break;
      case hardware::Joint::right_wheel:
        right_wheel_pos = joint.degrees;
        break;
    }
  }
  left_foot->update(left_foot_transforms());
  right_foot->update(right_foot_transforms());
}

void Model::reset() {
  left_wheel_pos, right_wheel_pos = 0;
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

void Model::rotate(double delta_x, double delta_y, double delta_z) {
  rot_x += delta_x;
  rot_y += delta_y;
  rot_z += delta_z;
  left_foot->update(left_foot_transforms());
  right_foot->update(right_foot_transforms());
}

std::vector<Transform> Model::left_foot_transforms() {
  return {
    { TRANSFORM_TYPE_ROTATE, 0, -90 },
    { TRANSFORM_TYPE_ROTATE, 2, left_wheel_pos },
    { TRANSFORM_TYPE_MOVE, 0, -75 + pos_x },
    { TRANSFORM_TYPE_MOVE, 1, pos_y },
    { TRANSFORM_TYPE_MOVE, 2, 20 + pos_z },
    { TRANSFORM_TYPE_ROTATE, 0, rot_x },
    { TRANSFORM_TYPE_ROTATE, 1, rot_y },
    { TRANSFORM_TYPE_ROTATE, 2, rot_z }
  };
}

std::vector<Transform> Model::right_foot_transforms() {
  return {
    { TRANSFORM_TYPE_ROTATE, 0, -90 },
    { TRANSFORM_TYPE_ROTATE, 2, right_wheel_pos },
    { TRANSFORM_TYPE_MOVE, 0, 75 + pos_x },
    { TRANSFORM_TYPE_MOVE, 1, pos_y },
    { TRANSFORM_TYPE_MOVE, 2, 20 + pos_z },
    { TRANSFORM_TYPE_ROTATE, 0, rot_x },
    { TRANSFORM_TYPE_ROTATE, 1, rot_y },
    { TRANSFORM_TYPE_ROTATE, 2, rot_z }
  };
}
