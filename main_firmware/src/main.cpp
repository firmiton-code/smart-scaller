#include <Arduino.h>
#include <SPI.h>
#include "TFT_eSPI.h"
#include "data/master.h"

#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

const int HX711_dout = 48; //mcu > HX711 dout pin
const int HX711_sck = 47; //mcu > HX711 sck pin

HX711_ADC LoadCell(HX711_dout, HX711_sck);
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

const int calVal_eepromAdress = 0;
unsigned long t = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  tft.init();
  LoadCell.begin();
  String x = _name[5];
  Serial.println(x);
}

void loop() {
  // put your main code here, to run repeatedly:
}