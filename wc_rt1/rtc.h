
#include <RtcDS3231.h>
#include <RtcDS1307.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

void rtc_init();
void set_alarm();
bool Alarmed();
void man_set_time(const RtcDateTime&);
void GetTime();

// ----------------------------------- interrupt
#if defined(ESP8266)
void IRAM_ATTR isr0();
#elif defined(__AVR__)
void ISR_ATTR isr0();
#define SQW 4 // Mega2560
#elif CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
void ARDUINO_ISR_ATTR isr0();
#endif


// marked volatile so interrupt can safely modify them and
// other code can safely read and modify them

// ----------------------------------- Конструктор DS3231
RtcDS3231<TwoWire> * ds3231;

// ----------------------------------- Конструктор DS1307
RtcDS1307<TwoWire> * ds1307;

// ----------------------------------- Конструктор DS1302
RtcDS1302<ThreeWire> * ds1302;
ThreeWire * myTWire;

volatile bool wasAlarmed_int = false;
static uint32_t prev_ms;
