#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/select/select.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace atrea {

#define MAXINTERVAL      60000   // max update interval [ms]
#define TIMEOUT_CP07     8000    // timeout pro nekomunikaci CP07   
#define BUFFER_SIZE      10      // delka paketu
#define SAFE_STRING_SIZE 80      // delka stringu režimu

class AtreaComponent : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_thermostat(climate::Climate *thermostat) { thermostat_ = thermostat; }
  void set_esp_intensity(select::Select *intensity) { esp_intensity_ = intensity; }
  void set_esp_mode(select::Select *mode) { esp_mode_ = mode; }
  void set_esp_heating(switch_::Switch *heating) { esp_heating_ = heating; }
  void set_esp_cooling(switch_::Switch *cooling) { esp_cooling_ = cooling; }

  // Text sensors
  void set_cp07_mode(text_sensor::TextSensor *sensor) { cp07_mode_ = sensor; }
  void set_cp07_intensity(text_sensor::TextSensor *sensor) { cp07_intensity_ = sensor; }
  void set_cp07_temperature_control(text_sensor::TextSensor *sensor) { cp07_temperature_control_ = sensor; }
  void set_cp07_bypass(text_sensor::TextSensor *sensor) { cp07_bypass_ = sensor; }
  void set_atrea_mode(text_sensor::TextSensor *sensor) { atrea_mode_ = sensor; }
  void set_atrea_intensity(text_sensor::TextSensor *sensor) { atrea_intensity_ = sensor; }
  void set_atrea_errors(text_sensor::TextSensor *sensor) { atrea_errors_ = sensor; }

  // Sensors
  void set_outdoor_temperature(sensor::Sensor *sensor) { outdoor_temperature_ = sensor; }
  void set_exchanger_temperature(sensor::Sensor *sensor) { exchanger_temperature_ = sensor; }
  void set_recuperator_temperature(sensor::Sensor *sensor) { recuperator_temperature_ = sensor; }
  void set_circulation(sensor::Sensor *sensor) { circulation_ = sensor; }

  // Binary sensors
  void set_atrea_heating(binary_sensor::BinarySensor *sensor) { atrea_heating_ = sensor; }
  void set_atrea_cooling(binary_sensor::BinarySensor *sensor) { atrea_cooling_ = sensor; }
  void set_atrea_boost_ventilation(binary_sensor::BinarySensor *sensor) { atrea_boost_ventilation_ = sensor; }
  void set_atrea_d1(binary_sensor::BinarySensor *sensor) { atrea_d1_ = sensor; }
  void set_atrea_d2(binary_sensor::BinarySensor *sensor) { atrea_d2_ = sensor; }
  void set_atrea_d3(binary_sensor::BinarySensor *sensor) { atrea_d3_ = sensor; }
  void set_atrea_d4(binary_sensor::BinarySensor *sensor) { atrea_d4_ = sensor; }
  void set_atrea_damper_sr(binary_sensor::BinarySensor *sensor) { atrea_damper_sr_ = sensor; }
  void set_atrea_bypass_sb(binary_sensor::BinarySensor *sensor) { atrea_bypass_sb_ = sensor; }
  void set_atrea_yv(binary_sensor::BinarySensor *sensor) { atrea_yv_ = sensor; }
  void set_atrea_k(binary_sensor::BinarySensor *sensor) { atrea_k_ = sensor; }
  void set_atrea_oc1(binary_sensor::BinarySensor *sensor) { atrea_oc1_ = sensor; }
  void set_esp_active_controller(binary_sensor::BinarySensor *sensor) { esp_active_controller_ = sensor; }
  void set_esp_heating_season(binary_sensor::BinarySensor *sensor) { esp_heating_season_ = sensor; }
  void set_esp_bypass(binary_sensor::BinarySensor *sensor) { esp_bypass_ = sensor; }

 protected:
  climate::Climate *thermostat_{nullptr};
  select::Select *esp_intensity_{nullptr};
  select::Select *esp_mode_{nullptr};
  switch_::Switch *esp_heating_{nullptr};
  switch_::Switch *esp_cooling_{nullptr};

  // Text sensors
  text_sensor::TextSensor *cp07_mode_{nullptr};
  text_sensor::TextSensor *cp07_intensity_{nullptr};
  text_sensor::TextSensor *cp07_temperature_control_{nullptr};
  text_sensor::TextSensor *cp07_bypass_{nullptr};
  text_sensor::TextSensor *atrea_mode_{nullptr};
  text_sensor::TextSensor *atrea_intensity_{nullptr};
  text_sensor::TextSensor *atrea_errors_{nullptr};

  // Sensors
  sensor::Sensor *outdoor_temperature_{nullptr};
  sensor::Sensor *exchanger_temperature_{nullptr};
  sensor::Sensor *recuperator_temperature_{nullptr};
  sensor::Sensor *circulation_{nullptr};

  // Binary sensors
  binary_sensor::BinarySensor *atrea_heating_{nullptr};
  binary_sensor::BinarySensor *atrea_cooling_{nullptr};
  binary_sensor::BinarySensor *atrea_boost_ventilation_{nullptr};
  binary_sensor::BinarySensor *atrea_d1_{nullptr};
  binary_sensor::BinarySensor *atrea_d2_{nullptr};
  binary_sensor::BinarySensor *atrea_d3_{nullptr};
  binary_sensor::BinarySensor *atrea_d4_{nullptr};
  binary_sensor::BinarySensor *atrea_damper_sr_{nullptr};
  binary_sensor::BinarySensor *atrea_bypass_sb_{nullptr};
  binary_sensor::BinarySensor *atrea_yv_{nullptr};
  binary_sensor::BinarySensor *atrea_k_{nullptr};
  binary_sensor::BinarySensor *atrea_oc1_{nullptr};
  binary_sensor::BinarySensor *esp_active_controller_{nullptr};
  binary_sensor::BinarySensor *esp_heating_season_{nullptr};
  binary_sensor::BinarySensor *esp_bypass_{nullptr};

  // Data packets
  uint8_t cp_data_01[BUFFER_SIZE] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  uint8_t esp_data[BUFFER_SIZE] = {0xF5, 0, 1, 0, 0, 2, 1, 2, 0};
  uint8_t atrea_data_01[BUFFER_SIZE] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  uint8_t atrea_data_03[BUFFER_SIZE] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  uint8_t atrea_data_13[BUFFER_SIZE] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  // Timers
  uint32_t last_packet_time = 0;        // cas posledniho paketu
  uint32_t packet_time_cp = 0;          // cas posledniho paketu CP07
  uint32_t update_time_esp = 0;         // cas pro publikovani dat paketu ESP
  uint32_t update_time_cp = 0;          // cas pro publikovani dat paketu CP07
  uint32_t update_time_atrea_01 = 0;    // cas pro publikovani dat paketu Atrea01
  uint32_t update_time_atrea_03 = 0;    // cas pro publikovani dat paketu Atrea03
  uint32_t update_time_atrea_13 = 0;    // cas pro publikovani dat paketu Atrea13
  
  bool pozadavek_chlazeni = false;
  bool topna_sezona = false;
  uint8_t bypass = 0;

  // Helper methods
  uint8_t crc8(uint8_t *addr, int len);
  int read_packet(int readch, uint8_t *buffer);
  void generate_esp_packet();
  void update();
  void update_esp();
};

}  // namespace atrea
}  // namespace esphome