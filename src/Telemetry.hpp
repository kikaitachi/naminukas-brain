#ifndef NAMINUKAS_BRAIN_TELEMETRY_H_
#define NAMINUKAS_BRAIN_TELEMETRY_H_

#include <map>
#include <string>

namespace telemetry {
  #define TYPE_INT 0
  #define TYPE_FLOAT 1
  #define TYPE_STRING 2
  #define TYPE_STL 3

  #define ROOT_ITEM_ID 0

  class Item {
    public:
      int id, parent_id, type;
      std::string name;

      static int next_id;

      Item(int parent_id, int type);
      virtual void serialize_definition(void **buf, int *buf_len);
      virtual void serialize_value(void **buf, int *buf_len) = 0;
  };

  class ItemInt: public Item {
    public:
      int value;

      ItemInt(int parent_id, int type, int value);
      void serialize_value(void **buf, int *buf_len);
  };

  class ItemString: public Item {
    public:
      std::string value;

      ItemString(int parent_id, int type, int string);
      void serialize_value(void **buf, int *buf_len);
  };

  class Items {
    public:
      int add(Item* item);

    private:
      std::map<int, Item*> id_to_item;
  };
}

#endif  // NAMINUKAS_BRAIN_TELEMETRY_H_
