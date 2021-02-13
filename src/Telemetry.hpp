#ifndef NAMINUKAS_BRAIN_TELEMETRY_H_
#define NAMINUKAS_BRAIN_TELEMETRY_H_

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Transform.hpp"

namespace telemetry {
  const int TYPE_INT = 0;
  const int TYPE_STRING = 1;
  const int TYPE_FLOAT = 2;
  const int TYPE_ACTION = 3;
  const int TYPE_3DMODEL = 4;
  const int TYPE_3DMODEL_REF = 5;
  const int TYPE_POINTS = 6;
  const int TYPE_CHOICE = 7;
  // TODO: introduce "reference" type to avoid sending duplicate STL files

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
      std::string name;

    protected:
      int id, parent_id, type;
      std::list<std::function<void(Item&)>> change_listeners;

    private:
      static int next_id;
  };

  class ItemInt: public Item {
    public:
      ItemInt(int parent_id, std::string name, int64_t value);
      void serialize_definition(void **buf, int *buf_len);
      void serialize_value(void **buf, int *buf_len);
      void update(int64_t value);

    private:
      int64_t value;
  };

  class ItemFloat: public Item {
    public:
      ItemFloat(int parent_id, std::string name, float value);
      void serialize_definition(void **buf, int *buf_len);
      void serialize_value(void **buf, int *buf_len);
      void update(float value);

    private:
      float value;
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
      ItemCommand(int parent_id, std::string name, std::string value, std::function<void(int, std::set<std::string>&)> action, std::vector<std::string> modifiers);
      void serialize_definition(void **buf, int *buf_len);
      void deserialize_value(void **buf, int *buf_len);

    private:
      std::string value;
      std::function<void(int, std::set<std::string>&)> action;
      std::vector<std::string> modifiers;
  };

  class Item3DModelBase : public Item {
   public:
    Item3DModelBase(int parent_id, int type, std::string name, uint32_t color, std::vector<Transform> transforms);
    void serialize_definition(void **buf, int *buf_len);
    void serialize_value(void **buf, int *buf_len);
    void update(std::vector<Transform> transforms);
   protected:
    uint32_t color;
    std::vector<Transform> transforms;
    void serialize_transforms(void **buf, int *buf_len);
  };

  class Item3DModel: public Item3DModelBase {
   public:
    Item3DModel(int parent_id, std::string name, std::string mime_type, std::string file_name, uint32_t color, std::vector<Transform> transforms);
    void serialize_definition(void **buf, int *buf_len);
   private:
    std::string mime_type;
    std::string file_name;
  };

  class Item3DModelRef: public Item3DModelBase {
   public:
    Item3DModelRef(int parent_id, std::string name, int ref_id, uint32_t color, std::vector<Transform> transforms);
    void serialize_definition(void **buf, int *buf_len);
   private:
    int ref_id;
  };

  class ColoredPoint {
    public:
      float x, y, z;
      uint8_t r, g, b;
  };

  class ItemPoints: public Item {
    public:
      ItemPoints(int parent_id, std::string name, std::vector<ColoredPoint> points);
      void serialize_definition(void **buf, int *buf_len);
      void serialize_value(void **buf, int *buf_len);
      void update(std::vector<ColoredPoint> points);

    private:
      std::vector<ColoredPoint> points;

      void serialize_points(void **buf, int *buf_len);
  };

  class ItemChoice: public Item {
    public:
      ItemChoice(int parent_id, std::string name, std::vector<std::string> choices, int selected, std::function<void(int)> on_change);
      void serialize_definition(void **buf, int *buf_len);
      void serialize_value(void **buf, int *buf_len);
      void deserialize_value(void **buf, int *buf_len);

    private:
      std::vector<std::string> choices;
      int selected;
      std::function<void(int)> on_change;
  };

  class Items {
    public:
      std::map<int, std::shared_ptr<Item>> id_to_item;
      std::vector<std::shared_ptr<Item>> items;

      void add_item(std::shared_ptr<Item> item);
      void add_change_listener(std::function<void(Item&)> listener);

    protected:
      std::list<std::function<void(Item&)>> change_listeners;
  };
}

#endif  // NAMINUKAS_BRAIN_TELEMETRY_H_
