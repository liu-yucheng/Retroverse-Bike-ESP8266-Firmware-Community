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

// Include project files.
#include "Debugger.h"
#include "Handlebar.h"
#include "Led.h"
#include "Pedal.h"

namespace RetroverseBike {
    /** Interrupts. */
    class Interrupts {
public:
        // Other bike components (begin):

        /** Debugger reference. */
        static inline Debugger * debugger = nullptr;
        /** LED reference. */
        static inline Led * led = nullptr;
        /** Handlebar reference. */
        static inline Handlebar * handlebar = nullptr;
        /** Pedal reference. */
        static inline Pedal * pedal = nullptr;

        // : Other bike components (end)

        /** Pedal digital pulse-A pin reference. */
        static inline unsigned char * pedalPinPulseA = nullptr;
        /** Pedal digital pulse-B pin reference. */
        static inline unsigned char * pedalPinPulseB = nullptr;
        /** Handlebar reset pin reference. */
        static inline unsigned char * handlebarPinReset = nullptr;

        /** Arduino setup callback. */
        static void setup() {
            debugger->printf("Interrupts::setup\n");
            led->toggle();

            pedalPinPulseA = &(pedal->pinPulseA);
            pedalPinPulseB = &(pedal->pinPulseB);
            attachInterrupt(digitalPinToInterrupt(*pedalPinPulseA), pedalLoop, CHANGE);
            attachInterrupt(digitalPinToInterrupt(*pedalPinPulseB), pedalLoop, CHANGE);

            handlebarPinReset = &(handlebar->pinReset);
            attachInterrupt(digitalPinToInterrupt(*handlebarPinReset), handlebarSendRequestReset, CHANGE);
        } // end static void setup

        /** pedal->loop interrupt callback. */
        static void pedalLoop();

        /** handlebar->sendRequestReset interrupt callback. */
        static void handlebarSendRequestReset();
    }; // end class Interrupts

    /*
    IRAM_ATTR macro does not work for in-place function implementation.
    Need to write implementation outside of class definition.
    */

    IRAM_ATTR void Interrupts::pedalLoop() {
        pedal->loop();
    } // end IRAM_ATTR void Interrupts::pedalLoop

    /*
    IRAM_ATTR macro does not work for in-place function implementation.
    Need to write implementation outside of class definition.
    */

    IRAM_ATTR void Interrupts::handlebarSendRequestReset() {
        handlebar->sendRequestReset();
    } // end IRAM_ATTR void Interrupts::handlebarSendRequestReset
} // end namespace RetroverseBike
