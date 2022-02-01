#ifndef conf_h
#define conf_h

#define FW_Ver 1.0 //16.05.17 added udp debug console

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif


// ------------------------------------------------------------- Include

#if defined(ESP8266)
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#elif CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <HTTPUpdateServer.h>
#include <ESP32SSDP.h>

#include <esp_int_wdt.h>
#include <esp_task_wdt.h>
#endif

#define ARDUINOJSON_USE_LONG_LONG 1

#include <pgmspace.h>
#include <ArduinoJson.h>

#include <my_wifi.h>
#include <Netwf.h>
#include <WiFiUdp.h>
#include <LittleFS.h>
#include <ArduinoOTA.h>

#include "web.h"

// ----------------------------------- Force define func name
void printFile(const char*);
void fs_setup();

// ----------------------------------- Web server
#if defined(ESP8266)
static ESP8266WebServer server(80);
static ESP8266HTTPUpdateServer httpUpdater;
#endif

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
static WebServer server(80);
HTTPUpdateServer httpUpdater;
#endif

#ifndef DBG_OUT_PORT
#define DBG_OUT_PORT Serial
#endif

#ifndef debug_level
#define debug_level 0
#endif

// ---------------------------------------------------- Common

const char *conf_f = "/conf_wifi.json";  // config file name

unsigned long   serv_ms = 60000;

IPAddress myIP;

NF nsys;
WF wifi;

wifi_data_t wifi_data;

#endif /*conf_h*/
