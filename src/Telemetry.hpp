#ifndef NAMINUKAS_BRAIN_TELEMETRY_H_
#define NAMINUKAS_BRAIN_TELEMETRY_H_

#include <functional>
#include <list>
#include <map>
#include <string>

namespace telemetry {
  const int TYPE_INT = 0;
  const int TYPE_STRING = 1;
  const int TYPE_ACTION = 2;
  const int TYPE_STL = 3;

  const int ROOT_ITEM_ID = 0;

  class Item {
    public:
      Item(int parent_id, int type, std::string name);
      virtual void serialize_definition(void **buf, int *buf_len);
      virtual void serialize_value(void **buf, int *buf_len);
      virtual void deserialize_value(void **buf, int *buf_len);
      void add_change_listener(std::function<void(Item&)> listener);
      int getId();
      int getType();

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
      ItemString(int parent_id, std::string name, std::string value);
      void serialize_definition(void **buf, int *buf_len);
      void serialize_value(void **buf, int *buf_len);
      void update(std::string value);
    private:
      std::string value;
  };

  class ItemCommand: public Item {
    public:
      /**
       * Allowed values are key codes: https://w3c.github.io/uievents-code/
       */
      ItemCommand(int parent_id, std::string name, std::string value, std::function<void(int value)> action);
      void serialize_definition(void **buf, int *buf_len);
      void deserialize_value(void **buf, int *buf_len);
    private:
      std::string value;
      std::function<void(int)> action;
  };

  class ItemSTL: public Item {
    public:
      ItemSTL(int parent_id, std::string name, std::string file_name,
        double rot_x, double rot_y, double rot_z,
        double pos_x, double pos_y, double pos_z);
      void serialize_definition(void **buf, int *buf_len);
    private:
      std::string file_name;
      double rot_x, rot_y, rot_z;
      double pos_x, pos_y, pos_z;
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
