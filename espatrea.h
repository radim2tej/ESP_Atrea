#include "esphome.h"

// pakety
uint8_t cpData[10] = {0xff};	// paket z CP07
uint8_t espData[10] = {0xF5, 0, 1, 0, 0, 2, 1, 2, 0} ;       	// paket ovladace do Atrey
uint8_t atreaData01[10] = {0xff};	// paket z Atrey
uint8_t atreaData03[10];
uint8_t atreaData13[10];

// casy paketu
uint32_t timeCp = 0;
uint32_t timeEsp = 0;
uint32_t timeAtrea01 = 0;
uint32_t timeAtrea03 = 0;
uint32_t timeAtrea13 = 0;

// casovace pro aktualizaci dat
uint32_t zmenaCpText = 0;
uint32_t zmenaCpBinSen = 0;
uint32_t zmenaAtreaText = 0;
uint32_t zmenaAtreaSensor01 = 0;
uint32_t zmenaAtreaSensor13 = 0;
uint32_t zmenaAtreaBinSen01 = 0;
uint32_t zmenaAtreaBinSen03 = 0;

bool pozadavek_chlazeni = false;
bool narazove_vetrani = false;
bool topna_sezona = false;

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

  int readpacket(int readch, uint8_t *buffer)
  {
    static int pos = 0;

    if (pos >= 10 && readch == 0xF5)
      pos = 0;

    if (pos < 10) {
      buffer[pos++] = readch;
      if (pos == 10)
        return 1;
//        return crc8(buffer, 9) == readch;
    }
    return 0;
  }

  void loop() override
  {
    static uint8_t buffer[10];
    static uint32_t last_ms = 0;
    uint32_t ms = millis();

    NSLP1;
    
    if (id(esp_topeni).state)
        topna_sezona = true;
    if ((pozadavek_chlazeni = id(esp_chlazeni).state) == true)    
        topna_sezona = false;
        
    // generovani paketu ESP ovladace pro Atreu
    if (ms > (timeCp + 15000) && ms > (last_ms + 2000)) {
      static uint32_t pruchod = 0;
      uint8_t recuperator;
      int i;

      last_ms = ms;

      switch (++pruchod % 4) {
        case 0:
          espData[1] = 0;
          espData[2] = 1;
          break;
        case 1:
          espData[1] = 0;
          espData[2] = 3;
          break;
        case 2:
          espData[1] = 0;
          espData[2] = 1;
          break;
        case 3:
          espData[1] = 1;
          espData[2] = 3;
          break;
      } 

      espData[3] = id(esp_intenzita).state == "Střední" ? 2 : (id(esp_intenzita).state == "Maximální" ? 4 : 1);
      
      if (pozadavek_chlazeni) {   // chlazeni se realizuje bitem termostatu jako topení, nema samostatnou polozku
        espData[4] = 1;
        espData[5] = 2;     
        espData[6] = 2;
        espData[7] = 1;
      } else if (id(esp_rezim_vzt).state == "Rovnotlaké větrání") {
        if (id(esp_topeni).state) { // topeni
//          // rovnotlake vetrani zapina kotel - proto vypinam topeni
//          espData[4] = 16;	// rovnotlaké větrání
//          espData[6] = 1;
//          espData[7] = 2;   // netopim
          espData[4] = 8;  // rovnotlake vetrani zapina kotel - proto vetram cirkulačním větráním, kdy topi TC
          espData[6] = 1;
          espData[7] = 3;  // topi
        } else {
          espData[4] = 16;
          espData[6] = 1;
          espData[7] = topna_sezona ? 2 : 0;  // netopím
        }
      } else if (id(esp_rezim_vzt).state == "Cirkulační větrání") {
        espData[4] = 8;
        espData[6] = 1;
        espData[7] = (topna_sezona ? 2 : 0) | id(esp_topeni).state ? 1 : 0;
      } else if (id(esp_rezim_vzt).state == "Cirkulace") {
        espData[4] = 4;
        espData[5] = 2;     
        espData[6] = 1;
        espData[7] = (topna_sezona ? 2 : 0) | id(esp_topeni).state ? 1 : 0;
      } else if (id(esp_rezim_vzt).state == "Přetlakové větrání") {
        espData[4] = 1;
        espData[5] = 2;     
        espData[6] = 1;
        espData[7] = 0;
      } else /*if (id(esp_rezim_vzt).state == "Cirkulace závislá")*/ {
        espData[4] = 4;
        espData[5] = 2;     
        espData[6] = 2;
        espData[7] = (topna_sezona ? 2 : 0) | id(esp_topeni).state ? 1 : 0;
      }
      
      if (topna_sezona) {
        espData[5] = 2;
      } else {
        if (atreaData01[6] >= 23+50)    // pokud je teplota vetsi nez 22, tak nechladi bypassem a jed pres rekuperator 
            espData[5] = 2;
        else if (espData[4] == 8) { // CV
          if (atreaData01[6] >= 13+50)
            espData[5] = 1;
          else if (atreaData01[6] <= 11+50)
            espData[5] = 2;
        } else if (espData[4] == 16) { // RV
          if (atreaData01[6] >= 16+50)
            espData[5] = 1;
          else if (atreaData01[6] <= 14+50)
            espData[5] = 2;
        }
      }
      
      espData[9] = crc8(espData, 9);

      timeEsp = ms;
      for (i = 0; i < 10; i++)
        write(espData[i]);
      ESP_LOGD("ESP", "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", espData[0], espData[1], espData[2], espData[3], espData[4], espData[5], espData[6], espData[7], espData[8], espData[9]);
    }

    while (available()) {
      if (readpacket(read(), buffer)) {
        if (memcmp(buffer, espData, 10) != 0) {  // neni echo vysilanych dat
          ESP_LOGD("LIN", "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9]);
          if (/*ms > (timeEsp + 1000) && */(buffer[1] >= 0 && buffer[1] <= 2) && (buffer[2] >= 0 && buffer[2] <= 3)) {
            if (buffer[8] == 0) {  // packet CP07
              timeCp = ms;
              if (cpData[9] != buffer[9])   // odlisne CRC
                zmenaCpText = zmenaCpBinSen = timeCp - MAXINTERVAL;
              memcpy(cpData, buffer, 10);
            } else {  // paket Atrea
              if (buffer[1] == 0 && buffer[2] == 1) {	  // packet Atrea01
                timeAtrea01 = ms;
                if (atreaData01[9] != buffer[9])   // odlisne CRC
                  zmenaAtreaText = zmenaAtreaSensor01 = zmenaAtreaBinSen01 = timeAtrea01 - MAXINTERVAL;
                memcpy(atreaData01, buffer, 10);
              } else if (buffer[1] == 0 && buffer[2] == 3) {	  // packet Atrea03
                timeAtrea03 = ms;
                if (atreaData03[9] != buffer[9])   // odlisne CRC
                  zmenaAtreaBinSen03 = timeAtrea03 - MAXINTERVAL;
                memcpy(atreaData03, buffer, 10);
              } else if (buffer[1] == 1 && buffer[2] == 3) {	  // packet Atrea13
                timeAtrea13 = ms;
                if (atreaData13[9] != buffer[9])   // odlisne CRC
                  zmenaAtreaSensor13 = timeAtrea13 - MAXINTERVAL;
                memcpy(atreaData13, buffer, 10);
              }
            }
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
      if (atreaData01[0] == 0xF5 && ms < (timeAtrea01 + 15000)) {
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
      
        if ((atreaData01[4] & 0x03) == 0)
          atrea_intenzita->publish_state("Vypnuto");
        else if ((atreaData01[4] & 0x03) == 1)
          atrea_intenzita->publish_state("Střední");
        else if ((atreaData01[4] & 0x03) == 2)
          atrea_intenzita->publish_state("Maximální");
        else
          atrea_intenzita->publish_state("???");

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
  Sensor *atrea_teplota_TI2 = new Sensor();
  Sensor *atrea_cirkulace = new Sensor();

  AtreaSensor() : PollingComponent(2000) { }

  void setup() override {
    // nothing to do here
  }

  void update() override {
    uint32_t ms = millis();

    if (ms > zmenaAtreaSensor01 + MAXINTERVAL) { // aktualizuj při změně nebo co MAXINTERVAL
      zmenaAtreaSensor01 = ms;    
      if (atreaData01[0] == 0xF5 && ms < (timeAtrea01 + 15000)) {
        if (atreaData01[6] > 10) {
            atrea_teplota_TE->publish_state(atreaData01[6] - 50.0);
            if ((atreaData01[6] - 50.0) > 25)
                topna_sezona = false;
            if ((atreaData01[6] - 50.0) < 0)
                topna_sezona = true;
        }
        if (atreaData01[7] > 30)
            atrea_teplota_TA->publish_state(atreaData01[7] - 50.0);
      }
    }
    if (ms > zmenaAtreaSensor13 + MAXINTERVAL) { // aktualizuj při změně nebo co MAXINTERVAL
      zmenaAtreaSensor13 = ms;    
      if (atreaData13[0] == 0xF5 && ms < (timeAtrea13 + 15000)) {
        if (atreaData13[5] > 40)
          atrea_teplota_TI2->publish_state(atreaData13[5] - 50.0);
        atrea_cirkulace->publish_state(atreaData13[3]*100/255);
      }
    }
  }
};

class AtreaBinarySensor : public PollingComponent {
 public:
  BinarySensor *cp07_bypass = new BinarySensor();
  BinarySensor *atrea_topi = new BinarySensor();
  BinarySensor *atrea_chladi = new BinarySensor();
  BinarySensor *atrea_narazove_vetrani = new BinarySensor();
  BinarySensor *atrea_fx = new BinarySensor();
  BinarySensor *atrea_D1 = new BinarySensor();
  BinarySensor *atrea_D2 = new BinarySensor();
  BinarySensor *atrea_D3 = new BinarySensor();
  BinarySensor *atrea_D4 = new BinarySensor();
  BinarySensor *atrea_klapka_SR = new BinarySensor();
  BinarySensor *atrea_bypass_SB = new BinarySensor();
  BinarySensor *atrea_YV = new BinarySensor();
  BinarySensor *atrea_K = new BinarySensor();
  BinarySensor *atrea_OC1 = new BinarySensor();
  BinarySensor *esp_aktivni_ovladac = new BinarySensor();
  BinarySensor *esp_bypass = new BinarySensor();

  AtreaBinarySensor() : PollingComponent(2000) { }

  void setup() override {
    // nothing to do here
  }

  void update() override {
    uint32_t ms = millis();

    if (ms > zmenaAtreaBinSen01 + MAXINTERVAL) { // aktualizuj při změně nebo co MAXINTERVAL    
      zmenaAtreaBinSen01 = ms;    
      if (atreaData01[0] == 0xF5 && ms < (timeAtrea01 + 15000)) {
        atrea_topi->publish_state(atreaData01[4] & 0x08);
        atrea_chladi->publish_state((atreaData01[3] == 5 && (atreaData01[4] & 0x03)) 
                                 || (atreaData01[3] == 8 && (atreaData01[4] & 0x03) && pozadavek_chlazeni)
                                 || (atreaData01[3] == 1 && (atreaData01[4] & 0x03) && (atreaData01[4] & 0x20)));
        narazove_vetrani = atreaData01[4] & 0x10;
        atrea_narazove_vetrani->publish_state(narazove_vetrani);
        atrea_fx->publish_state(atreaData01[4] & 0x20);
      }
    }

    if (ms > zmenaAtreaBinSen03 + MAXINTERVAL) { // aktualizuj při změně nebo co MAXINTERVAL    
      zmenaAtreaBinSen03 = ms;    
      if (atreaData03[0] == 0xF5 && ms < (timeAtrea03 + 15000)) {
        atrea_D1->publish_state(atreaData03[3] & 0x01);
        atrea_D2->publish_state(atreaData03[3] & 0x02);
        atrea_D3->publish_state(atreaData03[3] & 0x04);
        atrea_D4->publish_state(atreaData03[3] & 0x08);
        atrea_klapka_SR->publish_state(atreaData03[8] & 0x01);
        atrea_bypass_SB->publish_state(atreaData03[8] & 0x02);
        atrea_YV->publish_state(atreaData03[8] & 0x04);
        atrea_K->publish_state(atreaData03[8] & 0x08);
        atrea_OC1->publish_state(atreaData03[8] & 0x20);
      }
    }

    if (ms > zmenaCpBinSen + MAXINTERVAL) { // aktualizuj při změně nebo co MAXINTERVAL    
      zmenaCpBinSen = ms;
      if (cpData[0] == 0xF5 && ms < (timeCp + 15000)) {
        esp_aktivni_ovladac->publish_state(false);
        cp07_bypass->publish_state(cpData[5] == 1);
      } else {
        esp_aktivni_ovladac->publish_state(true);
        esp_bypass->publish_state(espData[5] == 1);
      }
    }
  }
};

