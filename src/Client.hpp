#ifndef SRC_CLIENT_HPP_
#define SRC_CLIENT_HPP_

#include <unordered_set>

class Client {
 public:
  int fd;
  std::unordered_set<int> changed_telemetry_item_ids;

  explicit Client(int fd);
};

#endif  // SRC_CLIENT_HPP_
