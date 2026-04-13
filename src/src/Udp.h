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
#include <string.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <WiFiUdp.h>

// Include project files.
#include "Debugger.h"
#include "Handlebar.h"
#include "Led.h"
#include "Pedal.h"

namespace RetroverseBike {
    /** UDP */
    class Udp {
public:
        /** sendPacket interval (ms) maximum count. */
        static size_t const intervalMsSendPacketCountMax = 4;
        /** Remote port maximum count. */
        static size_t const portRemoteCountMax = 8;
        /** Outgoing packet buffer size / byte count. */
        static size_t const packetOutBufferByteCount = 1536;
        /** Outgoing packet format. */
        static inline char const * packetOutFormat =
            "Udp.packetIndex %ld "
            "Handlebar.rotationDegrees %.6f "
            "Pedal.speedRpsFilteredCurrent %.6f "
            "Handlebar.position %d "
            "Pedal.positionCurrent %ld "
            "Udp.timeMsSendPacket %ld\n"
        ;

        // Other bike components (begin):

        /** Debugger reference. */
        Debugger * debugger;
        /** LED reference. */
        Led * led;
        /** Handlebar reference. */
        Handlebar * handlebar;
        /** Pedal reference. */
        Pedal * pedal;

        // : Other bike components (end)
        // Bike configs (begin):

        /** Local port. */
        int portLocal = 50010;
        /** Targeted remote IP address. */
        IPAddress ipRemote{192, 168, 31, 240};
        /** Targeted remote port count. */
        size_t portRemoteCount = 3;
        /** Targeted remote port list. */
        int portRemoteList[portRemoteCountMax] = {40000, 50000, 60000};

        // : Bike configs (end)

        /** Local UDP connection. */
        WiFiUDP udpLocal;
        /** Packet index. */
        unsigned long packetIndex = 0L;
        /** sendPacket time (ms). */
        unsigned long timeMsSendPacket = 0L;
        /** Outgoing packet buffer. */
        char packetOutBuffer[packetOutBufferByteCount];
        /** sendPacket interval (ms) count. */
        size_t intervalMsSendPacketCount = 2;
        /** sendPacket interval (ms) list. */
        unsigned int intervalMsSendPacketList[intervalMsSendPacketCountMax] = {20, 25};

        /** Arduino setup callback. */
        void setup() {
            debugger->printf("Udp.setup\n");
            led->toggle();
            udpLocal.begin(portLocal);
        } // end void setup

        /** Sends UDP packet. */
        void sendPacket() {
            debugger->printf("Udp.sendPacket\n");
            led->toggle();

            timeMsSendPacket = millis();
            sniprintf(
                packetOutBuffer,
                packetOutBufferByteCount,
                packetOutFormat,
                packetIndex,
                handlebar->rotationDegrees,
                pedal->speedRpsFilteredCurrent,
                handlebar->position,
                pedal->positionCurrent,
                timeMsSendPacket
            );
            debugger->printf(
                "Udp.packetOutBuffer (begin):\n%s\n: Udp.packetOutBuffer (end)\n",
                packetOutBuffer
            );

            for (size_t index = 0; index < portRemoteCount; index += 1) {
                udpLocal.beginPacket(ipRemote, portRemoteList[index]);
                udpLocal.write(
                    (unsigned char const *) packetOutBuffer,
                    strnlen(packetOutBuffer, packetOutBufferByteCount)
                );
                // int sendingResult = udpLocal.endPacket();
                udpLocal.endPacket();
            } // end for

            packetIndex += 1;
        } // end void sendPacket
    }; // end class Udp
} // end namespace RetroverseBike
