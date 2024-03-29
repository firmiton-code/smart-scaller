#ifndef HX711_h_
#define HX711_h_

#include <Arduino.h>
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

// const int calVal_eepromAdress = 0;
// unsigned long t = 0;

class HX711Class{
  public:
    HX711Class();
    void begin();
    bool update();
    float weight();
};

extern HX711Class load;

#endif