#include <Arduino.h>
#include "data/master.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  String x = _name[5];
  Serial.println(x);
}

void loop() {
  // put your main code here, to run repeatedly:
}