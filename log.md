# Logs CP 07 (fw 54.07) <-> Atrea RK2 (PIC fw 53.59 for Heating pump)
## operating mode equal pressure ventilation (RV)
#### not heating season
    off ventilation
      [CP-07]: F5 00 01 01 10 02 01 00 00 7C
      [ATREA]: F5 00 01 00 00 00 3D 48 FB EC
    medium ventilation
      [CP-07]: F5 00 01 02 10 02 01 00 00 25 (F5 00 01 02 10 01 01 00 00 xx)
      [ATREA]: F5 00 01 10 01 00 3D 48 FB 7A
    max ventilation
      [CP-07]: F5 00 01 04 10 02 01 00 00 97 (F5 00 01 04 10 01 01 00 00 xx)
      [ATREA]: F5 00 01 10 02 00 3D 48 FB 34
    medium / max shock ventilation
      [ATREA]: F5 00 01 10 12 00 3B 45 FB 10
      [ATREA]: F5 00 01 10 32 00 3B 48 FB 61
#### heating season, not heating
    off ventilation
      [CP-07]: F5 00 01 01 10 02 01 02 00 ED 
      [ATREA]: F5 00 01 00 00 00 3D 47 FB F4
    medium ventilation
      [CP-07]: F5 00 01 02 10 02 01 02 00 B4 (F5 00 01 02 10 01 01 02 00 xx)
      [ATREA]: F5 00 01 10 01 00 3D 48 FB 7A
    max ventilation
      [CP-07]: F5 00 01 04 10 02 01 02 00 06 (F5 00 01 04 10 01 01 02 00 xx) 
      [ATREA]: F5 00 01 10 02 00 3D 47 FB 2C
    medium / max shock ventilation
      [ATREA]: F5 00 01 10 32 00 3C 48 FB 1B
#### heating season, heating
    off ventilation (nonsense)
      [CP-07]: F5 00 01 01 10 02 01 03 00 29  
      [ATREA]: F5 00 01 00 00 00 3D 49 FB 28
    medium ventilation
      [CP-07]: F5 00 01 02 10 02 01 03 00 70
      [ATREA]: F5 00 01 10 09 00 3D 49 FB 80    // electric boiler heating
      [ATREA]: F5 00 01 10 29 00 3C 59 FB 3F    // electric boiler heating
    max ventilation
      [CP-07]: F5 00 01 04 10 02 01 03 00 C2
      [ATREA]: F5 00 01 10 0A 00 3D 54 FB xx    // electric boiler heating
      [ATREA]: F5 00 01 10 2A 00 3C 5A FB 24    // electric boiler heating
    medium / max shock ventilation
      [ATREA]: F5 00 01 10 1A 00 3D 58 FB 9A    // electric boiler heating
      [ATREA]: F5 00 01 10 3A 00 3D 46 FB 52    // electric boiler heating

## operating mode circulation ventilation (CV)
#### not heating season
    off ventilation
      [CP-07]: F5 00 01 01 08 02 01 00 00 3E
      [ATREA]: F5 00 01 00 00 00 3B 47 FB xx
      [ATREA]: F5 00 01 00 20 00 3B 47 FB DD
    medium ventilation
      [CP-07]: F5 00 01 02 08 02 01 00 00 67 (F5 00 01 02 08 01 01 00 00 xx)
      [ATREA]: F5 00 01 08 01 00 3B 49 FB 95
    max ventilation
      [CP-07]: F5 00 01 04 08 02 01 00 00 D5 (F5 00 01 04 08 01 01 00 00 xx)
      [ATREA]: F5 00 01 08 02 00 3B 49 FB DB
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 08 31 00 3B 47 FB CD
      [ATREA]: F5 00 01 08 32 00 3B 47 FB 83
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 32 00 3B 47 FB 79    // RV mode
#### heating season, not heating 
    off ventilation
      [CP-07]: F5 00 01 01 08 02 01 02 00 AF 
      [ATREA]: F5 00 01 00 00 00 3D 48 FB EC
    medium ventilation
      [CP-07]: F5 00 01 02 08 02 01 02 00 F6 (F5 00 01 02 08 01 01 02 00 xx)
      [ATREA]: F5 00 01 08 01 00 3D 48 FB 80
    max ventilation
      [CP-07]: F5 00 01 04 08 02 01 02 00 44 (F5 00 01 04 08 01 01 02 00 xx)
      [ATREA]: F5 00 01 08 02 00 3D 48 FB CE
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 08 11 00 3D 49 FB 38
      [ATREA]: F5 00 01 08 12 00 3D 49 FB 76
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 12 00 3D 49 FB 8C    // RV mode
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
      [ATREA]: F5 00 01 08 19 00 3C 56 FB 59
      [ATREA]: F5 00 01 08 39 00 3C 54 FB 30
      [ATREA]: F5 00 01 08 1A 00 3C 58 FB CB
      [ATREA]: F5 00 01 08 3A 00 3C 56 FB EF
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 1A 00 3D 49 FB B2    // RV mode, electric boiler heating
      [ATREA]: F5 00 01 10 3A 00 3C 49 FB E1    // RV mode, electric boiler heating

## operating mode dependent circulation (CZ)
#### not heating season
    off ventilation
      [CP-07]: F5 00 01 01 04 02 02 00 00 FB
      [ATREA]: F5 00 01 00 20 00 3B 47 FB DD
    medium ventilation
      [CP-07]: F5 00 01 02 04 02 02 00 00 A2
      [ATREA]: F5 00 01 04 20 00 3C 4B FB 36    // standby
      [ATREA]: F5 00 01 08 21 00 3E 4A FB 0D    // ventilation (CV mode)
    max ventilation
      [CP-07]: F5 00 01 04 04 02 02 00 00 10
      [ATREA]: F5 00 01 04 20 00 3B 4A FB 88    // standby
      [ATREA]: F5 00 01 08 22 00 3E 4A FB xx    // ventilation (CV mode)
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 04 31 00 3B 49 FB 6C
      [ATREA]: F5 00 01 04 32 00 3B 49 FB 22
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 32 00 3B 49 FB A5    // RV mode
#### heating season, not heating
    off ventilation
      [CP-07]: F5 00 01 01 04 02 02 02 00 6A 
      [ATREA]: F5 00 01 00 20 00 3D 4C FB 2F
    medium ventilation
      [CP-07]: F5 00 01 02 04 02 02 02 00 33 
      [ATREA]: F5 00 01 04 20 00 3C 4B FB 36    // standby
      [ATREA]: F5 00 01 08 21 00 3E 4A FB 0D    // ventilation (CV mode)
    max ventilation
      [CP-07]: F5 00 01 04 04 02 02 02 00 81 
      [ATREA]: F5 00 01 04 20 00 3C 4B FB 36    // standby
      [ATREA]: F5 00 01 08 22 00 3E 4A FB xx    // ventilation (CV mode)
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 04 31 00 3C 4A FB 43
      [ATREA]: F5 00 01 04 32 00 3D 57 FB C3
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 32 00 3C 49 FB DF    // RV mode
#### heating season, heating
    off ventilation
      [CP-07]: F5 00 01 01 04 02 02 03 00 AE 
      [ATREA]: F5 00 01 00 20 00 3D 58 FB F8
    medium ventilation
      [CP-07]: F5 00 01 02 04 02 02 03 00 F7 
      [ATREA]: F5 00 01 04 29 00 3D 57 FB CF    // standby
      [ATREA]:                                  // ventilation with heating
    max ventilation
      [CP-07]: F5 00 01 04 04 02 02 03 00 45 
      [ATREA]: F5 00 01 04 2A 00 3D 55 FB 10    // standby
      [ATREA]:                                  // ventilation with heating
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 04 39 00 3C 53 FB 23
      [ATREA]: F5 00 01 04 3A 00 3D 55 FB 6C
    max shock ventilation kitchen      
      [ATREA]: F5 00 01 10 3A 00 3C 4E FB 8F    // RV mode, electric boiler heating

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
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 04 11 00 3B 48 FB 50
      [ATREA]: F5 00 01 04 12 00 3B 48 FB 1E
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 12 00 3B 47 FB 81    // RV mode
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
    medium / max shock ventilation Wc
      [ATREA]: F5 00 01 04 11 00 3D 4A FB 10
      [ATREA]: F5 00 01 04 12 00 3D 49 FB 0B
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 12 00 3D 49 FB 8C    // RV mode
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
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 04 19 00 3D 4A FB 2E
      [ATREA]: F5 00 01 04 1A 00 3C 54 FB FB
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 10 1A 00 3C 53 FB 12    // RV mode, electric boiler heating

## operating mode pressure ventilation (PV)
    off ventilation
      [CP-07]: F5 00 01 01 01 02 01 00 00 CD
      [ATREA]: F5 00 01 00 00 00 3B 46 FB E1
    medium ventilation
      [CP-07]: F5 00 01 02 01 02 01 00 00 94
      [ATREA]: F5 00 01 01 01 00 3B 49 FB 03
    max ventilation
      [CP-07]: F5 00 01 04 01 02 01 00 00 26
      [ATREA]: F5 00 01 01 02 00 3B 49 FB 4D
    medium / max shock ventilation
      [ATREA]: F5 00 01 01 11 00 3B 49 FB 7F
      [ATREA]: F5 00 01 01 12 00 3B 47 FB ED
      
## operating mode cooling
#### not cooling
    cooling off
      [CP-07]: F5 00 01 01 01 02 02 00 00 xx
      [ATREA]: F5 00 01 00 20 00 3B 44 FB 88
    medium cooling
      [CP-07]: F5 00 01 02 01 02 02 00 00 xx
      [ATREA]: F5 00 01 05 20 00 3D 42 FB xx
    max cooling
      [CP-07]: F5 00 01 04 01 02 02 00 00 xx
      [ATREA]: F5 00 01 05 20 00 3D 49 FB xx
    medium / max shock ventilation WC
      [ATREA]: F5 00 01 01 31 00 3B 49 FB 87
      [ATREA]: F5 00 01 01 32 00 3B 49 FB C9
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 01 31 00 3B 44 FB 0E
#### cooling
    cooling off
      [CP-07]: F5 00 01 01 01 02 02 01 00 ED
      [ATREA]: F5 00 01 00 20 00 3B 44 FB 88
    medium cooling
      [CP-07]: F5 00 01 02 01 02 02 01 00 B4
      [ATREA]: F5 00 01 05 21 00 3D 42 FB D5
    max cooling
      [CP-07]: F5 00 01 04 01 02 02 01 00 06
      [ATREA]: F5 00 01 05 22 00 3D 49 FB B8
    off / medium / max shock ventilation WC
      [ATREA]: F5 00 01 10 32 00 3C 4A FB 8A
      [ATREA]: F5 00 01 01 31 00 3B 49 FB 87
      [ATREA]: F5 00 01 01 32 00 3B 49 FB C9
    medium / max shock ventilation kitchen
      [ATREA]: F5 00 01 01 31 00 3B 44 FB 0E

## servis
    main menu
      [CP-07]: F5 00 01 02 10 02 01 02 00 B4
      [ATREA]: F5 00 01 10 32 00 3D 43 FB 93
      [CP-07]: F5 00 03 02 10 02 01 02 00 CE
      [ATREA]: F5 00 03 61 0A 00 00 00 81 7A
      [CP-07]: F5 00 01 02 10 02 01 02 00 B4
      [ATREA]: F5 00 01 10 32 00 3D 43 FB 93
      [CP-07]: F5 01 03 02 10 02 01 02 00 8D
      [ATREA]: F5 01 03 FF 43 43 3D 00 A9 C6
      [CP-07]: F5 00 01 02 10 02 01 02 00 B4
      [ATREA]: F5 00 01 10 32 00 3D 43 FB 93
      [CP-07]: F5 02 03 02 10 02 01 02 00 48
      [ATREA]: F5 02 03 FF 33 82 EC FF FB 8C
    main menu / outputs control
      [CP-07]: F5 41 01 00 00 00 00 00 00 91
      [ATREA]: F5 41 01 12 11 0B 00 00 00 81
      [CP-07]: F5 42 01 00 00 00 00 00 00 54
      [ATREA]: F5 42 01 FF 33 82 EC FF 3B 75
      [CP-07]: F5 43 01 00 00 00 00 00 00 17
      [ATREA]: F5 43 01 03 02 00 00 00 00 CD
      
## startup sequence after switch-on power
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
