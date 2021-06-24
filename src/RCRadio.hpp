#ifndef SRC_RCRADIO_HPP_
#define SRC_RCRADIO_HPP_

#include <functional>

class RCRadioChannelChangedHandler {
 public:
  virtual void on_rc_radio_channel_change(int channel, int new_value) = 0;
};

class RCRadio {
 public:
  virtual void init(RCRadioChannelChangedHandler& change_handler) = 0;
  virtual int channel_min_value() = 0;
  virtual int channel_max_value() = 0;
};

#endif  // SRC_RCRADIO_HPP_
