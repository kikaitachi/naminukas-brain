#include <fstream>

#include "Logger.hpp"
#include "Message.hpp"
#include "Telemetry.hpp"

namespace telemetry {

  // Item **********************************************************************

  int Item::next_id = ROOT_ITEM_ID + 1;

  Item::Item(int parent_id, int type, std::string name) :
      parent_id(parent_id), type(type), name(name) {
    this->id = next_id;
    next_id++;
  }

  void Item::serialize_definition(void **buf, int *buf_len) {
    message::write_signed_integer(buf, buf_len, id);
    message::write_signed_integer(buf, buf_len, parent_id);
    message::write_signed_integer(buf, buf_len, type);
    message::write_string(buf, buf_len, name);
  }

  void Item::serialize_value(void **buf, int *buf_len) {
    //message::write_int(buf, buf_len, id);
  }

  void Item::deserialize_value(void **buf, int *buf_len) {
  }

  void Item::add_change_listener(std::function<void(Item&)> change_listener) {
    change_listeners.push_back(change_listener);
  }

  int Item::getId() {
    return id;
  }

  int Item::getType() {
    return type;
  }

  // ItemInt *******************************************************************

  ItemInt::ItemInt(int parent_id, std::string name, int64_t value) :
    Item(parent_id, TYPE_INT, name), value(value) { }

  void ItemInt::serialize_definition(void **buf, int *buf_len) {
    Item::serialize_definition(buf, buf_len);
    serialize_value(buf, buf_len);
  }

  void ItemInt::serialize_value(void **buf, int *buf_len) {
    Item::serialize_value(buf, buf_len);
    message::write_signed_integer(buf, buf_len, value);
  }

  void ItemInt::update(int64_t value) {
    if (this->value != value) {
      this->value = value;
      for (auto change_listener : change_listeners) {
        change_listener(*this);
      }
    }
  }

  // ItemString ****************************************************************

  ItemString::ItemString(int parent_id, std::string name, std::string value) :
      Item(parent_id, TYPE_STRING, name), value(value) {
    }

  void ItemString::serialize_definition(void **buf, int *buf_len) {
    Item::serialize_definition(buf, buf_len);
    serialize_value(buf, buf_len);
  }

  void ItemString::serialize_value(void **buf, int *buf_len) {
    Item::serialize_value(buf, buf_len);
    message::write_string(buf, buf_len, value);
  }

  void ItemString::update(std::string value) {
    if (this->value != value) {
      this->value = value;
      for (auto change_listener : change_listeners) {
        change_listener(*this);
      }
    }
  }

  // ItemCommand ***************************************************************

  ItemCommand::ItemCommand(int parent_id, std::string name, std::string value, std::function<void(int value)> action, std::vector<std::string> modifiers) :
      Item(parent_id, TYPE_ACTION, name), value(value), action(action), modifiers(modifiers) {
  }

  void ItemCommand::serialize_definition(void **buf, int *buf_len) {
    Item::serialize_definition(buf, buf_len);
    message::write_string(buf, buf_len, value);
    for (auto modifier : modifiers) {
      message::write_string(buf, buf_len, modifier);
    }
  }

  void ItemCommand::deserialize_value(void **buf, int *buf_len) {
    int value;
    message::read_int(buf, buf_len, &value);
    this->action(value);
  }

  // ItemSTL *******************************************************************

  ItemSTL::ItemSTL(int parent_id, std::string name, std::string file_name, int color, std::vector<Transform> transforms) :
      Item(parent_id, TYPE_STL, name), color(color), file_name(file_name),
      transforms(transforms) {
  }

  void ItemSTL::serialize_definition(void **buf, int *buf_len) {
    Item::serialize_definition(buf, buf_len);
    std::ifstream file(file_name, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    logger::debug("Loading STL model %s (%d bytes)", file_name.c_str(), size);
    message::write_unsigned_integer(buf, buf_len, size);
    file.seekg(0, std::ios::beg);
    file.read((char *)*buf, size);
    *buf = ((char *)*buf) + size;
    *buf_len -= size;
     message::write_unsigned_integer(buf, buf_len, color);
    serialize_transforms(buf, buf_len);
  }

  void ItemSTL::serialize_value(void **buf, int *buf_len) {
    Item::serialize_value(buf, buf_len);
    serialize_transforms(buf, buf_len);
  }

  void ItemSTL::update(std::vector<Transform> transforms) {
    this->transforms = transforms;
    for (auto change_listener : change_listeners) {
      change_listener(*this);
    }
  }

  void ItemSTL::serialize_transforms(void **buf, int *buf_len) {
    for (auto& transform : transforms) {
      message::write_signed_integer(buf, buf_len, transform.type);
      message::write_signed_integer(buf, buf_len, transform.axis);
      message::write_double(buf, buf_len, transform.value);
    }
  }

  // ItemPointCloud ************************************************************

  ItemPoints::ItemPoints(int parent_id, std::string name, std::vector<ColoredPoint> points) :
      Item(parent_id, TYPE_POINTS, name), points(points) {
  }

  void ItemPoints::serialize_definition(void **buf, int *buf_len) {
    Item::serialize_definition(buf, buf_len);
    serialize_points(buf, buf_len);
  }

  void ItemPoints::serialize_value(void **buf, int *buf_len) {
    Item::serialize_value(buf, buf_len);
    serialize_points(buf, buf_len);
  }

  void ItemPoints::update(std::vector<ColoredPoint> points) {
    this->points = points;
    // TODO: think how to update point cloud efficiently
    /*for (auto change_listener : change_listeners) {
      change_listener(*this);
    }*/
  }

  void ItemPoints::serialize_points(void **buf, int *buf_len) {
    for (auto& point : points) {
      message::write_float(buf, buf_len, point.x);
      message::write_float(buf, buf_len, point.y);
      message::write_float(buf, buf_len, point.z);
      message::write_byte(buf, buf_len, point.r);
      message::write_byte(buf, buf_len, point.g);
      message::write_byte(buf, buf_len, point.b);
    }
  }

  // Items *********************************************************************

  Item* Items::add_item(Item* item) {
    id_to_item[item->getId()] = item;
    for (auto change_listener : change_listeners) {
      item->add_change_listener(change_listener);
    }
    return item;
  }

  void Items::add_change_listener(std::function<void(Item&)> change_listener) {
    change_listeners.push_back(change_listener);
    for (const auto & [id, item] : id_to_item) {
      item->add_change_listener(change_listener);
    }
  }
}
