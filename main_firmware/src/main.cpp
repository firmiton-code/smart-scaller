#include <Arduino.h>
#include "display/lcd.h"
#include "sensor/hx711.h"
#include "data/master.h"

String codeDump = "";
int codeDump2 = 0;
int weightDump;
// int weightPerNutrient[];

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
  if(lcd.getScreen() == DASHBOARD_SCREEN && millis() - battScan > 30000){
    // int bat = random(1, 99);
    int bat = map(analogReadMilliVolts(7), 0.00, 3.30, 0.00, 100.00);
    if(bat > 70) lcd.updateBattery(BATTERY_FULL);
    else if(bat > 30 && bat <= 70) lcd.updateBattery(BATTERY_HALF);
    else lcd.updateBattery(BATTERY_LOW);
    battScan = millis();
  }

  if(lcd.getScreen() == DASHBOARD_SCREEN){
    if(lcd.touchUpdate()){
      if(lcd.getMode() == 1){
        lcd.show(NUTRITION_WEIGHT_SCREEN);
      } else if(lcd.getMode() == 2){
        lcd.show(UNIVERSAL_WEIGHT_SCREEN);
      } else if(lcd.getMode() == 3){
        lcd.show(SETTING_SCREEN);
      }
    }

  } else if(lcd.getScreen() == NUTRITION_WEIGHT_SCREEN){
    if(load.update()) {
      float weight_load = load.weight();
      if(weight_load < 0.00) weight_load = 0.00;
      Serial.println(weight_load);
      lcd.updateValue(weight_load);
    }

    if(lcd.touchUpdate()){
      if(lcd.getKey() < 10){
        if(codeDump2 < 1){
          codeDump2 = lcd.getKey();
        } else if(codeDump2 >= 1){
          codeDump2 = (codeDump2 * 10) + lcd.getKey();
        }
        lcd.updateCode(String(codeDump2));
        delay(500);
        // codeDump += String(lcd.getKey());
        // lcd.updateCode(codeDump);
      }

      if(lcd.getEnter()) {
        for(int x = 0; x <= sizeof(_id); x++){
          if(_id[x] == String(codeDump2)){
            Serial.println("Data Found");
            lcd.showFood(BANANA);
            codeDump2 = 0;
            lcd.updateCode(String(codeDump2));
            break;
          }

          if(x == sizeof(_id) && codeDump!=""){
            Serial.println("Data not found");
            lcd.showFood(UNKNOWN);
            codeDump2 = 0;
            lcd.updateCode(String(codeDump2));
          }
        }
      }

      if(lcd.getDelete()){
        codeDump2/=10;
        lcd.updateCode(String(codeDump2));
      }
      
      if(lcd.getTotal()){
        lcd.show(TOTAL_NUTRITION_SCREEN);
        lcd.showValue("10", "20", "30", "40", "50", "60");
      }

      if(lcd.getBack()){
        lcd.show(DASHBOARD_SCREEN);
        lcd.updateBattery(BATTERY_FULL);
      }
    }

  } else if(lcd.getScreen() == UNIVERSAL_WEIGHT_SCREEN){
    if(load.update()) {
      float weight_load = load.weight();
      if(weight_load < 0.00) weight_load = 0.00;
      Serial.println(weight_load);
      lcd.updateValue(weight_load);
    }
    
    if(lcd.touchUpdate()){
      if(lcd.getBack()){
        lcd.show(DASHBOARD_SCREEN);
        lcd.updateBattery(BATTERY_FULL);
      }
    }

  } else if(lcd.getScreen() == SETTING_SCREEN){
    if(lcd.touchUpdate()){
      if(lcd.getBack()){
        lcd.show(DASHBOARD_SCREEN);
        lcd.updateBattery(BATTERY_FULL);
      }
    }

  } else if(lcd.getScreen() == TOTAL_NUTRITION_SCREEN){
    if(lcd.touchUpdate()){
      if(lcd.getHome()){
        lcd.show(DASHBOARD_SCREEN);
        lcd.updateBattery(BATTERY_FULL);
      }
      
      if(lcd.getBack()){
        lcd.show(NUTRITION_WEIGHT_SCREEN);
      }
    }
  }
  
  delay(50);
}