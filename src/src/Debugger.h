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
#include <stdarg.h>
#include <Arduino.h>

namespace RetroverseBike {
    /** Hardware serial textual printer / debugger. */
    class Debugger {
public:
        /** Output buffer size / byte count. */
        static size_t const bufferByteCount = 1536;

        /** Serial baud rate. */
        unsigned int const serialBaudRate = 115200;
        /** Serial.begin timeout (ms). */
        unsigned int const timeoutMsSerialBegin = 500;
        /** Serial.read and Serial.write timeout (ms). */
        unsigned int const timeoutMsSerialIo = 100;

        /** Whether to be enabled. (Bike configs.) */
        bool enabled = false;

        /** Output buffer. */
        char buffer[bufferByteCount];

        /** Arduino setup callback. */
        void setup() {
            Serial.begin(serialBaudRate);
            unsigned long timeMsWaitBegin = millis();
            unsigned long timeMsWaited = millis() - timeMsWaitBegin;;

            while (!Serial && timeMsWaited <= timeoutMsSerialBegin) {
                // Feed watchdog timer in long-running loops.
                ESP.wdtFeed();

                delay(0);
                timeMsWaited = millis() - timeMsWaitBegin;
            } // end while

            Serial.setTimeout(timeoutMsSerialIo);
            
            if (!enabled) {
                return;
            } // end if

            Serial.printf("Debugger.setup\n");
        } // end void setup

        /** Prints a format string with arguments. */
        size_t printf(char const * format, ...) {
            if (!enabled) {
                return 0;
            } // end if

            va_list args;
            va_start(args, format);
            vsnprintf(buffer, bufferByteCount, format, args);
            va_end(args);
            buffer[bufferByteCount - 1] = '\0';

            size_t index = 0;

            while (index < bufferByteCount && buffer[index] != '\0') {
                // Feed watchdog timer in long-running loops.
                ESP.wdtFeed();

                Serial.print(buffer[index]);
                index += 1;
            } // end while

            size_t result = index + 1;
            return result;
        } // end size_t printf
    }; // end class Debug
} // end namespace RetroverseBike
