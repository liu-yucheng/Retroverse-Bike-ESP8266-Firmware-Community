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
#include <Arduino.h>

namespace RetroverseBike {
    /** LED */
    class Led {
public:
        /** Whether to enable LED. (Bike configs.) */
        bool enabled = false;

        /** Whether LED is already turned on. */
        bool turnedOn = false;
        /** Whether someone is toggling LED. */
        volatile bool toggling = false;

        /** Arduino setup callback. */
        void setup() {
            if (!enabled) {
                return;
            } // end if

            pinMode(LED_BUILTIN, OUTPUT);
            digitalWrite(LED_BUILTIN, HIGH);
        } // end void setup

        /** Toggles LED. (On to off. Off to on. Thread-safe.) */
        void toggle() {
            if (!enabled) {
                return;
            } // end if

            while (toggling) {
                delay(0);
            } // end while

            toggling = true;

            if (!turnedOn) {
                digitalWrite(LED_BUILTIN, LOW);
                turnedOn = true;
            } else {
                digitalWrite(LED_BUILTIN, HIGH);
                turnedOn = false;
            } // end if

            toggling = false;
        } // end void toggle
    }; // end class Led
} // end namespace RetroverseBike
