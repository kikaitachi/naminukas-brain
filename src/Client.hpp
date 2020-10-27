#ifndef NAMINUKAS_BRAIN_CLIENT_H_
#define NAMINUKAS_BRAIN_CLIENT_H_

#include <unordered_set>

class Client {
  public:
    int fd;
    std::unordered_set<int> changed_telemetry_item_ids;

    Client(int fd);
};

#endif  // NAMINUKAS_BRAIN_CLIENT_H_
