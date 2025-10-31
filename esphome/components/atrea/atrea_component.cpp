#include "atrea_component.h"
#include "esphome/core/log.h"
#include <cstring>
     
#define MODE_RV         "Rovnotlaké větrání"
#define MODE_CV         "Cirkulační větrání"
#define MODE_CZ         "Cirkulace závislá"
#define MODE_C          "Cirkulace"
#define MODE_PV         "Přetlakové větrání"
#define MODE_CH         "Chlazení"
#define MODE_CHV        "Chlazení a větrání"
#define MODE_OFF        "Vypnuto"

#define INTENSITY_OFF   "Vypnuto"
#define INTENSITY_MED   "Střední"
#define INTENSITY_MAX   "Maximální"
     
namespace esphome {
namespace atrea {

static const char *const TAG = "atrea";

uint8_t AtreaComponent::crc8(uint8_t *addr, int len) {
  uint8_t crc = 0;
  
  for (int i = 0; i < len; i++) {
    uint8_t inbyte = addr[i];
    for (int j = 0; j < 8; j++) {
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix)
        crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
  return crc;
}

int AtreaComponent::read_packet(int readch, uint8_t *buffer) {
  static int pos = 0;

  if (pos >= BUFFER_SIZE && readch == 0xF5)
    pos = 0;

  if (pos < BUFFER_SIZE) {
    buffer[pos++] = readch;
    if (pos == BUFFER_SIZE)
      return crc8(buffer, 9) == readch;
  }
  return 0;
}

void AtreaComponent::setup() {
  ESP_LOGI(TAG, "Atrea component initialized");
}

void AtreaComponent::loop() {
  static uint8_t buffer[BUFFER_SIZE];
  static uint32_t packet_time_esp = 0;
  uint32_t ms = millis();

  if (this == nullptr)
    return;

  // Check for communication timeout
  if (ms - last_packet_time > 60000 && last_packet_time > 0) {
    ESP_LOGW(TAG, "No data received from Atrea for 60 seconds");
    last_packet_time = 0;
  }

  // Generate ESP packet for Atrea
  if (ms > (packet_time_cp + TIMEOUT_CP07) && ms > (packet_time_esp + 2000)) {  // 8s bez paketu CP07 (start nebo vypadek) a pak co 2s
    generate_esp_packet();
    packet_time_esp = ms;
  }

  // Read incoming data
  while (available()) {
    if (read_packet(read(), buffer)) {
      last_packet_time = ms; // Update last received timestamp
      
      if (memcmp(buffer, esp_data, BUFFER_SIZE) != 0) {  // not echo of sent data
        ESP_LOGD(TAG, "LIN: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", 
                 buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], 
                 buffer[5], buffer[6], buffer[7], buffer[8], buffer[9]);

        if ((buffer[1] >= 0 && buffer[1] <= 2) && (buffer[2] >= 0 && buffer[2] <= 3)) {
          if (buffer[8] == 0) {  // CP07 packet
            packet_time_cp = ms;
            if (cp_data_01[9] != buffer[9])
              update_time_cp = ms;
            memcpy(cp_data_01, buffer, BUFFER_SIZE);
          } else {  // Atrea packet
            if (buffer[1] == 0 && buffer[2] == 1) {  // Atrea01
              if (atrea_data_01[9] != buffer[9])
                update_time_atrea_01 = ms;
              memcpy(atrea_data_01, buffer, BUFFER_SIZE);
            } else if (buffer[1] == 0 && buffer[2] == 3) {  // Atrea03
              if (atrea_data_03[9] != buffer[9])
                update_time_atrea_03 = ms;
              memcpy(atrea_data_03, buffer, BUFFER_SIZE);
            } else if (buffer[1] == 1 && buffer[2] == 3) {  // Atrea13
              if (atrea_data_13[9] != buffer[9])
                update_time_atrea_13 = ms;
              memcpy(atrea_data_13, buffer, BUFFER_SIZE);
            }
          }
        }
        update();
      }
    }
  }
}

void AtreaComponent::generate_esp_packet() {
  static uint8_t pruchod = 0;
  uint32_t ms = millis();

  // generovani paketu ESP ovladace pro Atreu

  // Check required components
  if (esp_intensity_ == nullptr || esp_mode_ == nullptr) {
    ESP_LOGW(TAG, "Required components not initialized");
    return;
  }

  switch (pruchod++ % 4) {  // střídání dotazů do Atrey, kvůli sběru většího množství dat
    case 0:
      esp_data[1] = 0;
      esp_data[2] = 1;
      break;
    case 1:
      esp_data[1] = 0;
      esp_data[2] = 3;
      break;
    case 2:
      esp_data[1] = 0;
      esp_data[2] = 1;
      break;
    case 3:
      esp_data[1] = 1;
      esp_data[2] = 3;
      break;
  }

  // Intensity
  std::string mode = esp_mode_->state;
  std::string intensity = esp_intensity_->state;
  esp_data[3] = (intensity == INTENSITY_MED) ? 2 : ((intensity == INTENSITY_MAX) ? 4 : 1);

  // Update heating season based on heating/cooling requests
  if (esp_heating_ != nullptr && esp_heating_->state)
    topna_sezona = true;
  if (esp_cooling_ != nullptr && (pozadavek_chlazeni = esp_cooling_->state) == true)
    topna_sezona = false;

  /* Bajt Bypass ovládá obtokovou klapku – 1 pro obtok a 2 pro rekuperátor. 
     V režimu RV a CV nastavuje regulátor CP07 bajt podle venkovní teploty TE, teploty v místnosti a cílové nastavené teploty. 
     V ostatních režimech je bajt bps nastaven na 2.  
     Modifikovaná pravidla vylepšující chování:
        pro topnou sezónu a režim
          1 - venkovní teplota TE je větší než skutečná teplota v místnosti
          2 - venkovní teplota TE je nižší než skutečná teplota v místnosti
        pro období mimo topnou sezónu
          2 - venkovní teplota TE je vyšší než aktuální teplota v místnosti
          2 - požadovaná vnitřní teplota je vyšší než aktuální teplota v místnosti
          jinak pro CV
            1 - venkovní teplota TE je vyšší než 13 °C
            2 - venkovní teplota TE je nižší než 12 °C
          jinak pro RV
            1 - venkovní teplota TE je vyšší než 16 °C
            2 - venkovní teplota TE je nižší než 15 °C
  */
  if (thermostat_ != nullptr && (mode == MODE_RV || mode == MODE_CV)) {
    if (topna_sezona) {
      if (atrea_data_01[6] > thermostat_->current_temperature + 50)  // venku je tepleji, větrej s bypasem
        esp_data[5] = 1;
      else if (atrea_data_01[6] < thermostat_->current_temperature + 50)  // venku je chladněji, větrej rekuperátorem
        esp_data[5] = 2;
    } else {  // non-heating season
      if (atrea_data_01[6] > thermostat_->current_temperature + 50)  // venku je tepleji, větrej rekuperátorem
        esp_data[5] = 2;
      else if (thermostat_->target_temperature_low > thermostat_->current_temperature)  // poadovaná teplota j vtší neř aktuální vnitřní, větrej rekuperátorem
        esp_data[5] = 2;
      else if (esp_data[4] == 8) {  // jinak dolní limit podle typu - CV
        if (atrea_data_01[6] >= 13 + 50)
          esp_data[5] = 1;
        else if (atrea_data_01[6] < 12 + 50)
          esp_data[5] = 2;
      } else if (esp_data[4] == 16) {  // RV
        if (atrea_data_01[6] >= 16 + 50)
          esp_data[5] = 1;
        else if (atrea_data_01[6] < 15 + 50)
          esp_data[5] = 2;
      }
    }
    if (bypass != esp_data[5]) {    // změna bypasu
      update_time_esp = ms;
      bypass = esp_data[5];
    }
  }

  // Ventilation mode
  if (pozadavek_chlazeni) {   // chlazeni se realizuje bitem termostatu jako topení u PV, nema samostatnou polozku
    esp_data[4] = 1;
    esp_data[5] = 2;
    esp_data[6] = 2;
    esp_data[7] = 1;
  } else if (mode == MODE_RV) { // rovnotlaké větrání
    if (esp_heating_ != nullptr && esp_heating_->state) { // topeni
      esp_data[4] = 8;  // rovnotlake vetrani zapina kotel - proto vetram cirkulačním větráním, kdy topi TC
      esp_data[6] = 1;
      esp_data[7] = 3;  // topi
    } else {
      esp_data[4] = 16;  // rovnotlaké větrání
      esp_data[6] = 1;
      esp_data[7] = topna_sezona ? 2 : 0;  // netopím
    }
  } else if (mode == MODE_CV) { // cirkulační větrání
    esp_data[4] = 8;
    esp_data[6] = 1;
    esp_data[7] = (topna_sezona ? 2 : 0) | ((esp_heating_ != nullptr && esp_heating_->state) ? 1 : 0);
  } else if (mode == MODE_C) {  // cirkulace
    esp_data[4] = 4;
    esp_data[5] = 2;
    esp_data[6] = 1;
    esp_data[7] = (topna_sezona ? 2 : 0) | ((esp_heating_ != nullptr && esp_heating_->state) ? 1 : 0);
  } else if (mode == MODE_PV) { // přetlakové větrání
    esp_data[4] = 1;
    esp_data[5] = 2;
    esp_data[6] = 1;
    esp_data[7] = 0;
  } else /*if (mode == MODE_CZ)*/ { // cirkulace závislá
    esp_data[4] = 4;
    esp_data[5] = 2;
    esp_data[6] = 2;
    esp_data[7] = (topna_sezona ? 2 : 0) | ((esp_heating_ != nullptr && esp_heating_->state) ? 1 : 0);
  }

  esp_data[9] = crc8(esp_data, 9);

  for (int i = 0; i < BUFFER_SIZE; i++)
    write(esp_data[i]);
  
  ESP_LOGD(TAG, "ESP: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
           esp_data[0], esp_data[1], esp_data[2], esp_data[3], esp_data[4],
           esp_data[5], esp_data[6], esp_data[7], esp_data[8], esp_data[9]);
           
  update_esp();
}

void AtreaComponent::update() {
  uint32_t ms = millis();

  if (ms >= update_time_cp && cp_data_01[0] == 0xF5) {
    update_time_cp = ms + MAXINTERVAL;
    cp_data_01[0] = 0xFF;

    pozadavek_chlazeni = false;
    
    if (cp07_mode_ != nullptr) {
      if (cp_data_01[4] == 0x01 && cp_data_01[6] == 0x02) {
        cp07_mode_->publish_state(MODE_CH);
        pozadavek_chlazeni = true;
      } else if (cp_data_01[4] == 0x01 && cp_data_01[6] == 0x01)
        cp07_mode_->publish_state(MODE_PV);
      else if (cp_data_01[4] == 0x04 && cp_data_01[6] == 0x02)
        cp07_mode_->publish_state(MODE_CZ);
      else if (cp_data_01[4] == 0x04 && cp_data_01[6] == 0x01)
        cp07_mode_->publish_state(MODE_C);
      else if (cp_data_01[4] == 0x08 && cp_data_01[6] == 0x01)
        cp07_mode_->publish_state(MODE_CV);
      else if (cp_data_01[4] == 0x10 && cp_data_01[6] == 0x01)
        cp07_mode_->publish_state(MODE_RV);
      else
        cp07_mode_->publish_state("???");
    }

    if (cp07_intensity_ != nullptr) {
      if (cp_data_01[3] == 0x01)
        cp07_intensity_->publish_state(INTENSITY_OFF);
      else if (cp_data_01[3] == 0x02)
        cp07_intensity_->publish_state(INTENSITY_MED);
      else if (cp_data_01[3] == 0x04)
        cp07_intensity_->publish_state(INTENSITY_MAX);
      else
        cp07_intensity_->publish_state("???");
    }

    if (cp07_temperature_control_ != nullptr) {
      if (cp_data_01[4] == 0x01 && cp_data_01[6] == 0x02 && cp_data_01[7] == 0x01)
        cp07_temperature_control_->publish_state("Chladí");
      else if ((cp_data_01[4] == 0x04 || cp_data_01[4] == 0x08 || cp_data_01[4] == 0x10) && cp_data_01[6] == 0x01 && (cp_data_01[7] & 0x01) == 0x01)
        cp07_temperature_control_->publish_state("Topí");
      else if (cp_data_01[4] == 0x04 && cp_data_01[6] == 0x02 && (cp_data_01[7] & 0x01) == 0x01)
        cp07_temperature_control_->publish_state("Topí");
      else
        cp07_temperature_control_->publish_state("Neaktivní");
    }

    if (cp07_bypass_ != nullptr)
      cp07_bypass_->publish_state(cp_data_01[5] == 1 ? "Zapnuto" : "Vypnuto");
      
    if (ms < (packet_time_cp + TIMEOUT_CP07)) {   // 8s pro prichodu CP paketu je aktivni komunikace s CP07
      if (esp_active_controller_ != nullptr)
        esp_active_controller_->publish_state(false);
    }
  }

  if (ms >= update_time_atrea_01 && atrea_data_01[0] == 0xF5) {
    char str_rezim[SAFE_STRING_SIZE] = {0};

    update_time_atrea_01 = ms + MAXINTERVAL;
    atrea_data_01[0] = 0xFF;

    if (atrea_mode_ != nullptr) {
      switch (atrea_data_01[3]) {
        case 0x00:
          strcpy(str_rezim, MODE_OFF);
          break;
        case 0x01:
          if (atrea_data_01[4] & 0x20)
            strcpy(str_rezim, MODE_CH);
          else
            strcpy(str_rezim, MODE_PV);
          break;
        case 0x04:
          if (atrea_data_01[4] & 0x20)
            strcpy(str_rezim, MODE_CZ);
          else
            strcpy(str_rezim, MODE_C);
          break;
        case 0x05:
          strcpy(str_rezim, MODE_CH);
          break;
        case 0x08:
          if (pozadavek_chlazeni)
            strcpy(str_rezim, MODE_CHV);
          else
            strcpy(str_rezim, MODE_CV);
          break;
        case 0x10:
          strcpy(str_rezim, MODE_RV);
          break;
        default:
          strcpy(str_rezim, "???");
      }
      
      if ((atrea_data_01[4] & 0x10) && (atrea_data_01[4] & 0x03))
        strncat(str_rezim, " + nárazové větrání", sizeof(str_rezim)-strlen(str_rezim)-1);
      atrea_mode_->publish_state(str_rezim);
    }

    if (atrea_intensity_ != nullptr) {
      if ((atrea_data_01[4] & 0x03) == 0)
        atrea_intensity_->publish_state(INTENSITY_OFF);
      else if ((atrea_data_01[4] & 0x03) == 1)
        atrea_intensity_->publish_state(INTENSITY_MED);
      else if ((atrea_data_01[4] & 0x03) == 2)
        atrea_intensity_->publish_state(INTENSITY_MAX);
      else
        atrea_intensity_->publish_state("???");
    }

    if (atrea_errors_ != nullptr) {
      if (atrea_data_01[4] & 0x04)
        atrea_errors_->publish_state("zanesený filtr");
      else if (atrea_data_01[5] & 0x01)
        atrea_errors_->publish_state("porucha čidla TE");
      else if (atrea_data_01[5] & 0x02)
        atrea_errors_->publish_state("porucha čidla TI2");
      else if (atrea_data_01[5] & 0x04)
        atrea_errors_->publish_state("zámraz rekuperátoru");
      else if (atrea_data_01[5] & 0x08)
        atrea_errors_->publish_state("porucha čidla TA");
      else if (atrea_data_01[5] & 0x10)
        atrea_errors_->publish_state("1.mrazová ochrana");
      else if (atrea_data_01[5] & 0x20)
        atrea_errors_->publish_state("2.mrazová ochrana");
      else if (atrea_data_01[5] & 0x40)
        atrea_errors_->publish_state("obvod STOP aktivní");
      else if (atrea_data_01[5] & 0x80)
        atrea_errors_->publish_state("chyba komunikace");
      else
        atrea_errors_->publish_state("ok");
    }

    if (atrea_data_01[6] > (-40 + 50) && atrea_data_01[6] < (50 + 50)) {
      if (outdoor_temperature_ != nullptr)
        outdoor_temperature_->publish_state(atrea_data_01[6] - 50.0);

      // Update heating season based on outdoor temperature
      if (atrea_data_01[6] > 25 + 50 && topna_sezona == true) {
        topna_sezona = false;
        update_time_esp = ms;
      }
      if (atrea_data_01[6] < 5 + 50 && topna_sezona == false) {
        topna_sezona = true;
        update_time_esp = ms;
      }
    }
    
    if (atrea_data_01[7] > -10 + 50) {
      if (exchanger_temperature_ != nullptr)
        exchanger_temperature_->publish_state(atrea_data_01[7] - 50.0);
    }

    if (atrea_heating_ != nullptr)
      atrea_heating_->publish_state(atrea_data_01[4] & 0x08);
    
    if (atrea_cooling_ != nullptr) {
      bool cooling = (atrea_data_01[3] == 5 && (atrea_data_01[4] & 0x03)) ||
                     (atrea_data_01[3] == 8 && (atrea_data_01[4] & 0x03) && pozadavek_chlazeni) ||
                     (atrea_data_01[3] == 1 && (atrea_data_01[4] & 0x03) && (atrea_data_01[4] & 0x20));
      atrea_cooling_->publish_state(cooling);
    }
    
    if (atrea_boost_ventilation_ != nullptr)
      atrea_boost_ventilation_->publish_state(atrea_data_01[4] & 0x10);
  }

  if (ms >= update_time_atrea_03 && atrea_data_03[0] == 0xF5) {
    update_time_atrea_03 = ms + MAXINTERVAL;
    atrea_data_03[0] = 0xFF;

    if (atrea_d1_ != nullptr)
      atrea_d1_->publish_state(atrea_data_03[3] & 0x01);
    if (atrea_d2_ != nullptr)
      atrea_d2_->publish_state(atrea_data_03[3] & 0x02);
    if (atrea_d3_ != nullptr)
      atrea_d3_->publish_state(atrea_data_03[3] & 0x04);
    if (atrea_d4_ != nullptr)
      atrea_d4_->publish_state(atrea_data_03[3] & 0x08);

    if (atrea_damper_sr_ != nullptr)
      atrea_damper_sr_->publish_state(atrea_data_03[8] & 0x01);
    if (atrea_bypass_sb_ != nullptr)
      atrea_bypass_sb_->publish_state(atrea_data_03[8] & 0x02);
    if (atrea_yv_ != nullptr)
      atrea_yv_->publish_state(atrea_data_03[8] & 0x04);
    if (atrea_k_ != nullptr)
      atrea_k_->publish_state(atrea_data_03[8] & 0x08);
    if (atrea_oc1_ != nullptr)
      atrea_oc1_->publish_state(atrea_data_03[8] & 0x20);
  }

  if (ms >= update_time_atrea_13 && atrea_data_13[0] == 0xF5) {
    update_time_atrea_13 = ms + MAXINTERVAL;
    atrea_data_13[0] = 0xFF;

    if (atrea_data_13[5] > -10 + 50) {
      if (recuperator_temperature_ != nullptr)
        recuperator_temperature_->publish_state(atrea_data_13[5] - 50.0);
    }

    if (circulation_ != nullptr)
      circulation_->publish_state(atrea_data_13[3] * 100 / 255);
  }
}

void AtreaComponent::update_esp() {
  uint32_t ms = millis();

  if (ms >= update_time_esp) {
    update_time_esp = ms + MAXINTERVAL;

    if (esp_active_controller_ != nullptr)
      esp_active_controller_->publish_state(true);
    if (cp07_intensity_ != nullptr)
      cp07_intensity_->publish_state("unavailable");
    if (cp07_mode_ != nullptr)
      cp07_mode_->publish_state("unavailable");
    if (cp07_temperature_control_ != nullptr)
      cp07_temperature_control_->publish_state("unavailable");
    if (cp07_bypass_ != nullptr)
      cp07_bypass_->publish_state("unavailable");
      
    if (esp_heating_season_ != nullptr)
      esp_heating_season_->publish_state(topna_sezona);
    if (esp_bypass_ != nullptr)
      esp_bypass_->publish_state(esp_data[5] == 1);
  }
}

}  // namespace atrea
}  // namespace esphome
