#include "Message.hpp"
#include "Telemetry.hpp"

namespace telemetry {

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
    dirty = false;
  }

  bool Item::is_dirty() {
    return dirty;
  }

  ItemInt::ItemInt(int parent_id, std::string name, int value) :
    Item(parent_id, TYPE_INT, name), value(value) { }

  void ItemInt::serialize_value(void **buf, int *buf_len) {
    Item::serialize_value(buf, buf_len);
    message::write_int(buf, buf_len, value);
  }

  void ItemInt::update(int value) {
    this->value = value;
    dirty = true;
  }

  ItemString::ItemString(int parent_id, std::string name, std::string value) :
    Item(parent_id, TYPE_STRING, name), value(value) { }

  void ItemString::serialize_definition(void **buf, int *buf_len) {
    Item::serialize_definition(buf, buf_len);
    serialize_value(buf, buf_len);
  }

  void ItemString::serialize_value(void **buf, int *buf_len) {
    Item::serialize_value(buf, buf_len);
    message::write_string(buf, buf_len, value);
  }

  void ItemString::update(std::string value) {
    this->value = value;
    dirty = true;
  }

  int Items::add(Item* item) {
    id_to_item[item->id] = item;
    return item->id;
  }
}
