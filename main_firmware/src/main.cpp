#include <Arduino.h>
#include "display/lcd.h"
#include "sensor/hx711.h"
#include "data/master.h"

String codeDump = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  lcd.init();
  load.begin();
  String x = _name[5];
  Serial.println(x);
  x = _air[5];
  Serial.println(x);
  x = _energy[5];
  Serial.println(x);
  x = _karbo[5];
  Serial.println(x);
  x = _protein[5];
  Serial.println(x);
  x = _serat[5];
  Serial.println(x);
  x = _id[5];
  Serial.println(x);
  lcd.show(DASHBOARD_SCREEN);
  delay(1000);
  lcd.show(NUTRITION_WEIGHT_SCREEN);
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.touchUpdate();
  
  if(lcd.getKey() < 10){
    codeDump+=String(lcd.getKey());
    lcd.updateCode(codeDump);
  }

  if(lcd.getEnter()) {
    codeDump = "";
    lcd.updateCode(codeDump);
  }

  if(lcd.getDelete()){
    codeDump = codeDump.substring(0, codeDump.length()-1);
    lcd.updateCode(codeDump);
  }
  
  if(load.update()) {
    Serial.println(load.weight());
    lcd.updateValue(load.weight()); //block updateValue trus pencet f12 buat edit fungsinya
  }
  delay(200);
}