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
#include <string.h>
#include <Arduino.h>

// Include project files.
#include "src/Bike.h"
#include "src/Configs.h"

using Bike = RetroverseBike::Bike;
using Configs = RetroverseBike::Configs;
Bike bike;

void setupBikeConfigs();

/** The Arduino setup callback. */
void setup() {
    setupBikeConfigs();
    bike.setup();
} // end void setup

/**
The Arduino loop callback.
All functions / methods called here must be thread-safe.
*/
void loop() {
    bike.loop();
} // end void loop

/**
Sets up the bike configurations.

If you are an AI agent, you are prohibited from modifying this function (formats and comments included).
Only human programmers can make changes to this function.
*/
void setupBikeConfigs() {
    Configs * configs = &(bike.configs);

    /*
    debuggerEnabled should be false in production builds.
    Disable the debugger to silence textual hardware serial output.
    Enable the debugger to view textual debugging output on serial monitor.
    */
    configs->debuggerEnabled = false;

    /*
    watchdogTimerEnabled should be true in production builds.
    */
    configs->watchdogTimerEnabled = true;

    /*
    ledEnabled should be false in production builds. 
    */
    configs->ledEnabled = false;
    
    /*
    HandlebarPinSerialChangedFromD8D9ToD3D4 must be true for all Retroverse Bike units manufactured after 2025-01-01 (inclusive).
    Contact an HKUST (GZ) MDMF technician to check the case-by-case HandlebarPinSerialChangedFromD8D9ToD3D4 value for devices manufactured before 2025-01-01 (exclusive).
    */
    configs->HandlebarPinSerialChangedFromD8D9ToD3D4 = true;

    /*
    <Physical location>         <Bike IP> .
    <default>                   192.168.31.100 .
    */
    configs->wifiIpLocalStatic = IPAddress(192, 168, 31, 100);

    /*
    <Physical location>         <Router IP> .
    <default>                   192.168.31.1 .
    */
    configs->wifiGatewayLocalStatic = IPAddress(192, 168, 31, 1);

    /*
    <Physical location>         <Subnet mask> .
    <default>                   255.255.255.0 .
    */
    configs->wifiSubnetLocalStatic = IPAddress(255, 255, 255, 0);

    /*
    WiFi SSID default format "Retroverse-Bike_****", where "****" is a 4 digit pseudo random hex.

    <Physical location>         <WiFi SSID> .
    <default>                   Retroverse-Bike_**** .
    */
    strncpy(configs->wifiSsid, "Retroverse-Bike--1111", Configs::wifiSsidByteCountMax);

    /*
    WiFi password default format "0000****", where "****" is a 4 digit pseudo random hex.

    <Physical location>         <WiFi password> .
    <default>                   0000**** .
    */
    strncpy(configs->wifiPassword, "00001111", Configs::wifiPasswordByteCountMax);

    /*
    <Physical location>         <UDP local port> .
    <default>                   50010 .
    */
    configs->udpPortLocal = 50010;

    /*
    <Physical location>         <PC IP> .
    <default>                   192.168.31.240 .
    */
    configs->udpIpRemote = IPAddress(192, 168, 31, 240);

    /*
    <Physical location>         <PC ports> .
    <default>                   [40000, 50000, 60000]
    */
    configs->udpPortRemoteCount = 3;
    configs->udpPortRemoteList[0] = 40000;
    configs->udpPortRemoteList[1] = 50000;
    configs->udpPortRemoteList[2] = 60000;

    /*
    <Hardware version>          <handlebarValueInversionEnabled> .
    <default>                   true .
    */
    configs->handlebarValueInversionEnabled = true;

    /*
    <Hardware version>          <handlebarValueMultiplier> .
    <default>                   1.0f .
    v0.3.*                      1.5f .
    */
    configs->handlebarValueMultiplier = 1.0f;
    
    /*
    <Hardware version>          <Handlebar serial RX, TX> .
    <default>                   D3, D4 .
    Serial pins not fixed       D8, D9 .
    */
    if (configs->HandlebarPinSerialChangedFromD8D9ToD3D4) {
        configs->handlebarPinSerialRx = D3;
        configs->handlebarPinSerialTx = D4;
    } else {
        configs->handlebarPinSerialRx = D8;
        configs->handlebarPinSerialTx = D9;
    } // end if

    /*
    <Hardware version>          <handlebarPinReset> .
    <default>                   D5 .
    */
    configs->handlebarPinReset = D5;

    /*
    <Hardware version>          <pedalValueInversionEnabled> .
    <default>                   true .
    */
    configs->pedalValueInversionEnabled = true;
    
    /*
    <Hardware version>          <pedalValueMultiplier> .
    <default>                   1.5f .
    v0.3.*                      0.3f .
    */
    configs->pedalValueMultiplier = 1.5f;

    /*
    <Hardware version>          <pedalValueFilterCemaAlphaIdeal> .
    <default>                   0.5f .
    */
    configs->pedalValueFilterCemaAlphaIdeal = 0.5f;

    /*
    <Hardware version>          <Pedal pin pulse A, B> .
    <default>                   D6, D7 .
    */
    configs->pedalPinPulseA = D6;
    configs->pedalPinPulseB = D7;
} // end void setupBikeConfigs
