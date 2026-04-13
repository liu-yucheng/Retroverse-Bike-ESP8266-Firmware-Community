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
#include <cmath>
#include <Arduino.h>
#include <Ticker.h>

// Include external third-party libraries.
#include <ESPRotary.h>

// Include project files.
#include "Debugger.h"
#include "Led.h"

namespace RetroverseBike {
    /** Continuous Exponential Moving Average filter. */
    class FilterCema {
public:
        /** Filters current raw value with given parameters. */
        static void filter(
            float inAlphaIdeal,
            float inValueRawCurrent,
            float inValueFilteredPrevious,
            float inIntervalSActual,
            float inIntervalSIdeal,
            float * outAlphaActual,
            float * outFilteredCurrent
        ) {
            float rectifyingExponent = inIntervalSActual / inIntervalSIdeal;
            float _1MinusAlphaActual = pow(1 - inAlphaIdeal, rectifyingExponent);
            *outAlphaActual = 1 - _1MinusAlphaActual;
            *outFilteredCurrent
                = *outAlphaActual * inValueRawCurrent
                + _1MinusAlphaActual * inValueFilteredPrevious
            ;
        } // end static void filter
    }; // end class FilterCema

    /** Pedal. */
    class Pedal {
public:
        /** Encoder steps per click. */
        unsigned char const positionStepsPerClick = 4;
        /** Encoder increment. */
        int const positionIncrement = 1;
        /** Encoder reset value. */
        int const positionResetValue = 0;
        /** Encoder pulses per revolution. */
        int const positionPulsesPerRevolution = 100;
        /** Encoder position underflow detection threshold. */
        long const positionThresholdUnderflow = INT16_MIN / 2;
        /** Encoder position overflow detection threshold. */
        long const positionThresholdOverflow = INT16_MAX / 2;
        /** Encoder position lower bound. */
        long const positionBoundLower = INT16_MIN;
        /** Encoder position upper bound. */
        long const positionBoundUpper = INT16_MAX;
        /** Encoder speedup interval. */
        int const speedupIntervalMs = 75;
        /** Encoder speedup increment. */
        int const speedupIncrement = 5;
        /** Whether to enable encoder speedup. */
        bool const speedupEnabled = false;
        /** Milliseconds per second. */
        int const msPerS = 1000;
        /** refreshSpeed ideal interval (ms). */
        unsigned int const intervalMsRefreshSpeedIdeal = 50;
        /** refreshSpeed ideal interval (s). */
        float const intervalSRefreshSpeedIdeal = (float) intervalMsRefreshSpeedIdeal / (float) msPerS;

        // Other bike components (begin):

        /** Debugger reference. */
        Debugger * debugger;
        /** LED reference. */
        Led * led;

        // : Other bike components (end)
        // Bike configs (begin):

        /** Whether to flip sign of speed value. */
        bool valueInversionEnabled = false;
        /** Speed value multiplier. */
        float valueMultiplier = 1.5f;
        /** Speed value filter C-EMA ideal alpha. */
        float valueFilterCemaAlphaIdeal = 0.5f;
        /** Digital pulse phase-A pin. */
        unsigned char pinPulseA = D6;
        /** Digital pulse phase-B pin. */
        unsigned char pinPulseB = D7;

        // : Bike configs (end)

        /** Rotary encoder. */
        ESPRotary encoder;
        /** Speed value filter C-EMA actual alpha. */
        float valueFilterCemaAlphaActual = 0.5f;
        /** Previous speed raw value (RPS). */
        float speedRpsRawPrevious = 0.0f;
        /** Current speed raw value (RPS). */
        float speedRpsRawCurrent = 0.0f;
        /** Previous speed transformed value (RPS). */
        float speedRpsTransformedPrevious = 0.0f;
        /** Current speed transformed value (RPS). */
        float speedRpsTransformedCurrent = 0.0f;
        /** Previous speed filtered value (RPS). */
        float speedRpsFilteredPrevious = 0.0f;
        /** Current speed filtered value (RPS). */
        float speedRpsFilteredCurrent = 0.0f;
        /** Previous refreshSpeed time (ms). */
        unsigned long timeMsRefreshSpeedPrevious = 0L;
        /** Current refreshSpeed time (ms). */
        unsigned long timeMsRefreshSpeedCurrent = 0L;
        /** Actual refreshSpeed interval (ms). */
        unsigned long intervalMsRefreshSpeedActual = 0L;
        /** Previous encoder position. */
        long positionPrevious = 0L;
        /** Current encoder position. */
        long positionCurrent = 0L;
        /** Encoder position difference. */
        long positionDifference = 0L;
        /** Whether someone is looping encoder. */
        volatile bool looping = false;

        /** Arduino setup callback. */
        void setup() {
            debugger->printf("Pedal.setup\n");
            led->toggle();
            encoder.begin(pinPulseA, pinPulseB, positionStepsPerClick);
            encoder.setIncrement(positionIncrement);
            encoder.setSpeedupInterval(speedupIntervalMs);
            encoder.setSpeedupIncrement(speedupIncrement);
            encoder.enableSpeedup(speedupEnabled);
            pinMode(pinPulseA, INPUT_PULLUP);
            pinMode(pinPulseB, INPUT_PULLUP);
            timeMsRefreshSpeedCurrent = millis();
            positionCurrent = encoder.getPosition();
            speedRpsFilteredCurrent = 0.0f;
        } // end void setup

        /**
        Something similar to an Arduino loop callback.
        Updates pedal encoder position by tracking signals from pinPulseA and pinPulseB.
        Thread-safe.
        Should be used in ExclusiveOr(loop, Ticker.attach_ms) but not both.
        */
        void loop() {
            debugger->printf("Pedal.loop\n");
            led->toggle();

            while (looping) {
                delay(0);
            } // end while

            looping = true;
            int position = encoder.getPosition();

            if (position == positionBoundUpper) {
                encoder.resetPosition(positionBoundLower);
            } else if (position == positionBoundLower) {
                encoder.resetPosition(positionBoundUpper);
            } // end if

            encoder.loop();
            looping = false;
        } // end void loop

        /** Refreshes speed. */
        void refreshSpeed() {
            debugger->printf("Pedal.refreshSpeed\n");
            led->toggle();
            parsePosition();
            calculateSpeed();
        } // end void refreshSpeed

        /** Parses encoder position. */
        void parsePosition() {
            timeMsRefreshSpeedPrevious = timeMsRefreshSpeedCurrent;
            timeMsRefreshSpeedCurrent = millis();
            intervalMsRefreshSpeedActual = timeMsRefreshSpeedCurrent - timeMsRefreshSpeedPrevious;
            debugger->printf("Pedal.intervalMsRefreshSpeedActual: %ld\n", intervalMsRefreshSpeedActual);

            if (!intervalMsRefreshSpeedActual) {
                intervalMsRefreshSpeedActual = intervalMsRefreshSpeedIdeal;
            } // end if

            positionPrevious = positionCurrent;
            positionCurrent = encoder.getPosition();
            debugger->printf("Pedal.positionCurrent: %ld\n", positionCurrent);
            positionDifference = positionCurrent - positionPrevious;

            if (
                positionPrevious < positionThresholdUnderflow
                && positionCurrent > positionThresholdOverflow
            ) {
                positionDifference 
                    = (positionBoundUpper - positionCurrent)
                    + (positionPrevious - positionBoundLower)
                ;
            } else if (
                positionPrevious > positionThresholdOverflow
                && positionCurrent < positionThresholdUnderflow
            ) {
                positionDifference
                    = (positionCurrent - positionBoundLower)
                    + (positionBoundUpper - positionPrevious)
                ;
            } // end if
        } // end void parsePosition

        /** Calculates speed. */
        void calculateSpeed() {
            speedRpsRawPrevious = speedRpsRawCurrent;
            speedRpsTransformedPrevious = speedRpsTransformedCurrent;
            speedRpsFilteredPrevious = speedRpsFilteredCurrent;
            
            float intervalSIdeal = intervalSRefreshSpeedIdeal;
            float intervalSActual = (float) intervalMsRefreshSpeedActual / (float) msPerS;
            debugger->printf("Pedal.intervalSActual: %.6f\n", intervalSActual);
            float revolutions = (float) positionDifference / (float) positionPulsesPerRevolution;
            speedRpsRawCurrent = revolutions / intervalSActual;
            debugger->printf("Pedal.speedRpsRawCurrent: %.6f\n", speedRpsRawCurrent);
            
            float speedRpsTransformedCurrentRefreshed = speedRpsRawCurrent;

            if (valueInversionEnabled) {
                speedRpsTransformedCurrentRefreshed = -speedRpsTransformedCurrentRefreshed;
            } // end if

            speedRpsTransformedCurrentRefreshed *= valueMultiplier;
            speedRpsTransformedCurrent = speedRpsTransformedCurrentRefreshed;
            debugger->printf("Pedal.speedRpsTransformedCurrent: %.6f\n", speedRpsTransformedCurrent);

            FilterCema::filter(
                valueFilterCemaAlphaIdeal,
                speedRpsTransformedCurrent,
                speedRpsFilteredPrevious,
                intervalSActual,
                intervalSIdeal,
                &valueFilterCemaAlphaActual,
                &speedRpsFilteredCurrent
            );
            debugger->printf("Pedal.valueFilterCemaAlphaActual: %.6f\n", valueFilterCemaAlphaActual);
            debugger->printf("Pedal.speedRpsFilteredCurrent: %.6f\n", speedRpsFilteredCurrent);
        } // end void calculateSpeed
    }; // end class Pedal
} // end namespace RetroverseBike
