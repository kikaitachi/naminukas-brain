#ifndef SRC_RCRADIO_PRU_SBUS_HPP_
#define SRC_RCRADIO_PRU_SBUS_HPP_

#include "../RCRadio.hpp"

class RCRadio_PRU_SBUS : public RCRadio {
 public:
  virtual void init(RCRadioChannelChangedHandler& change_handler);
  virtual int channel_min_value();
  virtual int channel_max_value();
};

#endif  // SRC_RCRADIO_PRU_SBUS_HPP_
