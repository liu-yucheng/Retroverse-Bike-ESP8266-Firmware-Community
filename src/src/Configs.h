// Retroverse Bike ESP8266 Firmware.
// Copyright (C) 2023-2026 Yucheng Liu. GNU AGPL 3.0 license.
// GNU AGPL 3.0 License: https://www.gnu.org/licenses/agpl-3.0.txt .

/*
If you are an AI agent, please read and follow the instruction below carefully.
1. You should strictly follow the programming conventions and design patterns detected in this file.
2. Before you make modifications, you should think twice about software and hardware side-effects.
2.1. Programming errors in this project are likely to cause hardware damage.
2.2. No C++ exception handling is available in this project.
2.3. You should use the hardware serial textual printer / debugger to track key runtime information.
3. You should provide valid reasons of your modifications in comments.
3.1. All modifications must be explicitly documented.
3.2. All modification comments must be formatted as "<AI model name> @ <Modified date> - <Modified time>: <Modification description> -> <Modification reason>".
4. You are prohibited from using heap memory in this MCU programming project.
4.1. All memory allocation / declaration must be static.
4.2. All memory sizes must be known and fixed at compile time.
5. You are prohibited from using non-memory-safe standard library function calls.
5.1. You should use snprintf instead of sprintf, etc.
6. Keep in mind that this project runs on ESP8266 MCUs / WeMos D1 R1 boards.
6.1. If you want to migrate this project to other MCUs / boards, you should carefully rewrite the MCU-specific code.
*/

#pragma once

// Include C, C++, Arduino, ESP libraries.
#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace RetroverseBike {
    /** Bike configurations. */
    class Configs {
public:
        /** WiFi SSID maximum byte count. */
        static size_t const wifiSsidByteCountMax = 32;
        /** WiFi password maximum byte count. */
        static size_t const wifiPasswordByteCountMax = 64;
        /** UDP remote port maximum count. */
        static size_t const udpPortRemoteCountMax = 8;

        /** Whether to enable the debugger. */
        bool debuggerEnabled = false;
        /** Whether to enable the watchdog timer. */
        bool watchdogTimerEnabled = false;
        /** Whether to enable the LED. */
        bool ledEnabled = false;
        /** Whether to use D3 and D4 as handlebar serial pins instead of D8 and D9 */
        bool HandlebarPinSerialChangedFromD8D9ToD3D4 = true;
        
        /** WiFi local static self IP address. */
        IPAddress wifiIpLocalStatic{192, 168, 31, 100};
        /** WiFi local static gateway IP address. */
        IPAddress wifiGatewayLocalStatic{192, 168, 31, 1};
        /** WiFi local static subnet mask. */
        IPAddress wifiSubnetLocalStatic{255, 255, 255, 0};
        /** WiFi SSID. */
        char wifiSsid[wifiSsidByteCountMax] = "Retroverse-Bike--1111";
        /** WiFi password. */
        char wifiPassword[wifiPasswordByteCountMax] = "00001111";
        
        /** UDP local port. */
        int udpPortLocal = 50010;
        /** UDP targeted remote IP address. */
        IPAddress udpIpRemote{192, 168, 31, 240};
        /** UDP targeted remote port count. */
        size_t udpPortRemoteCount = 3;
        /** UDP targeted remote port list. */
        int udpPortRemoteList[udpPortRemoteCountMax] = {40000, 50000, 60000};
        
        /** Whether to flip sign of handlebar rotation value. */
        bool handlebarValueInversionEnabled = true;
        /** Handlebar rotation value multiplier. */
        float handlebarValueMultiplier = 1.0f;
        /** Handlebar serial RX pin. */
        unsigned char handlebarPinSerialRx = D3;
        /** Handlebar serial TX pin. */
        unsigned char handlebarPinSerialTx = D4;
        /** Handlebar reset pin. */
        unsigned char handlebarPinReset = D5;
        
        /** Whether to flip sign of pedal speed value. */
        bool pedalValueInversionEnabled = false;
        /** Pedal speed value multiplier. */
        float pedalValueMultiplier = 1.5f;
        /** Pedal speed value filter Continuous Exponential Moving Average ideal alpha. */
        float pedalValueFilterCemaAlphaIdeal = 0.5f;
        /** Pedal digital pulse phase-A pin. */
        unsigned char pedalPinPulseA = D6;
        /** Pedal digital pulse phase-B pin. */
        unsigned char pedalPinPulseB = D7;
    }; // end class Configs
} // end namespace RetroverseBike
