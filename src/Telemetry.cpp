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

  ItemCommand::ItemCommand(int parent_id, std::string name, std::string value, std::function<void()> action) :
      Item(parent_id, TYPE_ACTION, name), value(value), action(action) {
  }

  void ItemCommand::serialize_definition(void **buf, int *buf_len) {
    Item::serialize_definition(buf, buf_len);
    message::write_string(buf, buf_len, value);
  }

  void ItemCommand::deserialize_value(void **buf, int *buf_len) {
    this->action();
  }

  // ItemSTL *******************************************************************

  ItemSTL::ItemSTL(int parent_id, std::string name, std::string file_name) :
      Item(parent_id, TYPE_STL, name), file_name(file_name) {
  }

  void ItemSTL::serialize_definition(void **buf, int *buf_len) {
    Item::serialize_definition(buf, buf_len);
    std::ifstream file(file_name, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    logger::debug("Loading STL model %s or %d", file_name, size);
    message::write_int(buf, buf_len, size);
    file.seekg(0, std::ios::beg);
    file.read((char *)*buf, size);
    *buf = ((char *)*buf) + size;
    *buf_len -= size;
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
