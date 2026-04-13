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
#include "Udp.h"
#include "WatchdogTimer.h"

namespace RetroverseBike {
    /** Tickers. */
    class Tickers {
public:
        /** Udp.sendPacket ticker maximum count. */
        static size_t const tickerUdpSendPacketCountMax = 4;

        // Other bike components (begin):

        /** Debugger reference. */
        static inline Debugger * debugger = nullptr;
        /** Watchdog timer reference. */
        static inline WatchdogTimer * watchdogTimer = nullptr;
        /** LED reference. */
        static inline Led * led = nullptr;
        /** Handlebar reference. */
        static inline Handlebar * handlebar = nullptr;
        /** Pedal reference. */
        static inline Pedal * pedal = nullptr;
        /** UDP reference. */
        static inline Udp * udp = nullptr;

        // : Other bike components (end)

        /** WatchdogTimer.feed interval (ms) reference. */
        static inline unsigned int const * intervalMsWatchdogTimerFeed = nullptr;
        /** WatchdogTimer.feed ticker. */
        static inline Ticker tickerWatchdogTimerFeed;

        /** Handlebar.refreshPosition interval (ms) reference. */
        static inline unsigned int const * intervalMsHandlebarRefreshPosition = nullptr;
        /** Handlebar.refreshPosition ticker. */
        static inline Ticker tickerHandlebarRefreshRotation;

        /** Pedal.refreshSpeed interval (ms) reference. */
        static inline unsigned int const * intervalMsPedalRefreshSpeed = nullptr;
        /** Pedal.refreshSpeed ticker. */
        static inline Ticker tickerPedalRefreshSpeed;

        /** Udp.sendPacket ticker count. */
        static inline size_t * tickerUdpSendPacketCount = nullptr;
        /** Udp.sendPacket interval (ms) reference. */
        static inline unsigned int * intervalMsUdpSendPacketList = nullptr;
        /** Udp.sendPacket ticker list. */
        static inline Ticker tickerUdpSendPacketList[tickerUdpSendPacketCountMax];

        /** Arduino setup callback. */
        static void setup() {
            debugger->printf("Tickers::setup\n");
            led->toggle();

            intervalMsWatchdogTimerFeed = &(watchdogTimer->intervalMsFeed);
            tickerWatchdogTimerFeed.attach_ms(*intervalMsWatchdogTimerFeed, watchdogTimerFeed);

            intervalMsHandlebarRefreshPosition = &(handlebar->intervalMsRefreshRotationIdeal);
            tickerHandlebarRefreshRotation.attach_ms(
                *intervalMsHandlebarRefreshPosition,
                handlebarRefreshRotation
            );

            intervalMsPedalRefreshSpeed = &(pedal->intervalMsRefreshSpeedIdeal);
            tickerPedalRefreshSpeed.attach_ms(
                *intervalMsPedalRefreshSpeed,
                pedalRefreshSpeed
            );

            tickerUdpSendPacketCount = &(udp->intervalMsSendPacketCount);
            intervalMsUdpSendPacketList = udp->intervalMsSendPacketList;

            for (size_t index = 0; index < *tickerUdpSendPacketCount; index += 1) {
                tickerUdpSendPacketList[index].attach_ms(
                    intervalMsUdpSendPacketList[index],
                    udpSendPacket
                );
            } // end for
        } // end static void setup

        /** watchdogTimer->feed ticker callback. */
        static void watchdogTimerFeed() {
            watchdogTimer->feed();
        } // end static void watchdogTimerFeed

        /** handlebar->refreshRotation ticker callback. */
        static void handlebarRefreshRotation() {
            handlebar->refreshRotation();
        } // end static void handlebarRefreshRotation

        /** pedal->refreshSpeed ticker callback. */
        static void pedalRefreshSpeed() {
            pedal->refreshSpeed();
        } // end static void pedalRefreshSpeed

        /** udp->sendPacket ticker callback. */
        static void udpSendPacket() {
            udp->sendPacket();
        } // end static void udpSendPacket
    }; // end class Interrupts
} // end namespace RetroverseBike
