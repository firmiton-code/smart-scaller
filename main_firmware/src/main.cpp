#include <Arduino.h>
#include "display/lcd.h"
#include "sensor/hx711.h"
#include "data/master.h"

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
  lcd.show("dash");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(load.weight());
  lcd.touchUpdate();
  delay(100);
}