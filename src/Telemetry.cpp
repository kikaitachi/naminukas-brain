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
    message::write_int(buf, buf_len, id);
    message::write_int(buf, buf_len, parent_id);
    message::write_int(buf, buf_len, type);
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

  ItemInt::ItemInt(int parent_id, std::string name, long long value) :
    Item(parent_id, TYPE_INT, name), value(value) { }

  void ItemInt::serialize_definition(void **buf, int *buf_len) {
    Item::serialize_definition(buf, buf_len);
    serialize_value(buf, buf_len);
  }

  void ItemInt::serialize_value(void **buf, int *buf_len) {
    Item::serialize_value(buf, buf_len);
    message::write_int(buf, buf_len, value);
  }

  void ItemInt::update(long long value) {
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

  ItemCommand::ItemCommand(int parent_id, std::string name, std::string value, std::function<void(int value)> action) :
      Item(parent_id, TYPE_ACTION, name), value(value), action(action) {
  }

  void ItemCommand::serialize_definition(void **buf, int *buf_len) {
    Item::serialize_definition(buf, buf_len);
    message::write_string(buf, buf_len, value);
  }

  void ItemCommand::deserialize_value(void **buf, int *buf_len) {
    int value;
    message::read_int(buf, buf_len, &value);
    this->action(value);
  }

  // ItemSTL *******************************************************************

  ItemSTL::ItemSTL(int parent_id, std::string name, std::string file_name,
        double rot_x, double rot_y, double rot_z,
        double pos_x, double pos_y, double pos_z) :
      Item(parent_id, TYPE_STL, name), file_name(file_name),
      rot_x(rot_x), rot_y(rot_y), rot_z(rot_z),
      pos_x(pos_x), pos_y(pos_y), pos_z(pos_z) {
  }

  void ItemSTL::serialize_definition(void **buf, int *buf_len) {
    Item::serialize_definition(buf, buf_len);
    std::ifstream file(file_name, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    logger::debug("Loading STL model %s (%d bytes)", file_name.c_str(), size);
    message::write_int(buf, buf_len, size);
    file.seekg(0, std::ios::beg);
    file.read((char *)*buf, size);
    *buf = ((char *)*buf) + size;
    *buf_len -= size;
    message::write_double(buf, buf_len, rot_x);
    message::write_double(buf, buf_len, rot_y);
    message::write_double(buf, buf_len, rot_z);
    message::write_double(buf, buf_len, pos_x);
    message::write_double(buf, buf_len, pos_y);
    message::write_double(buf, buf_len, pos_z);
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
