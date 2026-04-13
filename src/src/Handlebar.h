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
#include <functional>
#include <Arduino.h>
#include <SoftwareSerial.h>

// Include project files.
#include "Debugger.h"
#include "Led.h"
#include "WatchdogTimer.h"

namespace RetroverseBike {
    /** Baud rate get / set request to handlebar encoder. */
    class RequestBaudRate {
public:
        /** Size / byte count. */
        static size_t const byteCount = 8;
        /** Content. */
        static inline unsigned char const content[byteCount]
            = {0x01, 0x06, 0x00, 0x45, 0x00, 0x02, 0x19, 0xde}
        ;
        /** Timeout (ms). */
        static unsigned int const timeoutMs = 100;
    }; // end class RequestBaudRate

    /** Position reset request to handlebar encoder. */
    class RequestReset {
public:
        /** Size / byte count. */
        static size_t const byteCount = 13;
        /** Content. */
        static inline unsigned char const content[byteCount] = {
            0x01, 0x10, 0x00, 0x4a, 0x00, 0x02, 0x04, 0x00,
            0x00, 0x00, 0x00, 0x77, 0xe0
        };
        /** Timeout (ms). */
        static unsigned int const timeoutMs = 100;
    }; // end class RequestReset

    /** Position get request to handlebar encoder. */
    class RequestPosition {
public:
        /** Size / byte count. */
        static size_t const byteCount = 8;
        /** Content. */
        static inline unsigned char const content[byteCount]
            = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b}
        ;
        /** Timeout (ms). */
        static unsigned int const timeoutMs = 25;
    }; // end class RequestPosition

    /** Position get reply from handlebar encoder. */
    class ReplyPosition {
public:
        /** Size / byte count. */
        static size_t const byteCount = 9;
        /** Format. */
        static inline unsigned char const format[byteCount] = {
            0x01, 0x03, 0x04, 0x00, 0x00, 0x55, 0x41, 0x05,
            0x53
        };
        /** Position size / byte count. */
        static size_t const positionByteCount = 4;
        /** Position byte indexes. */
        static inline size_t const positionByteIndexes[positionByteCount]
            = {3, 4, 5, 6}
        ;
        /** Bits per byte. */
        static int const bitsPerByte = 8;
    }; // end class ReplyPosition

    /** Request to and reply from handlebar. */
    class RequestAndReply {
public:
        /** Reply buffer size / byte count. */
        static size_t const replyBufferByteCount = 16;

        /** Watchdog timer reference. */
        WatchdogTimer * watchdogTimer;
        /** Debugger reference. */
        Debugger * debugger;

        /** Handlebar serial reference. */
        SoftwareSerial * serial;

        /** Request content reference. */
        unsigned char const * requestContent;
        /** Request byte count. */
        size_t requestByteCount;
        /** Callback after request. */
        std::function<void()> callbackAfterRequest;
        /** Timeout (ms). */
        unsigned int timeoutMs;
        /** Reply buffer. */
        unsigned char replyBuffer[replyBufferByteCount];
        /** Reply byte count. */
        size_t replyByteCount;

        /** Prints an array in hexadecimal byte format. */
        void printHexForArray(unsigned char const * array, size_t byteCount) {
            for (size_t index = 0; index < byteCount; index += 1) {
                debugger->printf("%02x ", array[index]);
            } // end for
        } // end void printHexForArray

        /** Sends request to and receives reply from handelbar. */
        void sendAndReceive() {
            serial->write(requestContent, requestByteCount);
            debugger->printf("Request content: ");
            printHexForArray(requestContent, requestByteCount);
            debugger->printf("\n");
            callbackAfterRequest();
            unsigned long timeMsWaitBegin = millis();
            unsigned long timeMsWaited = millis() - timeMsWaitBegin;

            while (!serial->available() && timeMsWaited <= timeoutMs) {
                // Feed watchdog timer in long-running loops.
                watchdogTimer->feed();

                delay(0);
                timeMsWaited = millis() - timeMsWaitBegin;
            } // end while

            for (size_t index = 0; index < replyBufferByteCount && serial->available(); index += 1) {
                unsigned char byte_ = serial->read();
                replyBuffer[index] = byte_;
                replyByteCount = index + 1;
            } // end for

            debugger->printf("Reply content: ");
            printHexForArray(replyBuffer, replyByteCount);
            debugger->printf("\n");
        } // end void sendAndReceive
    }; // end class RequestAndReply

    /** Bike handlebar. */
    class Handlebar {
public:
        /** Rotation underflow detection threshold. */
        float const rotationThresholdUnderflow = -165.0f;
        /** Rotation overflow detection threshold. */
        float const rotationThresholdOverflow = 165.0f;
        /** Encoder position lower bound. */
        unsigned int const positionMin = 0x0;
        /** Encoder position upper bound. */
        unsigned int const positionMax = 0x3ff;
        /** Encoder position range. */
        unsigned int const positionRange = (positionMax + 1) - positionMin;
        /** Encoder serial baud rate. */
        unsigned int const serialBaudRate = 9600;
        /** Encoder serial mode. */
        EspSoftwareSerial::Config const serialMode = SWSERIAL_8E1;
        /** Encoder serial.begin timeout (ms). */
        unsigned int const timeoutMsSerialSetup = 500;
        /** refreshRotation ideal interval (ms). */
        unsigned int const intervalMsRefreshRotationIdeal = 50;

        // Other bike components (begin):

        /** Watchdog timer reference. */
        WatchdogTimer * watchdogTimer;
        /** Debugger reference. */
        Debugger * debugger;
        /** LED reference. */
        Led * led;

        // : Other bike components (end)
        // Bike configs (begin):

        /** Whether to flip sign of rotation value. */
        bool valueInversionEnabled = true;
        /** Rotation value multiplier. */
        float valueMultiplier = 1.0f;
        /** Serial RX pin. */
        unsigned char pinSerialRx = D3;
        /** Serial TX pin. */
        unsigned char pinSerialTx = D4;
        /** Reset pin. */
        unsigned char pinReset = D5;

        // : Bike configs (end)

        /** Previous rotation raw value. */
        float rotationRawPrevious = 0.0f;
        /** Current rotation raw value. */
        float rotationRawCurrent = 0.0f;
        /** Rotation portion out of bounds. */
        float rotationPortionOutofBounds = 0.0f;
        /** Rotation (degrees). */
        float rotationDegrees = 0.0f;
        /** Encoder position. */
        unsigned int position = 0;
        /** Encoder serial. */
        SoftwareSerial serial;
        /** Request and reply. */
        RequestAndReply requestAndReply;

        /** Arduino setup callback. */
        void setup() {
            debugger->printf("Handlebar.setup\n");
            led->toggle();
            pinMode(pinReset, INPUT_PULLUP);
            beginSerial();
            requestAndReply.watchdogTimer = watchdogTimer;
            requestAndReply.debugger = debugger;
            requestAndReply.serial = &serial;
            sendRequestBaudRate();
        } // void setup

        /** Initializes encoder serial. */
        void beginSerial() {
            pinMode(pinSerialRx, INPUT);
            pinMode(pinSerialTx, OUTPUT);
            serial.begin(serialBaudRate, serialMode, pinSerialRx, pinSerialTx);
        } // void beginSerial

        /** Sends baud rate get / set request. */
        void sendRequestBaudRate() {
            requestAndReply.requestContent = RequestBaudRate::content;
            requestAndReply.requestByteCount = RequestBaudRate::byteCount;
            requestAndReply.callbackAfterRequest = [this](){
                serial.begin(serialBaudRate, serialMode, pinSerialRx, pinSerialTx);
            };
            requestAndReply.timeoutMs = RequestBaudRate::timeoutMs;
            requestAndReply.sendAndReceive();
        } // end void sendRequestBaudRate

        /** Sends reset request. */
        void sendRequestReset() {
            requestAndReply.requestContent = RequestReset::content;
            requestAndReply.requestByteCount = RequestReset::byteCount;
            requestAndReply.callbackAfterRequest = [](){};
            requestAndReply.timeoutMs = RequestReset::timeoutMs;
            requestAndReply.sendAndReceive();
        } // end void sendRequestReset

        /** Sends position get request. */
        void sendRequestPosition() {
            requestAndReply.requestContent = RequestPosition::content;
            requestAndReply.requestByteCount = RequestPosition::byteCount;
            requestAndReply.callbackAfterRequest = [](){};
            requestAndReply.timeoutMs = RequestPosition::timeoutMs;
            requestAndReply.sendAndReceive();
        } // end void sendRequestPosition

        /** Refreshes rotation value. */
        void refreshRotation() {
            debugger->printf("Handlebar.refreshRotation\n");
            led->toggle();
            sendRequestPosition();
            parsePosition();
        } // end void refreshRotation

        /** Parses encoder position. */
        void parsePosition() {
            if (requestAndReply.replyByteCount != ReplyPosition::byteCount) {
                debugger->printf("Handlebar reply format unknown.\n");
                return;
            } // end if

            unsigned int positionRefreshed = 0;

            for (size_t index = 0; index < ReplyPosition::positionByteCount; index += 1) {
                int positionByteIndex = ReplyPosition::positionByteIndexes[index];
                unsigned char byte_ = requestAndReply.replyBuffer[positionByteIndex];
                size_t shiftLeftByteCount = ReplyPosition::positionByteCount - 1 - index;
                positionRefreshed |= byte_ << (shiftLeftByteCount * ReplyPosition::bitsPerByte);
            } // end for

            position = positionRefreshed;
            debugger->printf("Handlebar.position: %d\n", position);
            rotationRawPrevious = rotationRawCurrent;
            rotationRawCurrent = (float) position / (float) positionRange * 360.0f;
            debugger->printf("Handlebar.rotationRawCurrent: %.6f\n", rotationRawCurrent);

            if (rotationRawCurrent > 180.0f) {
                rotationRawCurrent -= 360.0f;
            } // end if

            if (
                rotationRawPrevious <= rotationThresholdUnderflow
                && rotationRawCurrent > rotationThresholdOverflow
            ) {
                rotationPortionOutofBounds -= 360.0f;
            } // end if

            if (
                rotationRawPrevious >= rotationThresholdOverflow
                && rotationRawCurrent < rotationThresholdUnderflow
            ) {
                rotationPortionOutofBounds += 360.0f;
            } // end if

            float rotationRefreshed = rotationRawCurrent + rotationPortionOutofBounds;

            if (valueInversionEnabled) {
                rotationRefreshed = -rotationRefreshed;
            } // end if

            rotationRefreshed *= valueMultiplier;
            rotationDegrees = rotationRefreshed;
            debugger->printf("Handlebar.rotationDegrees: %.6f\n", rotationDegrees);
        } // end void parsePosition
    }; // end class Handlebar
} // end namespace RetroverseBike
