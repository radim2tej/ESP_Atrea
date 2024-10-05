# ESP Atrea #
Replacement of the CP07 controller with the ESP8266 module for controlling the Atrea RK2 heat recovery unit and similar using Home Assistant. The module can either monitor or directly replace the CP07 controller via ESP8266, LIN bus interface and a Home Assistant. The module enables control of ventilation, heating and cooling.
![device](atrea4.png)

# Hardware
The CP07 controller communicates with the ATREA RK2 heat recovery unit via the LIN bus. It communicates at 300 Bauds and each packet has 10 bytes. The CP07 driver sends a query to the Atrea unit (the type is in bytes 2 and 3) and receives a response from Atrea.

For the connection use a LIN bus interface [TTL UART to LIN Can Bus Converter](https://www.aliexpress.com/item/4001054538389.html) or maybe [UART to LIN Bus Module](https://www.aliexpress.com/item/1005002376896294.html) and an ESP8266 module WeMos D1 mini, which is connected in parallel to the CP07 driver line. If the program detects the presence of the CP07 driver in the ESP8266 after startup, it will only listen to the ATREA system status. If the driver is not present, the module will take over its function. The CP07 driver is still required to configure the unit, but can be disconnect after setup ATREA.

![schema](schema.jpg)

# Logs CP07 (fw 54.07) <-> Atrea (PIC fw 53.59 for Heating pump)
## operating mode equal pressure ventilation (RV)
#### not heating season
    off ventilation
      [CP-07]: F5 00 01 01 10 02 01 00 00 7C
      [ATREA]: F5 00 01 00 00 00 3D 48 FB EC
    medium ventilation
      [CP-07]: F5 00 01 02 10 02 01 00 00 25
      [ATREA]: F5 00 01 10 01 00 3D 48 FB 7A
    max ventilation
      [CP-07]: F5 00 01 04 10 02 01 00 00 97
      [ATREA]: F5 00 01 10 02 00 3D 48 FB 34
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 10 12 00 3B 45 FB 10
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 32 00 3B 48 FB 61
#### heating season, not heating
    off ventilation
      [CP-07]: F5 00 01 01 10 02 01 02 00 ED 
      [ATREA]: F5 00 01 00 00 00 3D 47 FB F4
    medium ventilation
      [CP-07]: F5 00 01 02 10 02 01 02 00 B4 
      [ATREA]: F5 00 01 10 01 00 3D 48 FB 7A
    max ventilation
      [CP-07]: F5 00 01 04 10 02 01 02 00 06 
      [ATREA]: F5 00 01 10 02 00 3D 47 FB 2C
    medium / max shock ventilation
      [ATREA]: F5 00 01 10 32 00 3C 48 FB 1B
#### heating season, heating
    off ventilation (nonsense)
      [CP-07]: F5 00 01 01 10 02 01 03 00 29  
      [ATREA]: F5 00 01 00 00 00 3D 49 FB 28
    medium ventilation (electric boiler heating)
      [CP-07]: F5 00 01 02 10 02 01 03 00 70
      [ATREA]: F5 00 01 10 09 00 3D 49 FB 80
*    max ventilation
      [CP-07]: F5 00 01 04 10 02 01 03 00 C2
      [ATREA]: F5 00 01 10 3A 00 3D 54 FB 2F    // electric boiler heating
x    medium / max shock ventilation WC
      [ATREA]: F5 00 01 10 3A 00 3D 46 FB 52
      [ATREA]: F5 00 01 10 1A 00 3D 58 FB 9A    // electric boiler heating
    medium / max shock ventilation kitchen      
      [ATREA]: F5 00 01 10 3A 00 3C 47 FB 3D    // electric boiler heating

## operating mode circulation ventilation (CV)
#### not heating season
    off ventilation
      [CP-07]: F5 00 01 01 08 02 01 00 00 3E
*      [ATREA]: F5 00 01 00 20 00 3B 47 FB DD
    medium ventilation
      [CP-07]: F5 00 01 02 08 02 01 00 00 67
      [ATREA]: F5 00 01 08 01 00 3B 49 FB 95
    max ventilation
      [CP-07]: F5 00 01 04 08 02 01 00 00 D5
      [ATREA]: F5 00 01 08 02 00 3B 49 FB DB
 *   medium / max shock ventilation WC
      [ATREA]: F5 00 01 08 31 00 3B 47 FB CD
      [ATREA]: F5 00 01 08 32 00 3B 47 FB 83
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 32 00 3B 47 FB 79    // set max intensity and equal pressure ventilation
#### heating season, not heating 
    off ventilation
      [CP-07]: F5 00 01 01 08 02 01 02 00 AF 
      [ATREA]: F5 00 01 00 00 00 3D 48 FB EC
    medium ventilation
      [CP-07]: F5 00 01 02 08 02 01 02 00 F6 
      [ATREA]: F5 00 01 08 01 00 3D 48 FB 80
    max ventilation
      [CP-07]: F5 00 01 04 08 02 01 02 00 44 
      [ATREA]: F5 00 01 08 02 00 3D 48 FB CE
x    medium / max shock ventilation WC
      [ATREA]: F5 00 01 08 12 00 3D 49 FB 76
    medium / max shock ventilation kitchen
#### heating season, heating 
    off ventilation
      [CP-07]: F5 00 01 01 08 02 01 03 00 6B 
      [ATREA]: F5 00 01 00 00 00 3D 58 FB 00
    medium ventilation
      [CP-07]: F5 00 01 02 08 02 01 03 00 32
      [ATREA]: F5 00 01 08 09 00 3B 47 FB 77
    max ventilation
      [CP-07]: F5 00 01 04 08 02 01 03 00 80 
      [ATREA]: F5 00 01 08 0A 00 3D 5E FB B6
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 08 39 00 3C 54 FB 30
      [ATREA]: F5 00 01 08 3A 00 3C 56 FB EF
x    medium shock ventilation kitchen
      [ATREA]: F5 00 01 08 19 00 3D 49 FB 06
    max shock ventilation kitchen
      [ATREA]: F5 00 01 10 3A 00 3C 49 FB E1    // RV mode, electric boiler heating

## operating mode dependent circulation (CZ)
#### not heating season
    off ventilation
      [CP-07]: F5 00 01 01 04 02 02 00 00 FB
      [ATREA]: F5 00 01 00 20 00 3B 47 FB DD
    medium ventilation
      [CP-07]: F5 00 01 02 04 02 02 00 00 A2
      [ATREA]: F5 00 01 04 20 00 3C 4B FB 36    // standby
      [ATREA]: F5 00 01 08 21 00 3E 4A FB 0D    // circulation ventilation CV
    max ventilation
      [CP-07]: F5 00 01 04 04 02 02 00 00 10
      [ATREA]: F5 00 01 04 20 00 3B 4A FB 88
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 04 31 00 3B 49 FB 6C
      [ATREA]: F5 00 01 04 32 00 3B 49 FB 22
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 32 00 3B 49 FB A5
#### heating season, not heating
    off ventilation
      [CP-07]: F5 00 01 01 04 02 02 02 00 6A 
      [ATREA]: F5 00 01 00 20 00 3D 4C FB 2F    // 0x20 respond from 2nd 0x02 
    medium ventilation
      [CP-07]: F5 00 01 02 04 02 02 02 00 33 
      [ATREA]: F5 00 01 04 20 00 3C 4B FB 36    // standby
      [ATREA]: F5 00 01 08 21 00 3E 4A FB 0D    // circulation ventilation CV
    max ventilation
      [CP-07]: F5 00 01 04 04 02 02 02 00 81 
      [ATREA]: F5 00 01 04 20 00 3C 4B FB 36    // standby
      [ATREA]: F5 00 01 08? 22 00 3E 4A FB xxxx    // ventilation      
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 04 31 00 3C 4A FB 43
      [ATREA]: F5 00 01 04 32 00 3D 57 FB C3
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 32 00 3C 49 FB DF
#### heating season, heating
    off ventilation
      [CP-07]: F5 00 01 01 04 02 02 03 00 AE 
      [ATREA]: F5 00 01 00 20 00 3D 58 FB F8
    medium ventilation
      [CP-07]: F5 00 01 02 04 02 02 03 00 F7 
      [ATREA]: F5 00 01 04 29 00 3D 57 FB CF
    max ventilation
      [CP-07]: F5 00 01 04 04 02 02 03 00 45 
      [ATREA]: F5 00 01 04 2A 00 3D 55 FB 10
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 04 39 00 3C 53 FB 23
      [ATREA]: F5 00 01 04 3A 00 3D 55 FB 6C
    max shock ventilation kitchen      
      [ATREA]: F5 00 01 10 3A 00 3C 4E FB 8F    // RV mode, electric boiler heating??

## operating mode circulation (C)
#### not heating season
    off ventilation
      [CP-07]: F5 00 01 01 04 02 01 00 00 1F
      [ATREA]: F5 00 01 00 00 00 3B 46 FB E1
    medium ventilation
      [CP-07]: F5 00 01 02 04 02 01 00 00 46
      [ATREA]: F5 00 01 04 01 00 3B 4A FB BD
    max ventilation
      [CP-07]: F5 00 01 04 04 02 01 00 00 F4
      [ATREA]: F5 00 01 04 02 00 3B 49 FB A6
*    medium / max shock ventilation WC
      [ATREA]: F5 00 01 04 11 00 3B 48 FB 50
      [ATREA]: F5 00 01 04 12 00 3B 48 FB 1E
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 12 00 3B 47 FB 81    // set max intensity and equal pressure ventilation
#### heating season, not heating      
    off ventilation
      [CP-07]: F5 00 01 01 04 02 01 02 00 8E 
      [ATREA]: F5 00 01 00 00 00 3C 4C FB 7C
    medium ventilation
      [CP-07]: F5 00 01 02 04 02 01 02 00 D7 
      [ATREA]: F5 00 01 04 01 00 3C 4C FB 6D
    max ventilation
      [CP-07]: F5 00 01 04 04 02 01 02 00 65 
      [ATREA]: F5 00 01 04 02 00 3C 4B FB 4D
x    medium shock ventilation Wc
      [ATREA]: F5 00 01 04 11 00 3D 4A FB 10
    max shock ventilation WC
      [ATREA]: 
    medium / max shock ventilation kitchen
      [ATREA]: 
#### heating season, heating
    off ventilation
      [CP-07]: F5 00 01 01 04 02 01 03 00 4A 
      [ATREA]: F5 00 01 00 00 00 3C 5B FB FE
    medium ventilation
      [CP-07]: F5 00 01 02 04 02 01 03 00 13 
      [ATREA]: F5 00 01 04 09 00 3C 59 FB 40
    max ventilation
      [CP-07]: F5 00 01 04 04 02 01 03 00 A1 
      [ATREA]: F5 00 01 04 0A 00 3C 58 FB CA
    medium shock ventilation WC
      [ATREA]: F5 00 01 04 19 00 3D 4A FB 2E
    max shock ventilation WC
      [ATREA]: F5 00 01 04 1A 00 3C 54 FB FB
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 1A 00 3C 53 FB 12    // RV mode, electric boiler heating??

## operating mode pressure ventilation, not heating season (PV)
    off ventilation
      [CP-07]: F5 00 01 01 01 02 01 00 00 CD
      [ATREA]: F5 00 01 00 00 00 3B 46 FB E1
    medium ventilation
      [CP-07]: F5 00 01 02 01 02 01 00 00 94
      [ATREA]: F5 00 01 01 01 00 3B 49 FB 03
    max ventilation
      [CP-07]: F5 00 01 04 01 02 01 00 00 26
      [ATREA]: F5 00 01 01 02 00 3B 49 FB 4D
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 01 11 00 3B 49 FB 7F
      [ATREA]: F5 00 01 01 12 00 3B 47 FB ED
    medium / max shock ventilation kitchen
      [ATREA]: 
      
## operating mode cooling
#### not heating season, not cooling
    cooling off
      [CP-07]: F5 00 01 01 01 02 02 00 00 xx
      [ATREA]: F5 00 01 00 20 00 3B 44 FB 88
    medium cooling
      [CP-07]: F5 00 01 02 01 02 02 00 00 xx
      [ATREA]: F5 00 01 05 21 00 3D 42 FB D5
    max cooling
      [CP-07]: F5 00 01 04 01 02 02 00 00 xx
      [ATREA]: F5 00 01 05 22 00 3D 49 FB B8
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 01 31 00 3B 49 FB 87
      [ATREA]: F5 00 01 01 32 00 3B 49 FB C9
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 01 31 00 3B 44 FB 0E
#### not heating season, cooling
    cooling off
      [CP-07]: F5 00 01 01 01 02 02 01 00 ED
      [ATREA]: F5 00 01 00 20 00 3B 44 FB 88
    medium cooling
      [CP-07]: F5 00 01 02 01 02 02 01 00 B4
      [ATREA]: F5 00 01 05 21 00 3D 42 FB D5
    max cooling
      [CP-07]: F5 00 01 04 01 02 02 01 00 06
      [ATREA]: F5 00 01 05 22 00 3D 49 FB B8
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 01 31 00 3B 49 FB 87
      [ATREA]: F5 00 01 01 32 00 3B 49 FB C9
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 01 31 00 3B 44 FB 0E

## servis
    main menu
      [CP-07]: F5 00 01 02 10 02 01 02 00 B4
      [ATREA]: F5 00 01 10 32 00 3D 43 FB 93
      [CP-07]: F5 01 03 02 10 02 01 02 00 8D
      [ATREA]: F5 01 03 FF 43 43 3D 00 A9 C6
      [CP-07]: F5 02 03 02 10 02 01 02 00 48
      [ATREA]: F5 02 03 FF 33 82 EC FF FB 8C
      [CP-07]: F5 00 03 02 10 02 01 02 00 CE
      [ATREA]: F5 00 03 61 0A 00 00 00 81 7A
    outputs control
      [CP-07]: F5 43 01 00 00 00 00 00 00 17
      [ATREA]: F5 43 01 03 02 00 00 00 00 CD
      [CP-07]: F5 41 01 00 00 00 00 00 00 91
      [ATREA]: F5 41 01 12 11 0B 00 00 00 81
      [CP-07]: F5 42 01 00 00 00 00 00 00 54
      [ATREA]: F5 42 01 FF 33 82 EC FF 3B 75
      
## startup sequence
    [CP-07]: F5 00 01 01 10 02 01 00 00 7C
    [CP-07]: F5 00 01 01 10 02 01 00 00 7C
    [ATREA]: F5 00 01 00 00 00 00 00 FB CF
    [CP-07]: F5 00 01 01 10 02 01 00 00 7C
    [ATREA]: F5 00 01 1F 0F 00 00 00 FB DF
    [CP-07]: F5 00 01 01 10 02 01 00 00 7C
    [ATREA]: F5 00 01 1F 0F 00 00 00 FB DF
    [CP-07]: F5 00 01 01 10 02 01 00 00 7C
    [ATREA]: F5 00 01 1F 0F 00 00 00 FB DF
    [CP-07]: F5 00 01 01 10 02 01 00 00 7C
    [ATREA]: F5 00 01 1F 0F 00 00 00 FB DF
    [CP-07]: F5 00 01 01 10 02 01 00 00 7C
    [ATREA]: F5 00 01 1F 0F 00 00 00 FB DF
    [CP-07]: F5 00 01 01 10 02 01 00 00 7C
    [ATREA]: F5 00 01 1F 0F 00 00 00 FB DF
    [CP-07]: F5 00 01 01 10 02 01 00 00 7C
    [ATREA]: F5 00 01 1F 0F 00 00 00 FB DF
    [CP-07]: F5 00 01 01 10 02 01 00 00 7C
    [ATREA]: F5 00 01 1F 0F 00 3E 4B FB 4D
    [CP-07]: F5 52 01 01 10 02 01 00 00 E7
    [ATREA]: F5 52 01 32 4B 0F 35 00 00 6B
    [CP-07]: F5 53 01 01 10 02 01 00 00 A4
    [ATREA]: F5 53 01 37 2D 19 30 00 00 4A
    [CP-07]: F5 54 01 01 10 02 01 00 00 74
    [ATREA]: F5 54 01 32 0A 01 FF 00 00 F0
    [CP-07]: F5 52 01 01 10 02 01 00 00 E7
    [ATREA]: F5 52 01 32 4B 0F 35 00 00 6B
    [CP-07]: F5 53 01 01 10 02 01 00 00 A4
    [ATREA]: F5 53 01 37 2D 19 30 00 00 4A
    [CP-07]: F5 54 01 01 10 02 01 00 00 74
    [ATREA]: F5 54 01 32 0A 01 FF 00 00 F0
    [CP-07]: F5 54 01 01 10 02 01 00 00 74
    [ATREA]: F5 54 01 32 0A 01 FF 00 00 F0

![panel 1](atrea1.png)
![panel 2](atrea2.png)

# Decode packets
### operating mode
The requests packet from the CP07 controller to the Atrea unit:
- 0xF5 0x00 0x01 [intensity 1,2,4] [mode 1,2,4,8,16] 0x02 [flp 1,2] [temp 0,1,2,3] 0x00 [crc]
    - intensity is 1=off, 2=medium or 4=max
    - modes:
        - pressure ventilation: mode = 1, flp = 1, temp = 0
        - dependent circulation: mode = 4, flp = 2, temp = 3 or 0
        - circulation: mode = 4, flp = 1, temp = 3 or 0
        - circulation ventilation: mode = 8, flp = 1, temp = 3 or 2 (0 in summer?)
        - equal pressure ventilation: mode = 0x10, flp = 1, temp = 0, 2, 3
        - cooling (not cooling): mode = 1, flp = 2, temp = 0
        - cooling (cooling): mode = 1, flp = 2, temp = 1
    - flp: 0x01 or 0x02 = ? (flap)
    - temp: 0x01 = heating / cooling, 0x02 = heating season (for new fw CP-07?)
    - temp: 0, 1 = cooling during pressure ventilaton otherwise heating

The responds packet from the Atrea unit to the CP07 controller:
- 0xF5 0x00 0x01 [mode2 0,1,2,4,5,8,16] [flags: intensity [0,1,2], errorB 0x04, heating 0x08, shock vent. 0x10, ? 0x20] [errors] [TE] [TA] x [crc]
    - modes:
        - nothing: mode2 = 0, flags = 0 or 0x20
            - value 0x20 of flags possibly means state of the flaps (its similar as flp)
        - pressure ventilation: mode2 = 1, flags = 1 or 2
        - dependent circulation: mode2 = 2, flags = 0x20
        - circulation: mode2 = 4, flag = 1 or 2
        - circulation ventilation: mode2 = 8, flag = 1 or 2
        - equal pressure ventilation: mode2 = 16, flag = 0x21 or 0x22
        - cooling: mode2 = 5, flags = 0x21 or 0x22
        - heating: mode2 = 4 (circulation dep. and circulation), 8 (circulation vent.), 16 (equal press. vent.), flags = 9 (circulation) or 0x29 (other)
    - intensity: 0=off, 1=medium or 2=max
    - shock ventilation flag 0x10 (0x31 or 0x32 with intensity 1 or 2)
    - errorB: 0x04 = filter clogged
    - errors: 1=TE error, 2=TI2 error, 4=recuperator freezing, 8=TA error, 16=1st.freezing protection, 32=2nd.freezing protection, 64=active STOP, 128=communication error
    - outdoor temperature = TE-50
    - radiator temperature = TA-50

### servis menu
The requests packets from the CP07 controller to the ATREA unit:
- 0xF5 x x [intensity] [mode] 0x02 [flp] [temp] 0x00 [crc]
  
The responds packets from the ATREA unit to the CP07 controller:
- 0xF5 0x00 0x01 [mode2] [intensity, errorB, heating, shock vent., ?] [errors] [TE] [TA] x [crc]
- 0xF5 0x00 0x03 [0x60 + power inputs D1-D3 1,2,4] 0x0A 0x00 0x00 0x00 0x81 [crc]
- 0xF5 0x02 0x03 0xFF 0x33 0x82 0xEC 0xFF 0xCB [crc]
- 0xF5 0x01 0x03 0xFF [temp TI2 - behind recuperator] [temp TE - outdoor] 0x39 0x00 0x99 [crc]

### servis menu / outputs control
The requests packets from the CP07 controller to the ATREA unit:
- 0xF5 0x41 0x01 [circulation] [node DA2] [engine MC 0,1,2,3] [engine MV 0,1,2] [bypass 1, ZR 2, pump 4, boiler 8, output OC1 16, output EXT 32] 0x00 [crc]
- 0xF5 0x42 0x01 x x x x [bypass 1, ZR 2, pump 4, boiler 8, output OC1 0x10, output EXT 0x20] 0x00 [crc]
- 0xF5 0x43 0x01 x x x x [bypass 1, ZR 2, pump 4, boiler 8, output OC1 0x10, output EXT 0x20] 0x00 [crc]

The responds packets from the ATREA unit to the CP07 controller:
- 0xF5 0x41 0x01 [temp TA] [temp TI2] [temp TE] 0x00 0x00 0x00 [crc]
- 0xF5 0x42 0x01 0xFF 0x33 0x82 0xEC 0xFF 0x0B [crc]
- 0xF5 0x43 0x01 0x03 [input D1 D2 D3] 0x00 0x00 0x00 0x00 [crc]

# Instalation
Programming of the module uses the ESPHome environment. The code is written in C and in YAML (espatrea.h espatrea.yaml). The files are moved to configuration/esphome. The wifi password is in secrets.yaml . In espatrea.yaml, edit the API Key and OTA passwords.

![chart](atrea3.png)

# Examples of automations
Solar gain circulation
```
alias: VZT - cirkulace solárních ziskù
description: ""
trigger:
  - platform: state
    entity_id:
      - sun.sun
    attribute: azimuth
  - platform: numeric_state
    entity_id: sensor.pv_power
    for:
      hours: 0
      minutes: 10
      seconds: 0
    above: 2500
    enabled: false
condition:
  - condition: time
    after: "09:30:00"
    before: "17:00:00"
action:
  - if:
      - condition: numeric_state
        entity_id: sun.sun
        attribute: azimuth
        above: 140
        below: 220
      - condition: numeric_state
        entity_id: sensor.pv_power
        above: 2500
    then:
      - device_id: 904cd0b7d9147d7b3b4ad392bb80d9a8
        domain: select
        entity_id: select.esp_rezim_vzt
        type: select_option
        option: Cirkulace
    else:
      - device_id: 904cd0b7d9147d7b3b4ad392bb80d9a8
        domain: select
        entity_id: select.esp_rezim_vzt
        type: select_option
        option: Cirkulace závislá
mode: single
```

Control heating and cooling
```
alias: VZT - řízeni topení a chlazení
description: >-
  VZT nastavování teploty topení, blokování topení podle ceny, chlazení pøi
  nadbytku FVE
trigger:
  - platform: state
    entity_id:
      - sensor.final_price_kwh
    for:
      hours: 0
      minutes: 0
      seconds: 5
  - platform: state
    entity_id:
      - binary_sensor.atrea_narazove_vetrani
  - platform: state
    entity_id:
      - sensor.pv_power
    enabled: false
  - platform: time_pattern
    minutes: /20
action:
  - alias: topení
    if:
      - condition: state
        entity_id: climate.home_thermostat
        state: heat
    then:
      - if:
          - condition: or
            conditions:
              - condition: state
                entity_id: binary_sensor.count_offpeak_heat_cheapest_hours
                state: "on"
              - condition: state
                entity_id: binary_sensor.count_peak_heat_cheapest_hours
                state: "on"
        then:
          - data:
              value: 1
            target:
              entity_id: input_number.upravena_temperature
            alias: nastaví offset termostatu na 0,5 kvuli levne energii
            action: input_number.set_value
        else:
          - data:
              value: 0
            target:
              entity_id: input_number.upravena_temperature
            alias: vypnuti offsetu termostatu
            action: input_number.set_value
      - if:
          - condition: state
            entity_id: binary_sensor.count_expensive_hours
            state: "on"
        then:
          - device_id: 904cd0b7d9147d7b3b4ad392bb80d9a8
            domain: select
            entity_id: select.esp_intenzita
            type: select_option
            option: Vypnuto
        else:
          - device_id: 904cd0b7d9147d7b3b4ad392bb80d9a8
            domain: select
            entity_id: select.esp_intenzita
            type: select_option
            option: Střední
    else:
      - alias: kontrola nadbytku energie pro chlazeni
        if:
          - condition: and
            conditions:
              - condition: or
                conditions:
                  - condition: numeric_state
                    entity_id: sensor.pv_power
                    above: 2800
                  - condition: numeric_state
                    entity_id: sensor.export_s_tc
                    above: 900
              - condition: numeric_state
                entity_id: sensor.battery_state_of_charge
                above: 50
              - condition: numeric_state
                entity_id: sensor.solcast_pv_forecast_forecast_remaining_today
                below: sensor.battery_state_of_charge
                value_template: >-
                  {{ max(130 - 100 *
                  states('sensor.solcast_pv_forecast_forecast_remaining_today')
                  | float(default=50.0) / 14.4, 0) }}
                alias: bude vìtší výroba energie FVE k dobití baterie do 100%
        then:
          - data:
              hvac_mode: cool
            target:
              entity_id: climate.home_thermostat
            action: climate.set_hvac_mode
          - device_id: 904cd0b7d9147d7b3b4ad392bb80d9a8
            domain: select
            entity_id: select.esp_intenzita
            type: select_option
            option: Maximální
            enabled: false
        else:
          - data:
              hvac_mode: "off"
            target:
              entity_id: climate.home_thermostat
            action: climate.set_hvac_mode
          - device_id: 904cd0b7d9147d7b3b4ad392bb80d9a8
            domain: select
            entity_id: select.esp_intenzita
            type: select_option
            option: Støední
            enabled: false
mode: single
```

Control heatin and cooling bypass
```
alias: VZT - øízení klapky špajzu pro chlazení / topení
description: chlazení a topení ve špajzu
trigger:
  - platform: state
    entity_id:
      - binary_sensor.atrea_chlazeni
    for:
      hours: 0
      minutes: 1
      seconds: 0
  - platform: state
    entity_id:
      - binary_sensor.atrea_topeni
    for:
      hours: 0
      minutes: 1
      seconds: 0
  - platform: time_pattern
    minutes: /10
condition: []
action:
  - if:
      - condition: or
        conditions:
          - condition: state
            entity_id: binary_sensor.atrea_chlazeni
            state: "on"
          - condition: and
            conditions:
              - condition: state
                entity_id: binary_sensor.atrea_topeni
                state: "on"
              - condition: numeric_state
                entity_id: sensor.th_spiz_temperature
                below: 10
    then:
      - type: turn_on
        device_id: 7b6b2b2705a9a6d467216a1d3fba84c2
        entity_id: light.klapka_spajz_vypinac
        domain: light
      - type: turn_on
        device_id: 7b6b2b2705a9a6d467216a1d3fba84c2
        entity_id: light.klapka_spajz_vypinac_2
        domain: light
    else:
      - type: turn_off
        device_id: 7b6b2b2705a9a6d467216a1d3fba84c2
        entity_id: light.klapka_spajz_vypinac
        domain: light
      - type: turn_off
        device_id: 7b6b2b2705a9a6d467216a1d3fba84c2
        entity_id: light.klapka_spajz_vypinac_2
        domain: light
mode: single
```
