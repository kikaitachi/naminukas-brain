#ifndef SRC_RCRADIO_PRU_SBUS_HPP_
#define SRC_RCRADIO_PRU_SBUS_HPP_

#include "../RCRadio.hpp"

class RCRadio_PRU_SBUS : public RCRadio {
 public:
  void init(RCRadioChannelChangedHandler& change_handler);
};

#endif  // SRC_RCRADIO_PRU_SBUS_HPP_
