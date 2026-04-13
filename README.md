# Retroverse Bike ESP8266 Firmware (Community)

Community version of a Retroverse Bike PlatformIO / Arduino polyglot project that runs on ESP8266 MCU and WeMos D1 R1 board.

# Usage

## Usage as a PlatformIO Project

- Open the project root folder with [PlatformIO for VSCode](https://platformio.org/platformio-ide).
- Install the following **platforms**.
```
Espressif 8266
Espressif 32
```
- Install the following **libraries**.
```
ESPSoftwareSerial,
ESP Rotary
```
- Compile the firmware.
- Flash the compiled firmware to any ESP8266 MCU boards compatible with WeMos D1 R1.

## Usage as an Arduino Project

- Open `./src/src.ino` with [`Arduino IDE`](https://www.arduino.cc/en/software).
- Add the following **additional board manager URLs**.
```
http://arduino.esp8266.com/stable/package_esp8266com_index.json,
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```
- Install the following **boards**.
```
Arduino AVR Boards,
Arduino ESP32 Boards,
esp32,
esp8266
```
- Install the following **libraries**.
```
ESP Rotary,
ESPSoftwareSerial
```
- Compile the project files.
- Flash the compiled project files to any ESP8266 MCU boards compatible with WeMos D1 R1.

## ./Script-Utils/UDP_Packets_Receive.ps1

- Run with [`PowerShell`](https://github.com/PowerShell/PowerShell)
- Receives UDP packets from any remote IP and port to local port 50000.

## ./Script-Utils/UDP_Packets_Send.ps1

- Run with [`PowerShell`](https://github.com/PowerShell/PowerShell)
- Send UDP packets from local port 50010 to remote IP 192.168.31.240 port 50010.

# Copyright

```
Retroverse Bike ESP8266 Firmware.
Copyright (C) 2023-2026 Yucheng Liu. GNU AGPL 3.0 license.
GNU AGPL 3.0 License: https://www.gnu.org/licenses/agpl-3.0.txt .
```

# Open-source Software & Licenses

Developers of Retroverse Bike ESP8266 Firmware give credits to the following open-source software.

- [Arduino Core and Libraries.](https://www.arduino.cc/)
Copyright (C) 2008-2025 Arduino LLC. GNU LGPL3 License.

- [ESP8266 Arduino Core and Libraries.](https://github.com/esp8266/Arduino)
Copyright (C) 2014-2023 ESP8266 Community. GNU LGPL2.1 License.

- [ESPSoftwareSerial Arduino Libraries.](https://github.com/plerup/espsoftwareserial/)
Copyright (C) 2015-2024 Dirk Kaar, Peter Lerup. GNU LGPL2.1 License.

- [ESPRotary Arduino Libraries.](https://github.com/LennartHennigs/ESPRotary)
Copyright (C) 2017-2023 Lennart Hennigs. MIT License.
