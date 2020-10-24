#ifndef NAMINUKAS_BRAIN_TELEMETRY_H_
#define NAMINUKAS_BRAIN_TELEMETRY_H_

#include <functional>
#include <list>
#include <map>
#include <string>

namespace telemetry {
  #define TYPE_INT 0
  #define TYPE_STRING 1
  #define TYPE_ACTION 2
  #define TYPE_STL 3
  #define TYPE_RATIO 4

  const int ROOT_ITEM_ID = 0;

  class Item {
    public:
      Item(int parent_id, int type, std::string name);
      virtual void serialize_definition(void **buf, int *buf_len);
      virtual void serialize_value(void **buf, int *buf_len);
      virtual void deserialize_value(void **buf, int *buf_len);
      void add_change_listener(std::function<void(Item&)> listener);
      int getId();

    protected:
      int id, parent_id, type;
      std::string name;
      std::list<std::function<void(Item&)>> change_listeners;

    private:
      static int next_id;
  };

  class ItemInt: public Item {
    public:
      long long value;

      ItemInt(int parent_id, std::string name, long long value);
      void serialize_definition(void **buf, int *buf_len);
      void serialize_value(void **buf, int *buf_len);
      void update(long long value);
  };

  class ItemString: public Item {
    public:
      std::string value;

      ItemString(int parent_id, std::string name, std::string value);
      void serialize_definition(void **buf, int *buf_len);
      void serialize_value(void **buf, int *buf_len);
      void update(std::string value);
  };

  class Items {
    public:
      std::map<int, Item*> id_to_item;

      Item* add_item(Item* item);
      void add_change_listener(std::function<void(Item&)> listener);

    protected:
      std::list<std::function<void(Item&)>> change_listeners;
  };
}

#endif  // NAMINUKAS_BRAIN_TELEMETRY_H_
