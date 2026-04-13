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

// Include project files.
#include "Configs.h"
#include "Debugger.h"
#include "Handlebar.h"
#include "Interrupts.h"
#include "Led.h"
#include "Pedal.h"
#include "Tickers.h"
#include "Udp.h"
#include "WatchdogTimer.h"
#include "Wifi.h"

namespace RetroverseBike {
    /** Root container of the Retroverse Bike software. */
    class Bike {
public:
        /** Configurations used to determine behaviors of bike components. */
        Configs configs;

        // Bike components (begin):

        /** Watchdog timer component. */
        WatchdogTimer watchdogTimer;
        /** Debugger component. */
        Debugger debugger;
        /** LED component. */
        Led led;
        /** WiFi component. */
        Wifi wifi;
        /** UDP component. */
        Udp udp;
        /** Handlebar off-board component. */
        Handlebar handlebar;
        /** Pedal off-board component. */
        Pedal pedal;
        /** Interrupts software component. */
        Interrupts interrupts;
        /** Tickers software component. */
        Tickers tickers;

        // : Bike components (end)

        /** Arduino setup callback. */
        void setup() {
            setupWatchdogTimer();
            setupDebugger();
            debugger.printf("Bike.setup\n");
            setupLed();
            led.toggle();

            setupWifi();
            setupUdp();
            setupHandlebar();
            setupPedal();
            setupInterrupts();
            setupTickers();
        } // end void setup

        /**
        Arduino loop callback. (Thread-safe.)
        All functions / methods called here must be thread-safe.
        */
        void loop() {
            debugger.printf("Bike.loop\n");
            led.toggle();
            watchdogTimer.loop();
        } // end void loop

        /** Injects configuration into debugger and invoke its setup procedure. */
        void setupDebugger() {
            debugger.enabled = configs.debuggerEnabled;
            debugger.setup();
        } // end void setupDebugger

        /** Injects configuration into led and invoke its setup procedure. */
        void setupLed() {
            led.enabled = configs.ledEnabled;
            led.setup();
        } // end void setupLed

        /** Injects configuration into watchdogTimer and invoke its setup procedure. */
        void setupWatchdogTimer() {
            watchdogTimer.enabled = configs.watchdogTimerEnabled;
            watchdogTimer.setup();
        } // end void setupWatchdogTimer

        /** Injects configuration into wifi and invoke its setup procedure. */
        void setupWifi() {
            wifi.watchdogTimer = &watchdogTimer;
            wifi.debugger = &debugger;
            wifi.led = &led;

            wifi.ipLocalStatic = configs.wifiIpLocalStatic;
            wifi.gatewayLocalStatic = configs.wifiGatewayLocalStatic;
            wifi.subnetLocalStatic = configs.wifiSubnetLocalStatic;
            strncpy(wifi.ssid, configs.wifiSsid, Wifi::ssidByteCountMax);
            strncpy(wifi.password, configs.wifiPassword, Wifi::passwordByteCountMax);
            wifi.setup();
        } // end void setupWifi

        /** Injects configuration into udp and invoke its setup procedure. */
        void setupUdp() {
            udp.debugger = &debugger;
            udp.led = &led;
            udp.handlebar = &handlebar;
            udp.pedal = &pedal;

            udp.portLocal = configs.udpPortLocal;
            udp.ipRemote = configs.udpIpRemote;
            udp.portRemoteCount = configs.udpPortRemoteCount;
            
            for (size_t index = 0; index < configs.udpPortRemoteCount; index += 1) {
                udp.portRemoteList[index] = configs.udpPortRemoteList[index];
            } // end for

            udp.setup();
        } // end void setupUdp

        /** Injects configuration into handlebar and invoke its setup procedure. */
        void setupHandlebar() {
            handlebar.watchdogTimer = &watchdogTimer;
            handlebar.debugger = &debugger;
            handlebar.led = &led;

            handlebar.valueInversionEnabled = configs.handlebarValueInversionEnabled;
            handlebar.valueMultiplier = configs.handlebarValueMultiplier;
            handlebar.pinSerialRx = configs.handlebarPinSerialRx;
            handlebar.pinSerialTx = configs.handlebarPinSerialTx;
            handlebar.pinReset = configs.handlebarPinReset;
            handlebar.setup();
        } // end void setupHandlebar

        /** Injects configuration into pedal and invoke its setup procedure. */
        void setupPedal() {
            pedal.debugger = &debugger;
            pedal.led = &led;

            pedal.valueInversionEnabled = configs.pedalValueInversionEnabled;
            pedal.valueMultiplier = configs.pedalValueMultiplier;
            pedal.valueFilterCemaAlphaIdeal = configs.pedalValueFilterCemaAlphaIdeal;
            pedal.pinPulseA = configs.pedalPinPulseA;
            pedal.pinPulseB = configs.pedalPinPulseB;
            pedal.setup();
        } // end void setupPedal

        /** Injects configuration into Interrupts and invoke its setup procedure. */
        void setupInterrupts() {
            Interrupts::debugger = &debugger;
            Interrupts::led = &led;
            Interrupts::handlebar = &handlebar;
            Interrupts::pedal = &pedal;
            Interrupts::setup();
        } // end void setupInterrupts

        /** Injects configuration into Tickers and invoke its setup procedure. */
        void setupTickers() {
            Tickers::debugger = &debugger;
            Tickers::watchdogTimer = &watchdogTimer;
            Tickers::led = &led;
            Tickers::handlebar = &handlebar;
            Tickers::pedal = &pedal;
            Tickers::udp = &udp;
            Tickers::setup();
        } // end void setupTickers
    }; // end class Bike
} // end namespace RetroverseBike
