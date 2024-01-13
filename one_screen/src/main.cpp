#include <Arduino.h>
#include "display/lcd.h"
#include "sensor/hx711.h"
#include "data/_data.h"
#include "data/_data2.h"

bool state = false;
String code;
int count = 0;
String inputName;
String inputWeight, inputAir, inputEnergy, inputProtein, inputLemak, inputKarbo, inputSerat, inputCarbing;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  lcd.init();
  lcd.show(NUTRITION_WEIGHT_SCREEN);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(lcd.touchUpdate()){
    if(lcd.getKey()<10){
      code += String(lcd.getKey());
      lcd.updateCode(code);
    }

    if(lcd.getEnter()){
      int rawCode = code.toInt();
      switch(rawCode){
        case 10001 :
          inputName = AR001[1];
          inputAir = AR001[2];
          inputEnergy = AR001[3];
          inputProtein = AR001[4];
          inputLemak = AR001[5];
          inputKarbo = AR001[6];
          inputSerat = AR001[7];
          inputCarbing = AR001[8];
          break;
        case 10002 :
          inputName = AR002[1];
          inputAir = AR002[2];
          inputEnergy = AR002[3];
          inputProtein = AR002[4];
          inputLemak = AR002[5];
          inputKarbo = AR002[6];
          inputSerat = AR002[7];
          inputCarbing = AR002[8];
          break;
        
        case 10003 :
          inputName = AR003[1];
          inputAir = AR003[2];
          inputEnergy = AR003[3];
          inputProtein = AR003[4];
          inputLemak = AR003[5];
          inputKarbo = AR003[6];
          inputSerat = AR003[7];
          inputCarbing = AR003[8];
          break;
        
        default :
          inputName = "Not Found";
          inputAir = 0.00;
          inputEnergy = 0.00;
          inputProtein = 0.00;
          inputLemak = 0.00;
          inputKarbo = 0.00;
          inputSerat = 0.00;
          inputCarbing = 0.00;
          break;
      }
    
      // for(int x = 0; x < sizeof(code_list)/sizeof(code_list[0]); x++){
      //   Serial.println(code_list[x][1]);
      //   // Serial.println(code_list[x]);
      // }
      // if(code.toInt() == 10001){
      //   Serial.println(data1[1]);
      // } else{
      //   Serial.println("not match!");
      // }
      
      float protein = inputProtein.toFloat() * 100.0;
      Serial.println(inputName);
      Serial.println(protein);
      code = "";
      lcd.show(NUTRITION_WEIGHT_SCREEN);
    }

    if(lcd.getDelete()){
      code = code.substring(0, code.length()-1);
      lcd.show(NUTRITION_WEIGHT_SCREEN);
      lcd.updateCode(String(code));
    }
    
    delay(250);
  }
}