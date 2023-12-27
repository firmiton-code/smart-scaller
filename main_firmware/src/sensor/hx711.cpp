#include "hx711.h"

const int HX711_dout = 48; //mcu > HX711 dout pin
const int HX711_sck = 47; //mcu > HX711 sck pin
HX711_ADC LoadCell(HX711_dout, HX711_sck);

HX711Class *hx711_instances;

HX711Class::HX711Class(){
  hx711_instances = this;
}

void HX711Class::begin(){
   LoadCell.begin();

  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = 1036.96; // uncomment this if you want to set the calibration value in the sketch

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }
}

bool HX711Class::update(){
  if (LoadCell.update()) return true;
  else return false;
}

float HX711Class::weight(){
  return LoadCell.getData();
}

HX711Class load;