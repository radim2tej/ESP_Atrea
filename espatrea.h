#include "esphome.h"

// pakety
char cpData[11] = {0xff};	// paket z CP07
char atreaData01[11] = {0xff};	// paket z Atrey
//char atreaData03[10];
//char atreaData13[10];
//char atreaData23[10];

// casy paketu
uint32_t timeCp = 0;
uint32_t timeEsp = 0;
uint32_t timeAtrea = 0;

// casovace pro aktualizaci dat
uint32_t zmenaCpText = 0;
uint32_t zmenaCpBinSen = 0;
uint32_t zmenaAtreaText = 0;
uint32_t zmenaAtreaSensor = 0;
uint32_t zmenaAtreaBinSen = 0;

bool pozadavek_chlazeni = false;
bool narazove_vetrani = false;
bool maximalni_intenzita_vetrani = false;

#define MAXINTERVAL     60000   // max update interval [ms] 

const int gpio_nslp = 4; // D2
#define NSLP0   digitalWrite(gpio_nslp, LOW);
#define NSLP1   digitalWrite(gpio_nslp, HIGH);

unsigned char crc8(char *addr, int len)
{
  char crc=0;
      
  for (int i = 0; i < len; i++) 
  {
    char inbyte = addr[i];
    for (int j = 0; j < 8; j++) 
    {
      char mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) 
        crc ^= 0x8C;
                  
      inbyte >>= 1;
    }
  }
  return crc;
}

class AtreaUart : public Component, public UARTDevice, public TextSensor {
 public:
  AtreaUart(UARTComponent *parent) : UARTDevice(parent) {}

  void setup() override
  {
    pinMode(gpio_nslp, OUTPUT);
    NSLP0;
  }

  int readpacket(int readch, char *buffer)
  {
    static int pos = 0;

    if (readch == 0xF5)
      pos = 0;

    if (pos < 10) {
      buffer[pos++] = readch;
      if (pos == 10)
        //return 1;
        return crc8(buffer, 9) == readch;
    }
    return 0;
  }

  void loop() override
  {
    static char buffer[10+1];
    static uint32_t last_ms = 0;
    uint32_t ms = millis();
    uint8_t espData[10] = {0xF5, 0, 1, 0, 0, 2, 1, 2, 0} ;       	// paket ovladace do Atrey

    NSLP1;
    // generovani paketu ESP ovladace pro Atreu
    if (ms > (timeCp + 15000) && ms > (last_ms + 2000)) {
      int i;

      last_ms = ms;
      espData[3] = id(esp_intenzita).state == "Střední" ? 2 : (id(esp_intenzita).state == "Maximální" ? 4 : 1);
      pozadavek_chlazeni = id(esp_chlazeni).state;
      if (pozadavek_chlazeni) {   // chlazeni se realizuje bitem termostatu jako topení, nema samostatnou polozku
        espData[4] = 1;
        espData[5] = 2;
        espData[6] = 2;
        espData[7] = 1;
      } else if (id(esp_rezim_vzt).state == "Rovnotlaké větrání") {
        if (id(esp_topeni).state) { // topeni
//          if (narazove_vetrani && maximalni_intenzita_vetrani) { // vetrani kuchyne zapina kotel - proto vypinam topeni
//            espData[4] = 16;	// rovnotlaké větrání
//            espData[5] = 2;
//            espData[6] = 2;
//            espData[7] = 0;   // 2; netopim (0 u NTS)
//          } else {
            espData[4] = 8;
            espData[5] = 1;//2; //2 nebo 1 v NTS nevim souvislost
            espData[6] = 1;
            espData[7] = 3;// topi
//          }
        } else {	// vetrani WC zapina kotel - proto vetram cirkulačním větráním, kdy topi TC
          espData[4] = 16;
          espData[5] = 2;
          espData[6] = 2;
          espData[7] = 0;   // topim
        }
      } else if (id(esp_rezim_vzt).state == "Cirkulační větrání") {
        espData[4] = 8;
        espData[5] = 1;//2; //2 nebo 1 v NTS nevim souvislost
        espData[6] = 1;
        espData[7] = id(esp_topeni).state ? 3 : 2;//0; //0 u NTS
      } else if (id(esp_rezim_vzt).state == "Cirkulace") {
        espData[4] = 4;
        espData[5] = 2;
        espData[6] = 1;
        espData[7] = id(esp_topeni).state ? 3 : 0; //0 u NTS
      } else if (id(esp_rezim_vzt).state == "Přetlakové větrání") {
        espData[4] = 1;
        espData[5] = 2;
        espData[6] = 1;
        espData[7] = 0;
      } else /*if (id(esp_rezim_vzt).state == "Cirkulace závislá")*/ {
        espData[4] = 4;
        espData[5] = 2;
        espData[6] = 2;
        espData[7] = id(esp_topeni).state ? 3 : 0; //0 u NTS
      }
      espData[9] = crc8(espData, 9);

      timeEsp = ms;
      for (i = 0; i < 10; i++)
        write(espData[i]);
//      flush();
//      ESP_LOGD("ESP", "%02X %02X - %02X %02X %02X %02X %02X %02X", espData[1], espData[2], espData[3], espData[4], espData[5], espData[6], espData[7], espData[8]);
    }

    while (available()) {
      if (readpacket(read(), buffer)) {
        if (ms > (timeEsp + 1000) && buffer[1] == 0 && buffer[2] == 1 && buffer[8] == 0) {  // packet CP07
//      if (memcmp(espData, buffer, 10) != 0 && buffer[1] == 0 && buffer[2] == 1 && buffer[8] == 0) {  // packet CP07
//          ESP_LOGD("CP07", "%02X %02X - %02X %02X %02X %02X %02X %02X", buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8]);
          timeCp = ms;
          if (cpData[9] != buffer[9])   // odlisne CRC
            zmenaCpText = zmenaCpBinSen = timeCp - MAXINTERVAL;
          memcpy(cpData, buffer, 10);
        } else {  // filtruji ESP paket
          if (buffer[1] == 0 && buffer[2] == 1 && buffer[8] != 0) {	  // packet Atrea01
            ESP_LOGD("Atrea", "%02X %02X - %02X %02X %02X %02X %02X %02X", buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8]);
            timeAtrea = ms;
            if (atreaData01[9] != buffer[9])   // odlisne CRC
              zmenaAtreaText = zmenaAtreaSensor = zmenaAtreaBinSen = timeAtrea - MAXINTERVAL;
            memcpy(atreaData01, buffer, 10);
//          } else if (buffer[1] == 0 && buffer[2] == 3 && buffer[8] != 0) {	  // packet Atrea03
//            ESP_LOGD("Atrea", "%02X %02X - %02X %02X %02X %02X %02X %02X", buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8]);
//            memcpy(atreaData03, buffer, 9);
//          }
//          else if (buffer[1] == 1 && buffer[2] == 3 && buffer[8] != 0) {	  // packet Atrea13
//            ESP_LOGD("Atrea", "%02X %02X - %02X %02X %02X %02X %02X %02X", buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8]);
//            memcpy(atreaData13, buffer, 9);
//          }
//          else if (buffer[1] == 2 && buffer[2] == 3 && buffer[8] != 0) {	  // packet Atrea23
//            ESP_LOGD("Atrea", "%02X %02X - %02X %02X %02X %02X %02X %02X", buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8]);
//            memcpy(atreaData23, buffer, 9);
          }
        }
	  }
    }
  }
};

class AtreaTextSensor : public PollingComponent {
 public:
  TextSensor *cp07_rezim = new TextSensor();
  TextSensor *cp07_intenzita = new TextSensor();
  TextSensor *cp07_rizeni_teploty = new TextSensor();
  TextSensor *atrea_rezim = new TextSensor();
  TextSensor *atrea_intenzita = new TextSensor();
  TextSensor *atrea_chyby = new TextSensor();

  AtreaTextSensor() : PollingComponent(2000) {}

  void update() override {
    uint32_t ms = millis();

    if (ms > zmenaCpText + MAXINTERVAL) { // aktualizuj při změně nebo co MAXINTERVAL    
      zmenaCpText = ms;    
      if (cpData[0] == 0xF5 && ms < (timeCp + 15000)) {
        pozadavek_chlazeni = false;
        if (cpData[4] == 0x01 && cpData[6] == 0x02) {
          cp07_rezim->publish_state("Chlazení");              // Chlazeni
          pozadavek_chlazeni = true;
        } else if (cpData[4] == 0x01 && cpData[6] == 0x01)
          cp07_rezim->publish_state("Přetlakové větrání");    // PV
        else if (cpData[4] == 0x04 && cpData[6] == 0x02)
          cp07_rezim->publish_state("Cirkulace závislá");     // CZ
        else if (cpData[4] == 0x04 && cpData[6] == 0x01)
          cp07_rezim->publish_state("Cirkulace");             // C
        else if (cpData[4] == 0x08 && cpData[6] == 0x01)
          cp07_rezim->publish_state("Cirkulační větrání");    // CV
        else if (cpData[4] == 0x10 && cpData[6] == 0x01)
          cp07_rezim->publish_state("Rovnotlaké větrání");    // RV
        else 
          cp07_rezim->publish_state("???");
	
        if (cpData[3] == 0x01)
          cp07_intenzita->publish_state("Vypnuto");
        else if (cpData[3] == 0x02)
          cp07_intenzita->publish_state("Střední");
        else if (cpData[3] == 0x04)
          cp07_intenzita->publish_state("Maximální");
        else
          cp07_intenzita->publish_state("???");

        if (cpData[4] == 0x01 && cpData[6] == 0x02 && cpData[7] == 0x01)	// PV
          cp07_rizeni_teploty->publish_state("Chladí");
        else if ((cpData[4] == 0x04 || cpData[4] == 0x08 || cpData[4] == 0x10) && cpData[6] == 0x01 && (cpData[7] & 0x01) == 0x01)	// R, CV, C
          cp07_rizeni_teploty->publish_state("Topí");
        else if (cpData[4] == 0x04 && cpData[6] == 0x02 && (cpData[7] & 0x01) == 0x01)	// CZ
          cp07_rizeni_teploty->publish_state("Topí");
        else
          cp07_rizeni_teploty->publish_state("Neaktivní");
      }
    }

    if (ms > zmenaAtreaText + MAXINTERVAL) { // aktualizuj při změně nebo co MAXINTERVAL
      char strRezim[80];
          
      zmenaAtreaText = ms;    
      if (atreaData01[0] == 0xF5 && ms < (timeAtrea + 15000)) {
        switch (atreaData01[3]) {
          case 0x00:
            strcpy(strRezim, "Vypnuto");                            // 00
            break;
          case 0x01:
            if (atreaData01[4] & 0x20)                              // 01 31 Chlazení s nárazovým větráním
              strcpy(strRezim, "Chlazení");
            else
              strcpy(strRezim, "Přetlakové větrání");               // 01 01 Pretlakove vetrani
            break;
          case 0x04:
            if (atreaData01[4] & 0x20)
              strcpy(strRezim, "Cirkulace závislá");                // 04 20 Cirkulace závislá
            else                                                    // 04 31 Nárazové větrání WC
              strcpy(strRezim, "Cirkulace");                        // 04 01 Cirkulace
            break;                                                  // 04 11 Cirkulace s nárazovým větráním
          case 0x05:
            strcpy(strRezim, "Chlazení");                           // 05 21 Chlazeni
            break;
          case 0x08:
            if (pozadavek_chlazeni)
              strcpy(strRezim, "Chlazení a větrání");               // 08 ?? Chlazení s periodickým větráním
            else
              strcpy(strRezim, "Cirkulační větrání");               // 08 21 Cirkulacni vetrani (také periodické větrání)
            break;
          case 0x10:
            strcpy(strRezim, "Rovnotlaké větrání");                 // 10 21 Rovnotlake větráni
            break;                                                  // 10 32 Nárazové větráni kuchyň
          default:
            strcpy(strRezim, "???");
        }
        if ((atreaData01[4] & 0x10) && (atreaData01[4] & 0x03))     // nárazové větrání
          strcat(strRezim, " + nárazové větrání");
        atrea_rezim->publish_state(strRezim);
      
        if ((atreaData01[4] & 0x03) == 0) {
          atrea_intenzita->publish_state("Vypnuto");
          maximalni_intenzita_vetrani = false;
        } else if ((atreaData01[4] & 0x03) == 1) {
          atrea_intenzita->publish_state("Střední");
          maximalni_intenzita_vetrani = false;
        } else if ((atreaData01[4] & 0x03) == 2) {
          atrea_intenzita->publish_state("Maximální");
          maximalni_intenzita_vetrani = true;
        } else {
          atrea_intenzita->publish_state("???");
          maximalni_intenzita_vetrani = false;
        }

        if (atreaData01[4] & 0x04)          
          atrea_chyby->publish_state("zanesený filtr");
        else if (atreaData01[5] & 0x01)
          atrea_chyby->publish_state("porucha čidla TE");
        else if (atreaData01[5] & 0x02)
          atrea_chyby->publish_state("porucha čidla TI2");
        else if (atreaData01[5] & 0x04)
          atrea_chyby->publish_state("zámraz rekuperátoru");
        else if (atreaData01[5] & 0x08)
          atrea_chyby->publish_state("porucha čidla TA");
        else if (atreaData01[5] & 0x10)
          atrea_chyby->publish_state("1.mrazová ochrana");
        else if (atreaData01[5] & 0x20)
          atrea_chyby->publish_state("2.mrazová ochrana");
        else if (atreaData01[5] & 0x40)
          atrea_chyby->publish_state("obvod STOP aktivní");
        else if (atreaData01[5] & 0x80)
          atrea_chyby->publish_state("chyba komunikace");
        else 
          atrea_chyby->publish_state("ok");
      }
    }
  }
};

class AtreaSensor : public PollingComponent {
 public:
  Sensor *atrea_teplota_TE = new Sensor();
  Sensor *atrea_teplota_TA = new Sensor();

  AtreaSensor() : PollingComponent(2000) { }

  void setup() override {
    // nothing to do here
  }

  void update() override {
    uint32_t ms = millis();

    if (ms > zmenaAtreaSensor + MAXINTERVAL) { // aktualizuj při změně nebo co MAXINTERVAL
      zmenaAtreaSensor = ms;    
      if (atreaData01[0] == 0xF5 && ms < (timeAtrea + 15000)) {
        if (atreaData01[6] > 10)
            atrea_teplota_TE->publish_state(atreaData01[6] - 50.0);
        if (atreaData01[7] > 10)
            atrea_teplota_TA->publish_state(atreaData01[7] - 50.0);
      }
    }
  }
};

class AtreaBinarySensor : public PollingComponent {
 public:
  BinarySensor *atrea_topi = new BinarySensor();
  BinarySensor *atrea_chladi = new BinarySensor();
  BinarySensor *atrea_narazove_vetrani = new BinarySensor();
  BinarySensor *esp_aktivni_ovladac = new BinarySensor();

  AtreaBinarySensor() : PollingComponent(2000) { }

  void setup() override {
    // nothing to do here
  }

  void update() override {
    uint32_t ms = millis();

    if (ms > zmenaAtreaBinSen + MAXINTERVAL) { // aktualizuj při změně nebo co MAXINTERVAL    
      zmenaAtreaBinSen = ms;    
      if (atreaData01[0] == 0xF5 && ms < (timeAtrea + 15000)) {
        atrea_topi->publish_state(atreaData01[4] & 0x08);
        atrea_chladi->publish_state((atreaData01[3] == 5 && (atreaData01[4] & 0x03)) 
                                 || (atreaData01[3] == 8 && (atreaData01[4] & 0x03) && pozadavek_chlazeni)
                                 || (atreaData01[3] == 1 && (atreaData01[4] & 0x03) && (atreaData01[4] & 0x20)));
        atrea_narazove_vetrani->publish_state(atreaData01[4] & 0x10);
        narazove_vetrani = atreaData01[4] & 0x10;
      }
    }

    if (ms > zmenaCpBinSen + MAXINTERVAL) { // aktualizuj při změně nebo co MAXINTERVAL    
      zmenaCpBinSen = ms;    
      if (ms < (timeCp + 15000))
        esp_aktivni_ovladac->publish_state(false);
      else
        esp_aktivni_ovladac->publish_state(true);
    }
  }
};
