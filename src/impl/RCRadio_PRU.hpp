#ifndef SRC_RCRADIO_PRU_HPP_
#define SRC_RCRADIO_PRU_HPP_

#include "../RCRadio.hpp"

class RCRadio_PRU : public RCRadio {
 public:
  virtual void init(RCRadioChannelChangedHandler& change_handler);
  virtual int channel_min_value();
  virtual int channel_max_value();
};

#endif  // SRC_RCRADIO_PRU_HPP_
