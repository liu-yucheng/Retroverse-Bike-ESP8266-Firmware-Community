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

namespace RetroverseBike {
    /** Watchdog timer. */
    class WatchdogTimer {
public:
        /** Feed interval (ms). */
        unsigned int const intervalMsFeed = 500;

        /** Whether to enable the watchdog timer. (Bike configs.) */
        bool enabled = false;

        /** Whether someone is feeding the watchdog timer. */
        volatile bool feeding = false;

        /** Arduino setup callback. */
        void setup() {
            if (enabled) {
                *((volatile uint32_t *) 0x60000900) |= 0x00000001;
            } else {
                ESP.wdtDisable();
                *((volatile uint32_t *) 0x60000900) &= 0xfffffffe;
            }// end if
        } // end void setup

        /** Arduino loop callback. */
        void loop() {
            ESP.wdtFeed();
        } // end void loop

        /** Feeds the watchdog timer. (Thread-safe.) */
        void feed() {
            while (feeding) {
                delay(0);
            } // end while

            feeding = true;
            ESP.wdtFeed();
            feeding = false;
        } // end void feed
    }; // end class WatchdogTimer
} // end namespace RetroverseBike
