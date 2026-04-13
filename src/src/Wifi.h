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
4. You are prohibited from using heap memory in this MCU programming project.
4.1. All memory allocation / declaration must be static.
4.2. All memory sizes must be known and fixed at compile time.
5. Keep in mind that this project runs on ESP8266 MCUs / WeMos D1 R1 boards.
5.1. If you want to migrate this project to other MCUs / boards, you should carefully rewrite the MCU-specific code.
*/

#pragma once

// Include C, C++, Arduino, ESP libraries.
#include <ESP8266WiFi.h>

// Include project files.
#include "Debugger.h"
#include "Led.h"
#include "WatchdogTimer.h"

namespace RetroverseBike {
    /** WiFi */
    class Wifi {
public:
        /** SSID maximum byte count. */
        static size_t const ssidByteCountMax = 32;
        /** Password maximum byte count. */
        static size_t const passwordByteCountMax = 64;
        
        /** Dynamic connection attempt timeout (ms). */
        unsigned int const timeoutMsDynamicAttempt = 7500;
        /** Static connection attempt timeout (ms). */
        unsigned int const timeoutMsStaticAttempt = 7500;

        // Other bike components (begin):

        /** Watchdog timer reference. */
        WatchdogTimer * watchdogTimer;
        /** Debugger reference. */
        Debugger * debugger;
        /** LED reference. */
        Led * led;

        // : Other bike components (end)
        // Bike configs (begin):

        /** Local self static IP address. */
        IPAddress ipLocalStatic{192, 168, 31, 100};
        /** Local gateway static IP address. */
        IPAddress gatewayLocalStatic{192, 168, 31, 1};
        /** Local subnet static IP address. */
        IPAddress subnetLocalStatic{255, 255, 255, 0};
        /** SSID. */
        char ssid[ssidByteCountMax] = "Retroverse-Bike--1111";
        /** Password. */
        char password[passwordByteCountMax] = "00001111";

        // : Bike configs (end)

        /** Arduino setup callback. */
        void setup() {
            debugger->printf("Wifi.setup\n");
            led->toggle();
            setupDynamic();
            setupStatic();            
        } // end void setup

        /** Starts dynamic (DHCP) connection attempt. */
        void setupDynamic() {
            if (WiFi.status() != WL_CONNECTED) {
                WiFi.begin(ssid, password);
            } // end if

            unsigned long timeMsWaitBegin = millis();
            unsigned long timeMsWaited = millis() - timeMsWaitBegin;

            while (WiFi.status() != WL_CONNECTED && timeMsWaited <= timeoutMsDynamicAttempt) {
                // Feed watchdog timer in long-running loops.
                watchdogTimer->feed();

                delay(0);
                timeMsWaited = millis() - timeMsWaitBegin;
            } // end while
        } // end void setupDynamic

        /** Starts static (manually specified IP) connection attempt. */
        void setupStatic() {
            if (WiFi.status() != WL_CONNECTED) {
                WiFi.config(ipLocalStatic, gatewayLocalStatic, subnetLocalStatic);
                WiFi.begin(ssid, password);
            } // end if

            unsigned long timeMsWaitBegin = millis();
            unsigned long timeMsWaited = millis() - timeMsWaitBegin;;

            while (WiFi.status() != WL_CONNECTED && timeMsWaited <= timeoutMsStaticAttempt) {
                // Feed watchdog timer in long-running loops.
                watchdogTimer->feed();

                delay(0);
                timeMsWaited = millis() - timeMsWaitBegin;
            } // end while
        } // end void setupStatic
    }; // end class Wifi
} // end namespace RetroverseBike
