# ESP Atrea #
Replacement of the CP07 controller with the ESP8266 module for controlling the Atrea RK2 heat recovery unit and similar using Home Assistant. The module can either monitor or directly replace the CP07 controller via ESP8266, LIN interface and a Home Assistant. The module enables control of ventilation, heating and cooling.
![example view](atrea1.png)
![example view](atrea2.png)

### Communication
The CP07 controller communicates with the ATREA RK2 heat recovery unit via the LIN bus. It communicates at 300 Bauds and each packet has 10 bytes.
The packet format to the CP07 driver is:
0xF5 0x00 0x01 [mode 0,1,2,4,5,8,16] [intensity 0,1,2, heating 8, intensive ventilation 16, cooling] [errors states] [temp TE] [temp TA] x x [crc]
The packet shape to the Atrea is:
0xF5 0x00 0x01 [intensity 1,2 or 4] [mode 1,8,4,16] 0x02 [mode 0,1,2,3] 0x02 0x00 [crc]

### Hardware
For the connection use a LIN bus interface ('https://www.aliexpress.com/item/1005002376896294.html?spm=a2g0o.order_list.order_list_main.481.21ef1802IH1yTN', 'https://www.aliexpress.com/item/4001054538389.html?spm=a2g0o.order_list.order_list_main.486.21ef1802IH1yTN') and an ESP8266 module WeMos D1 mini, which is connected in parallel to the CP07 driver line. If the program detects the presence of the CP07 driver in the ESP8266 after startup, it will only listen to the ATREA system status. If the driver is not present, the module will take over its function. The CP07 driver is still required to configure the unit, but can be dispensed with after setup.

### Instalation
Programming of the module uses the ESPHome environment. The code is written in C and in YAML (espatrea.h espatrea.yaml).

![example view](atrea3.png)
