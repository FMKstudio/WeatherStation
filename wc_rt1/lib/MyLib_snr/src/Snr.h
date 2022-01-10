// Snr.h

#ifndef Snr_h
#define Snr_h

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <Adafruit_Sensor.h>
#include <Adafruit_Si7021.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_BMP280.h>
#include <AM2320.h>
#include <Adafruit_BME280.h>
#include <DHT.h>
#if defined(__xtensa__) ||  defined(__AVR__) || CONFIG_IDF_TARGET_ESP32C3
#include <DHT_U.h>
#endif

#include <Udt.h>

#include "OneWireNg_CurrentPlatform.h"
#include "drivers/DSTherm.h"
#include "utils/Placeholder.h"

#define PARASITE_POWER  false


#ifndef DBG_OUT_PORT
#define DBG_OUT_PORT Serial
#endif

#define SEALEVELPRESSURE_HPA (1013.25)

class SNR
{
public:

	void dht_preset(uint8_t, uint8_t);
	ram_data_t init(ram_data_t);
	snr_data_t read_snr(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, snr_data_t, snr_data_t, snr_data_t, wf_data_t);

private:
	//----------------------------------------------------------dht
#if defined(__xtensa__) ||  defined(__AVR__) || CONFIG_IDF_TARGET_ESP32C3
	DHT_Unified* dht;
#endif

#if defined(BOARD_RTL8710) || defined(BOARD_RTL8195A) || defined(BOARD_RTL8711AM)
	DHT* dht;
#endif
	uint32_t delayMS;

	//-------------------------------------------------------am2320
	AM2320 am23;

	//----------------------------------------------------------htu
	Adafruit_Si7021 si = Adafruit_Si7021();

	//----------------------------------------------------------bmp
	Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
	//----------------------------------------------------------bmp2
	Adafruit_BMP280 bmp2; // I2C

	//----------------------------------------------------------bme

	Adafruit_BME280 bme; // I2C

	float humi_read(uint8_t);
	float temp_read(uint8_t);
	float pres_read(uint8_t);
    void  ds_init(uint8_t);
    int   ds_read();

	Placeholder<OneWireNg_CurrentPlatform> _ow;

	bool printId(const OneWireNg::Id& id);

	int printScratchpad(const DSTherm::Scratchpad& scrpd);

protected:

};

//-------------------------------------------------------Sensor

/*
  Типы датчиков температуры, влажности, давления
  0 - нет
  1 - ThingSpeak
  2 - Внешний сервер 1
  3 - Внешний сервер 2
  4 - dht22
  5 - Встроеный в DS3231 датчик температуры
  6 - si7021
  7 - am2320
  8 - bmp180
  9 - bmp280
 10 - bme280

  bh1750 - датчик освещенности
*/


#endif //Snr_h