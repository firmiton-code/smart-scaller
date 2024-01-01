#include <Arduino.h>
#include "display/lcd.h"
#include "sensor/hx711.h"
#include "data/master.h"

String codeDump = "";

unsigned long battScan = 0;
unsigned long weightScan = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  lcd.init();
  delay(2000);
  load.begin();
  pinMode(7, INPUT);
  lcd.show(DASHBOARD_SCREEN); lcd.updateBattery(BATTERY_FULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.touchUpdate();
  
  if(lcd.getScreen() == DASHBOARD_SCREEN){
    if(millis() - battScan > 30000){
      int bat = random(1, 99);
      // int bat = map(analogReadMilliVolts(7), 0.00, 3.30, 0.00, 100,00);
      if(bat > 70) lcd.updateBattery(BATTERY_FULL);
      else if(bat > 30 && bat <= 70) lcd.updateBattery(BATTERY_HALF);
      else lcd.updateBattery(BATTERY_LOW);
      battScan = millis();
    }

    if(lcd.getMode() == 1){
      lcd.show(NUTRITION_WEIGHT_SCREEN);
    } else if(lcd.getMode() == 2){
      lcd.show(UNIVERSAL_WEIGHT_SCREEN);
    } else if(lcd.getMode() == 3){
      lcd.show(SETTING_SCREEN);
    }

  } else if(lcd.getScreen() == NUTRITION_WEIGHT_SCREEN){
    if(lcd.getKey() < 10){
      codeDump += String(lcd.getKey());
      lcd.updateCode(codeDump);
    }

    if(lcd.getEnter()) {
      for(int x = 0; x <= sizeof(_id); x++){
        if(_id[x] == codeDump){
          Serial.println("Data Found");
          codeDump = "";
          lcd.updateCode("");
          break;
        }

        if(x == sizeof(_id) && codeDump!=""){
          Serial.println("Data not found");
          codeDump = "";
          lcd.updateCode("");
        }
      }
    }

    if(lcd.getDelete()){
      codeDump = "";
      lcd.updateCode(codeDump);
    }
    
    if(load.update()) {
      Serial.println(load.weight());
      lcd.updateValue(load.weight()); //block updateValue trus pencet f12 buat edit fungsinya
    }

    if(lcd.getBack()){
      lcd.show(DASHBOARD_SCREEN);
      lcd.updateBattery(BATTERY_FULL);
    }

  } else if(lcd.getScreen() == UNIVERSAL_WEIGHT_SCREEN){
    if(load.update()) {
      Serial.println(load.weight());
      lcd.updateValue(load.weight()); //block updateValue trus pencet f12 buat edit fungsinya
    }

    if(lcd.getBack()){
      lcd.show(DASHBOARD_SCREEN);
      lcd.updateBattery(BATTERY_FULL);
    }

  } else if(lcd.getScreen() == SETTING_SCREEN){
    if(lcd.getBack()){
      lcd.show(DASHBOARD_SCREEN);
      lcd.updateBattery(BATTERY_FULL);
    }
  }
  
  delay(50);
}