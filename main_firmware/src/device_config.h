#ifndef DEVICE_CONFIG_H_
#define DEVICE_CONFIG_H_

#include <Arduino.h>
#include <WiFi.h>
#include "display/lcd.h"
#include "sensor/hx711.h"
#include "data/_data.h"
#include "data/credentials.h"
#include "ble.h"

#define debug(x,y) Serial.print(x); Serial.print(" : "); Serial.println(y);

String typeCode;
bool state = false;
String code;
int count = 0;
String inputName;
String inputWeight, inputAir, inputEnergy, inputProtein, inputLemak, inputKarbo, inputSerat, inputCarbing, inputType, inputCode;
Food_type_t inputFood;
float rawWeight, rawAir, rawEnergy, rawProtein, rawLemak, rawKarbo, rawSerat, rawCarbing;
unsigned long lastRead;

float totalWeight, totalAir, totalEnergy, totalProtein, totalLemak, totalKarbo, totalSerat, totalCarbing;

float dumpPlate;
float lastWeight;

double _calibration;

#endif