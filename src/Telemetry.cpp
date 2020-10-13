#include "Message.hpp"
#include "Telemetry.hpp"

namespace telemetry {

  int Item::next_id = 1;

  Item::Item(int parent_id, int type): parent_id(parent_id), type() {
    this->id = next_id;
    next_id++;
  }

  void Item::serialize_definition(void **buf, int *buf_len) {
    //message::write_int(buf, buf_len, mesagge::TELEMETRY_DEFINITION);
    message::write_int(buf, buf_len, id);
    message::write_int(buf, buf_len, parent_id);
    message::write_int(buf, buf_len, type);
    message::write_data(buf, buf_len, name.c_str(), name.length());
  }

  ItemInt::ItemInt(int parent_id, int type, int value) :
    Item(parent_id, type), value(value) { }

  void ItemInt::serialize_value(void **buf, int *buf_len) {
    message::write_int(buf, buf_len, value);
  }

  ItemString::ItemString(int parent_id, int type, int string) :
    Item(parent_id, type), value(value) { }

  void ItemString::serialize_value(void **buf, int *buf_len) {
    message::write_data(buf, buf_len, value.c_str(), value.length());
  }

  int Items::add(Item* item) {
    id_to_item[item->id] = item;
  }
}
