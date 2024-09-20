# ESP Atrea #
Replacement of the CP07 controller with the ESP8266 module for controlling the Atrea RK2 heat recovery unit and similar using Home Assistant. The module can either monitor or directly replace the CP07 controller via ESP8266, LIN interface and a Home Assistant. The module enables control of ventilation, heating and cooling.
![example view](atrea4.png)

### Communication
The CP07 controller communicates with the ATREA RK2 heat recovery unit via the LIN bus. It communicates at 300 Bauds and each packet has 10 bytes.

The packet format to the CP07 driver is:
- 0xF5 0x00 0x01 [mode 0,1,2,4,5,8,16] [intensity 0,1,2, heating 8, intensive ventilation 16, cooling] [errors states] [temp TE] [temp TA] x [crc]

The packet format to the Atrea is:
- 0xF5 0x00 0x01 [intensity 1,2 or 4] [mode 1,8,4,16] 0x02 [mode 0,1,2,3] 0x02 0x00 [crc]

### Hardware
For the connection use a LIN bus interface (https://www.aliexpress.com/item/1005002376896294.html, https://www.aliexpress.com/item/4001054538389.html) and an ESP8266 module WeMos D1 mini, which is connected in parallel to the CP07 driver line. If the program detects the presence of the CP07 driver in the ESP8266 after startup, it will only listen to the ATREA system status. If the driver is not present, the module will take over its function. The CP07 driver is still required to configure the unit, but can be disconnect after setup ATREA.

![example view](atrea1.png)
![example view](atrea2.png)

### Instalation
Programming of the module uses the ESPHome environment. The code is written in C and in YAML (espatrea.h espatrea.yaml). The files are moved to configuration/esphome. The wifi password is in secrets.yaml . In espatrea.yaml, edit the API Key and OTA passwords.

![example view](atrea3.png)

### Examples of automations
Solar gain circulation
```
alias: VZT - cirkulace solárních zisků
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
alias: VZT - rizeni topení a chlazení
description: >-
  VZT nastavování teploty topení, blokování topení podle ceny, chlazení při
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
                alias: bude větší výroba energie FVE k dobití baterie do 100%
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
            option: Střední
            enabled: false
mode: single
```

Control heatin and cooling bypass
```
alias: VZT - řízení klapky špajzu pro chlazení / topení
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
