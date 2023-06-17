#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/hal.h"
//#include "esphome/core/time.h"
#include "esphome/components/i2c/i2c.h"

#include <vector>

#ifdef USE_TIME
#include "esphome/components/time/real_time_clock.h"
#endif

namespace esphome {
namespace ht16k33v110 {

class HT16K33V110Display;

using ht16k33v110_writer_t = std::function<void(HT16K33V110Display &)>;

class HT16K33V110Display : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_writer(ht16k33v110_writer_t &&writer) { this->writer_ = writer; }

  void setup() override;

  void dump_config() override;

  float get_setup_priority() const override;

  void update() override;

  /// Evaluate the printf-format and print the result at the given position.
  uint8_t printf(uint8_t pos, const char *format, ...) __attribute__((format(printf, 3, 4)));
  /// Evaluate the printf-format and print the result at position 0.
  uint8_t printf(const char *format, ...) __attribute__((format(printf, 2, 3)));

  /// Print `str` at the given position.
  uint8_t print(uint8_t pos, const char *str);
  /// Print `str` at position 0.
  uint8_t print(const char *str);

  void set_intensity(uint8_t intensity) { this->intensity_ = intensity; }
  void set_inverted(bool inverted) { this->inverted_ = inverted; }

  void set_intensity_auto() { this->auto_intensity_ = true;}
  void set_intensity_number_id(const char* auto_intensity_source) {this->auto_intensity_source_ = auto_intensity_source;}
  void calculate_new_intensity();
 
  void display();

#ifdef USE_TIME
  /// Evaluate the strftime-format and print the result at the given position.
  uint8_t strftime(uint8_t pos, const char *format, time::ESPTime time) __attribute__((format(strftime, 3, 0)));
  /// Evaluate the strftime-format and print the result at position 0.
  uint8_t strftime(const char *format, time::ESPTime time) __attribute__((format(strftime, 2, 0)));
#endif

 protected:
  void bit_delay_();
  void setup_pins_();
  bool send_byte_(uint8_t a_register, uint8_t value);
  uint8_t read_byte_(uint8_t a_register, uint8_t *value);
  void start_();
  void stop_();

  uint8_t intensity_;
  uint8_t length_ = 4;
  bool inverted_;
  bool colon_;
  optional<ht16k33v110_writer_t> writer_{};
  uint8_t buffer_[6] = {0};

  bool auto_intensity_ = false;
  const char* auto_intensity_source_;
};

}  // namespace tm1637
}  // namespace esphome
