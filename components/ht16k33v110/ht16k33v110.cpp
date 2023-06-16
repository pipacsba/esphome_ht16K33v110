#include "ht16k33v110.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/application.h"

namespace esphome {
namespace ht16k33v110 {

static const char *const TAG = "display.ht16k33v110";
const uint8_t HT16K33V110_CHR0_ADDRESS = 0x00;
// display internal address of second character
const uint8_t HT16K33V110_CHR1_ADDRESS = 0x02;
// display internal address of third character
const uint8_t HT16K33V110_CHR2_ADDRESS = 0x06;
// display internal address of fourth character
const uint8_t HT16K33V110_CHR3_ADDRESS = 0x08;
// display internal address of colon character
const uint8_t HT16K33V110_COLON_ADDRESS = 0x04;
// define constant for maximum dimming value
const uint8_t HT16K33V110_MAX_DIMMING = 15;
// define constant for display on/off adress
const uint8_t HT16K33V110_DISP_ON = 0x81;
const uint8_t HT16K33V110_DISP_OFF = 0x80;
// define constant for display oscillator on/off adress
const uint8_t HT16K33V110_OSC_ON = 0x21;
const uint8_t HT16K33V110_OSC_OFF = 0x20;
    
const uint8_t HT16K33V110_UNKNOWN_CHAR = 0b1111111;
    
    
//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D   
// ABCDEFG
const uint8_t HT16K33V110_ASCII_TO_RAW[] PROGMEM = {
    0b0000000,                 // ' ', ord 0x20
    HT16K33V110_UNKNOWN_CHAR,  // '!', ord 0x21
    0b0100010,                 // '"', ord 0x22
    HT16K33V110_UNKNOWN_CHAR,  // '#', ord 0x23
    HT16K33V110_UNKNOWN_CHAR,  // '$', ord 0x24
    0b1001001,                 // '%', ord 0x25
    HT16K33V110_UNKNOWN_CHAR,  // '&', ord 0x26
    0b0000010,                 // ''', ord 0x27
    0b1001110,                 // '(', ord 0x28
    0b1111000,                 // ')', ord 0x29
    0b1000000,                 // '*', ord 0x2A
    HT16K33V110_UNKNOWN_CHAR,  // '+', ord 0x2B
    0b0010000,                 // ',', ord 0x2C
    0b0000001,                 // '-', ord 0x2D
    HT16K33V110_UNKNOWN_CHAR,  // '.', ord 0x2E
    HT16K33V110_UNKNOWN_CHAR,  // '/', ord 0x2F
    0b1111110,                 // '0', ord 0x30
    0b0110000,                 // '1', ord 0x31
    0b1101101,                 // '2', ord 0x32
    0b1111001,                 // '3', ord 0x33
    0b0110011,                 // '4', ord 0x34
    0b1011011,                 // '5', ord 0x35
    0b1011111,                 // '6', ord 0x36
    0b1110000,                 // '7', ord 0x37
    0b1111111,                 // '8', ord 0x38
    0b1111011,                 // '9', ord 0x39
    0b1001000,                 // ':', ord 0x3A
    0b1011000,                 // ';', ord 0x3B
    HT16K33V110_UNKNOWN_CHAR,  // '<', ord 0x3C
    HT16K33V110_UNKNOWN_CHAR,  // '=', ord 0x3D
    HT16K33V110_UNKNOWN_CHAR,  // '>', ord 0x3E
    0b1100101,                 // '?', ord 0x3F
    0b1101111,                 // '@', ord 0x40
    0b1110111,                 // 'A', ord 0x41
    0b0011111,                 // 'B', ord 0x42
    0b1001110,                 // 'C', ord 0x43
    0b0111101,                 // 'D', ord 0x44
    0b1001111,                 // 'E', ord 0x45
    0b1000111,                 // 'F', ord 0x46
    0b1011110,                 // 'G', ord 0x47
    0b0110111,                 // 'H', ord 0x48
    0b0110000,                 // 'I', ord 0x49
    0b0111100,                 // 'J', ord 0x4A
    HT16K33V110_UNKNOWN_CHAR,  // 'K', ord 0x4B
    0b0001110,                 // 'L', ord 0x4C
    HT16K33V110_UNKNOWN_CHAR,  // 'M', ord 0x4D
    0b0010101,                 // 'N', ord 0x4E
    0b1111110,                 // 'O', ord 0x4F
    0b1100111,                 // 'P', ord 0x50
    HT16K33V110_UNKNOWN_CHAR,  // 'Q', ord 0x51
    0b0000101,                 // 'R', ord 0x52
    0b1011011,                 // 'S', ord 0x53
    0b0000111,                 // 'T', ord 0x54
    0b0111110,                 // 'U', ord 0x55
    0b0111110,                 // 'V', ord 0x56
    0b0111111,                 // 'W', ord 0x57
    HT16K33V110_UNKNOWN_CHAR,  // 'X', ord 0x58
    0b0100111,                 // 'Y', ord 0x59
    0b1101101,                 // 'Z', ord 0x5A
    0b1001110,                 // '[', ord 0x5B
    HT16K33V110_UNKNOWN_CHAR,  // '\', ord 0x5C
    0b1111000,                 // ']', ord 0x5D
    HT16K33V110_UNKNOWN_CHAR,  // '^', ord 0x5E
    0b0001000,                 // '_', ord 0x5F
    0b0100000,                 // '`', ord 0x60
    0b1110111,                 // 'a', ord 0x61
    0b0011111,                 // 'b', ord 0x62
    0b0001101,                 // 'c', ord 0x63
    0b0111101,                 // 'd', ord 0x64
    0b1001111,                 // 'e', ord 0x65
    0b1000111,                 // 'f', ord 0x66
    0b1011110,                 // 'g', ord 0x67
    0b0010111,                 // 'h', ord 0x68
    0b0010000,                 // 'i', ord 0x69
    0b0111100,                 // 'j', ord 0x6A
    HT16K33V110_UNKNOWN_CHAR,  // 'k', ord 0x6B
    0b0001110,                 // 'l', ord 0x6C
    HT16K33V110_UNKNOWN_CHAR,  // 'm', ord 0x6D
    0b0010101,                 // 'n', ord 0x6E
    0b0011101,                 // 'o', ord 0x6F
    0b1100111,                 // 'p', ord 0x70
    HT16K33V110_UNKNOWN_CHAR,  // 'q', ord 0x71
    0b0000101,                 // 'r', ord 0x72
    0b1011011,                 // 's', ord 0x73
    0b0000111,                 // 't', ord 0x74
    0b0011100,                 // 'u', ord 0x75
    0b0011100,                 // 'v', ord 0x76
    HT16K33V110_UNKNOWN_CHAR,  // 'w', ord 0x77
    HT16K33V110_UNKNOWN_CHAR,  // 'x', ord 0x78
    0b0100111,                 // 'y', ord 0x79
    HT16K33V110_UNKNOWN_CHAR,  // 'z', ord 0x7A
    0b0110001,                 // '{', ord 0x7B
    0b0000110,                 // '|', ord 0x7C
    0b0000111,                 // '}', ord 0x7D
    0b1100011,                 // '~', ord 0x7E (degree symbol)
};
    

void HT16K33V110Display::setup() {
  ESP_LOGCONFIG(TAG, "Setting up HT16K33V110...");
  uint8_t data;
  this->read_byte_(HT16K33V110_OSC_ON, &data);
  this->read_byte_(HT16K33V110_DISP_ON, &data);
  this->display();
}

void HT16K33V110Display::dump_config() {
  ESP_LOGCONFIG(TAG, "TM1637:");
  ESP_LOGCONFIG(TAG, "  Intensity: %d", this->intensity_);
  ESP_LOGCONFIG(TAG, "  Inverted: %d", this->inverted_);
  ESP_LOGCONFIG(TAG, "  Length: %d", this->length_);
  LOG_UPDATE_INTERVAL(this);
}

void HT16K33V110Display::update() {
  for (uint8_t &i : this->buffer_)
    i = 0;
  if (this->writer_.has_value())
    (*this->writer_)(*this);
  this->display();
}

float HT16K33V110Display::get_setup_priority() const { return setup_priority::PROCESSOR; }
    
void HT16K33V110Display::bit_delay_() { delayMicroseconds(100); }
    
void HT16K33V110Display::start_() {
}

void HT16K33V110Display::stop_() {

}

void HT16K33V110Display::display() {
  ESP_LOGVV(TAG, "Display %02X%02X%02X%02X", buffer_[0], buffer_[1], buffer_[2], buffer_[3]);

  uint8_t segment_a = ((this->inverted_) ? this->buffer_[3] : this->buffer_[0]);
  uint8_t segment_b = ((this->inverted_) ? this->buffer_[2] : this->buffer_[1]);
  uint8_t segment_c = ((this->inverted_) ? this->buffer_[1] : this->buffer_[2]);
  uint8_t segment_d = ((this->inverted_) ? this->buffer_[0] : this->buffer_[3]);
  uint8_t colon = ((this->colon_) ? 0x02 : 0x0);
  uint8_t dimming = 0xE0 + this->intensity_;
  
  this->send_byte_(HT16K33V110_CHR0_ADDRESS, segment_a);
  this->send_byte_(HT16K33V110_CHR1_ADDRESS, segment_b);
  this->send_byte_(HT16K33V110_CHR2_ADDRESS, segment_c);
  this->send_byte_(HT16K33V110_CHR3_ADDRESS, segment_d);
  this->send_byte_(HT16K33V110_COLON_ADDRESS, colon);
  uint8_t a_value;
  this->read_byte_(dimming, &a_value);

  calculate_new_intensity();
    
}

void HT16K33V110Display::calculate_new_intensity()
{
  float hysteresis = 1.03; // percentage to switch to higher intensity
  if (this->auto_intensity_) {
    float a_sensor_value;
    for (sensor::Sensor *obj : App.get_sensors()) {
      if (obj->get_name().c_str() != this->auto_intensity_source_) {
        ESP_LOGW(TAG, "%s do not match %s", obj->get_name().c_str(), this->auto_intensity_source_);
        continue;
      }
      a_sensor_value = obj->state;
      ESP_LOGW(TAG, "Measured sensor value is %.1f.", a_sensor_value);
    }
    if (isnan(a_sensor_value)) { a_sensor_value = 0;}
    uint8_t a_dimming = 0;
    uint8_t i = 0;
    uint8_t vecSize = this->intensity_source_values_.size();
    for (uint8_t i = 0; i < vecSize; i++)
    {
        if (a_sensor_value > (this->intensity_source_values_[i] * hysteresis))
        {
            a_dimming = this->intensity_values_[i];
        }
    }
    set_intensity(a_dimming);
}
    
bool HT16K33V110Display::send_byte_(uint8_t a_register, uint8_t value) {
  return this->write_byte(a_register, value);
}

uint8_t HT16K33V110Display::read_byte_(uint8_t a_register, uint8_t *value) {
  return this->read_byte(a_register, value);
}

uint8_t HT16K33V110Display::print(uint8_t start_pos, const char *str) {
  // ESP_LOGV(TAG, "Print at %d: %s", start_pos, str);
  uint8_t pos = start_pos;
  bool colon = false;
  for (; *str != '\0'; str++) {
    uint8_t data = HT16K33V110_UNKNOWN_CHAR;
    if (*str >= ' ' && *str <= '~')
      data = progmem_read_byte(&HT16K33V110_ASCII_TO_RAW[*str - ' ']);

    if (data == HT16K33V110_UNKNOWN_CHAR) {
      ESP_LOGW(TAG, "Encountered character '%c' with no HT16K33V110 representation while translating string!", *str);
    }
    // Remap segments, for compatibility with MAX7219 segment definition which is
    // XABCDEFG, but HT16K33V110 is // XGFEDCBA
    if (this->inverted_) {
      // XABCDEFG > GCBAFED
      data = ((data & 0x40) ? 0x8 : 0) |   // A
             ((data & 0x20) ? 0x10 : 0) |  // B
             ((data & 0x10) ? 0x20 : 0) |  // C
             ((data & 0x8) ? 0x1 : 0) |    // D
             ((data & 0x4) ? 0x2 : 0) |    // E
             ((data & 0x2) ? 0x4 : 0) |    // F
             ((data & 0x1) ? 0x40 : 0);    // G
    } else {
      // XABCDEFG > GFEDCBA
      data = ((data & 0x40) ? 0x1 : 0) |   // A
             ((data & 0x20) ? 0x2 : 0) |   // B
             ((data & 0x10) ? 0x4 : 0) |   // C
             ((data & 0x8) ? 0x8 : 0) |    // D
             ((data & 0x4) ? 0x10 : 0) |   // E
             ((data & 0x2) ? 0x20 : 0) |   // F
             ((data & 0x1) ? 0x40 : 0);    // G
    }
    if (*str == ':') {
      if (pos != start_pos)
        pos--;
      colon = true;
    } else {
      if (pos >= 4) {
        ESP_LOGE(TAG, "String is too long for the display!");
        break;
      }
      this->buffer_[pos] = data;
    }
    pos++;
  }
  this->colon_ = colon;
  return pos - start_pos;
}
uint8_t HT16K33V110Display::print(const char *str) { return this->print(0, str); }
uint8_t HT16K33V110Display::printf(uint8_t pos, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}
uint8_t HT16K33V110Display::printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(buffer);
  return 0;
}

#ifdef USE_TIME
uint8_t HT16K33V110Display::strftime(uint8_t pos, const char *format, time::ESPTime time) {
  char buffer[64];
  size_t ret = time.strftime(buffer, sizeof(buffer), format);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}
    
uint8_t HT16K33V110Display::strftime(const char *format, time::ESPTime time) { return this->strftime(0, format, time); }
#endif
}  // namespace ht16k33v110
}  // namespace esphome
