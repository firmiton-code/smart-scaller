#include <Arduino.h>
#include "device_config.h"

//comment for real measurement
bool RANDOM_TEST = false;
unsigned long sensor_speed = 100;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        debug("MAIN", "Device Connected");
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        debug("MAIN", "Device Disconnected");
        pServer->startAdvertising();
    }
};

float updateWeight(){
  float weight_dump = 0.00;
  if(load.update()) {
    weight_dump = load.weight();
    if(weight_dump < 0.00) weight_dump = 0.00;
  }

  if(RANDOM_TEST) weight_dump = random(0.00, 9999.00);
  Serial.println(weight_dump);

  return weight_dump;
}

void updateWeightTask(void *param){
  while(1){
    if(deviceConnected){
      String a = String(rawWeight);
      WeightCharacteristic->setValue(a.c_str());
      WeightCharacteristic->notify();

      // int battRand = random(1, 100);
      int battRand = bat;
      String b = String(battRand);
      BattMonCharacteristic->setValue(b.c_str());
      BattMonCharacteristic->notify();
    }

    vTaskDelay(250 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF); 

  lcd.init();
  load.begin();

  BLEDevice::init(device_name.c_str());

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *WeightService = pServer->createService(WEIGHT_SERVICE_UUID);
  WeightCharacteristic = WeightService->createCharacteristic(
                      WEIGHT_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                      );
  WeightCharacteristic->addDescriptor(new BLE2902());
  
  BLEService *BattMonService = pServer->createService(BATT_UUID);
  BattMonCharacteristic = BattMonService->createCharacteristic(
                      BATT_C_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                      );
  BattMonCharacteristic->addDescriptor(new BLE2902());

  WeightService->start();
  BattMonService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(WEIGHT_SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();

  Serial.println("Bluetooth Created!");

  pinMode(7, INPUT);
  
  ledcSetup(0, 4096, 14);
  ledcAttachPin(42, 0);
  ledcWriteTone(0, 0);
  ledcWrite(0, 4096);

  _calibration = data.read("cal").toFloat();

  lcd.show(DASHBOARD_SCREEN);

  xTaskCreate(updateWeightTask, "Update Weight", 5000, NULL, 3, NULL);
  // xTaskCreate(updateLCDTask, "Update LCD", 10000, NULL, 4, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(lcd.touchUpdate()){
    ledcWriteTone(0, 2048);
    
    if(lcd.getScreen() == MENU_1_SCREEN){
      if(lcd.getChoose() == 1){
        typeCode = "14";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(CEREALIA);
      } else if(lcd.getChoose() == 2){
        typeCode = "16";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(ROOTS);
      } else if(lcd.getChoose() == 3){
        typeCode = "13";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(NUTS);
      } else if(lcd.getChoose() == 4){
        typeCode = "15";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(VEGETABLE);
      } 
      // else{
      //   lcd.show(NUTRITION_WEIGHT_SCREEN);
      //   delay(50);
      //   lcd.showFood(UNKNOWN);

      //   code = "";
      //   lastWeight = 0;
      //   totalWeight = 0;
      //   totalAir = 0;
      //   totalCarbing = 0;
      //   totalEnergy = 0;
      //   totalKarbo = 0;
      //   totalLemak = 0;
      //   totalProtein = 0;
      //   totalSerat = 0;
      // }

      if(lcd.getMenu() == 1){
        lcd.show(MENU_4_SCREEN);
      } else if(lcd.getMenu() == 2){
        lcd.show(MENU_2_SCREEN);
      }

      if(lcd.getBack()){
        lastWeight = 0;
        lcd.show(DASHBOARD_SCREEN);
      }
      delay(50);

    } else if(lcd.getScreen() == MENU_2_SCREEN){
      if(lcd.getChoose() == 1){
        typeCode = "15";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(VEGETABLE);
      } else if(lcd.getChoose() == 2){
        typeCode = "34";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(OIL);
      } else if(lcd.getChoose() == 3){
        typeCode = "25";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(SUGAR);
      } else if(lcd.getChoose() == 4){
        typeCode = "85";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(MILK);
      }
      // else{
      //   lcd.show(NUTRITION_WEIGHT_SCREEN);
      //   delay(50);
      //   lcd.showFood(UNKNOWN);

      //   code = "";
      //   lastWeight = 0;
      //   totalWeight = 0;
      //   totalAir = 0;
      //   totalCarbing = 0;
      //   totalEnergy = 0;
      //   totalKarbo = 0;
      //   totalLemak = 0;
      //   totalProtein = 0;
      //   totalSerat = 0;
      // }

      if(lcd.getMenu() == 1){
        lcd.show(MENU_1_SCREEN);
      } else if(lcd.getMenu() == 2){
        lcd.show(MENU_3_SCREEN);
      }

      if(lcd.getBack()){
        lastWeight = 0;
        lcd.show(DASHBOARD_SCREEN);
      }
      delay(50);

    } else if(lcd.getScreen() == MENU_3_SCREEN){
      if(lcd.getChoose() == 1){
        typeCode = "85";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(MILK);
      } else if(lcd.getChoose() == 2){
        typeCode = "11";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(FRUITS);
      } else if(lcd.getChoose() == 3){
        typeCode = "31";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(MEAT);
      } else if(lcd.getChoose() == 4){
        typeCode = "32";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(FISH);
      } 
      // else{
      //   lcd.show(NUTRITION_WEIGHT_SCREEN);
      //   delay(50);
      //   lcd.showFood(UNKNOWN);

      //   code = "";
      //   lastWeight = 0;
      //   totalWeight = 0;
      //   totalAir = 0;
      //   totalCarbing = 0;
      //   totalEnergy = 0;
      //   totalKarbo = 0;
      //   totalLemak = 0;
      //   totalProtein = 0;
      //   totalSerat = 0;
      // }

      if(lcd.getMenu() == 1){
        lcd.show(MENU_2_SCREEN);
      } else if(lcd.getMenu() == 2){
        lcd.show(MENU_4_SCREEN);
      }

      if(lcd.getBack()){
        lastWeight = 0;
        lcd.show(DASHBOARD_SCREEN);
      }
      delay(50);

    } else if(lcd.getScreen() == MENU_4_SCREEN){
      if(lcd.getChoose() == 1){
        typeCode = "32";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(FISH);
      } else if(lcd.getChoose() == 2){
        typeCode = "51";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(EGG);
      } else if(lcd.getChoose() == 3){
        typeCode = "22";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(SPICE);
      } else if(lcd.getChoose() == 4){
        typeCode = "91";
        code = "";
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        delay(50);
        lcd.showFood(DRINK);
      }
      // else{
      //   lcd.show(NUTRITION_WEIGHT_SCREEN);
      //   delay(50);
      //   lcd.showFood(UNKNOWN);

      //   code = "";
      //   lastWeight = 0;
      //   totalWeight = 0;
      //   totalAir = 0;
      //   totalCarbing = 0;
      //   totalEnergy = 0;
      //   totalKarbo = 0;
      //   totalLemak = 0;
      //   totalProtein = 0;
      //   totalSerat = 0;
      // }
      
      if(lcd.getMenu() == 1){
        lcd.show(MENU_3_SCREEN);
      } else if(lcd.getMenu() == 2){
        lcd.show(MENU_1_SCREEN);
      }
      
      if(lcd.getBack()){
        lastWeight = 0;
        lcd.show(DASHBOARD_SCREEN);
      }

      delay(50);
    } else if(lcd.getScreen() == DASHBOARD_SCREEN){
      if(lcd.getMode() == 1){
        lcd.show(MENU_1_SCREEN);

      } else if(lcd.getMode() == 2){
        lcd.show(UNIVERSAL_WEIGHT_SCREEN);
      } else if(lcd.getMode() == 3){
        // lcd.show(SETTING_SCREEN);
        if(deviceConnected){
          lcd.show(SETTING_BTON_SCREEN);
        } else{
          lcd.show(SETTING_BTOFF_SCREEN);
        }
        delay(50);
      }
    } else if(lcd.getScreen() == NUTRITION_WEIGHT_SCREEN){
      if(lcd.getKey()<10){
        if(code.length() < 6){
          code += String(lcd.getKey());
          lcd.updateCode(code);
        }
      }

      if(lcd.getEnter()){
        int rawCode = code.toInt();
        if(rawCode < 10){
          rawCode = typeCode.toInt()*10 + rawCode;
        } else if(rawCode < 100){
          rawCode = typeCode.toInt()*100 + rawCode;
        } else if(rawCode < 1000){
          rawCode = typeCode.toInt()*1000 + rawCode;
        } else if(rawCode < 10000){
          rawCode = typeCode.toInt()*10000 + rawCode;
        } else if(rawCode < 100000){
          rawCode = typeCode.toInt()*100000 + rawCode;
        }

        switch(rawCode){
          case 141 : inputName = AR001[1]; inputAir = AR001[2]; inputEnergy = AR001[3]; inputProtein = AR001[4]; inputLemak = AR001[5]; inputKarbo = AR001[6]; inputSerat = AR001[7]; inputCarbing = AR001[8]; inputType = AR001[9]; inputCode = AR001[10]; break;
          case 142 : inputName = AR002[1]; inputAir = AR002[2]; inputEnergy = AR002[3]; inputProtein = AR002[4]; inputLemak = AR002[5]; inputKarbo = AR002[6]; inputSerat = AR002[7]; inputCarbing = AR002[8]; inputType = AR002[9]; inputCode = AR002[10]; break;
          case 143 : inputName = AR003[1]; inputAir = AR003[2]; inputEnergy = AR003[3]; inputProtein = AR003[4]; inputLemak = AR003[5]; inputKarbo = AR003[6]; inputSerat = AR003[7]; inputCarbing = AR003[8]; inputType = AR003[9]; inputCode = AR003[10]; break;
          case 144 : inputName = AR004[1]; inputAir = AR004[2]; inputEnergy = AR004[3]; inputProtein = AR004[4]; inputLemak = AR004[5]; inputKarbo = AR004[6]; inputSerat = AR004[7]; inputCarbing = AR004[8]; inputType = AR004[9]; inputCode = AR004[10]; break;
          case 145 : inputName = AR005[1]; inputAir = AR005[2]; inputEnergy = AR005[3]; inputProtein = AR005[4]; inputLemak = AR005[5]; inputKarbo = AR005[6]; inputSerat = AR005[7]; inputCarbing = AR005[8]; inputType = AR005[9]; inputCode = AR005[10]; break;
          case 1411 : inputName = AR006[1]; inputAir = AR006[2]; inputEnergy = AR006[3]; inputProtein = AR006[4]; inputLemak = AR006[5]; inputKarbo = AR006[6]; inputSerat = AR006[7]; inputCarbing = AR006[8]; inputType = AR006[9]; inputCode = AR006[10]; break;
          case 1412 : inputName = AR007[1]; inputAir = AR007[2]; inputEnergy = AR007[3]; inputProtein = AR007[4]; inputLemak = AR007[5]; inputKarbo = AR007[6]; inputSerat = AR007[7]; inputCarbing = AR007[8]; inputType = AR007[9]; inputCode = AR007[10]; break;
          case 1413 : inputName = AR008[1]; inputAir = AR008[2]; inputEnergy = AR008[3]; inputProtein = AR008[4]; inputLemak = AR008[5]; inputKarbo = AR008[6]; inputSerat = AR008[7]; inputCarbing = AR008[8]; inputType = AR008[9]; inputCode = AR008[10]; break;
          case 1414 : inputName = AR009[1]; inputAir = AR009[2]; inputEnergy = AR009[3]; inputProtein = AR009[4]; inputLemak = AR009[5]; inputKarbo = AR009[6]; inputSerat = AR009[7]; inputCarbing = AR009[8]; inputType = AR009[9]; inputCode = AR009[10]; break;
          case 1415 : inputName = AR010[1]; inputAir = AR010[2]; inputEnergy = AR010[3]; inputProtein = AR010[4]; inputLemak = AR010[5]; inputKarbo = AR010[6]; inputSerat = AR010[7]; inputCarbing = AR010[8]; inputType = AR010[9]; inputCode = AR010[10]; break;
          case 1421 : inputName = AR011[1]; inputAir = AR011[2]; inputEnergy = AR011[3]; inputProtein = AR011[4]; inputLemak = AR011[5]; inputKarbo = AR011[6]; inputSerat = AR011[7]; inputCarbing = AR011[8]; inputType = AR011[9]; inputCode = AR011[10]; break;
          case 1422 : inputName = AR012[1]; inputAir = AR012[2]; inputEnergy = AR012[3]; inputProtein = AR012[4]; inputLemak = AR012[5]; inputKarbo = AR012[6]; inputSerat = AR012[7]; inputCarbing = AR012[8]; inputType = AR012[9]; inputCode = AR012[10]; break;
          case 1423 : inputName = AR013[1]; inputAir = AR013[2]; inputEnergy = AR013[3]; inputProtein = AR013[4]; inputLemak = AR013[5]; inputKarbo = AR013[6]; inputSerat = AR013[7]; inputCarbing = AR013[8]; inputType = AR013[9]; inputCode = AR013[10]; break;
          case 1424 : inputName = AR014[1]; inputAir = AR014[2]; inputEnergy = AR014[3]; inputProtein = AR014[4]; inputLemak = AR014[5]; inputKarbo = AR014[6]; inputSerat = AR014[7]; inputCarbing = AR014[8]; inputType = AR014[9]; inputCode = AR014[10]; break;
          case 1425 : inputName = AR015[1]; inputAir = AR015[2]; inputEnergy = AR015[3]; inputProtein = AR015[4]; inputLemak = AR015[5]; inputKarbo = AR015[6]; inputSerat = AR015[7]; inputCarbing = AR015[8]; inputType = AR015[9]; inputCode = AR015[10]; break;
          case 1431 : inputName = AR016[1]; inputAir = AR016[2]; inputEnergy = AR016[3]; inputProtein = AR016[4]; inputLemak = AR016[5]; inputKarbo = AR016[6]; inputSerat = AR016[7]; inputCarbing = AR016[8]; inputType = AR016[9]; inputCode = AR016[10]; break;
          case 1432 : inputName = AR017[1]; inputAir = AR017[2]; inputEnergy = AR017[3]; inputProtein = AR017[4]; inputLemak = AR017[5]; inputKarbo = AR017[6]; inputSerat = AR017[7]; inputCarbing = AR017[8]; inputType = AR017[9]; inputCode = AR017[10]; break;
          case 1433 : inputName = AR018[1]; inputAir = AR018[2]; inputEnergy = AR018[3]; inputProtein = AR018[4]; inputLemak = AR018[5]; inputKarbo = AR018[6]; inputSerat = AR018[7]; inputCarbing = AR018[8]; inputType = AR018[9]; inputCode = AR018[10]; break;
          case 1434 : inputName = AR019[1]; inputAir = AR019[2]; inputEnergy = AR019[3]; inputProtein = AR019[4]; inputLemak = AR019[5]; inputKarbo = AR019[6]; inputSerat = AR019[7]; inputCarbing = AR019[8]; inputType = AR019[9]; inputCode = AR019[10]; break;
          case 1435 : inputName = AR020[1]; inputAir = AR020[2]; inputEnergy = AR020[3]; inputProtein = AR020[4]; inputLemak = AR020[5]; inputKarbo = AR020[6]; inputSerat = AR020[7]; inputCarbing = AR020[8]; inputType = AR020[9]; inputCode = AR020[10]; break;
          case 1441 : inputName = AR021[1]; inputAir = AR021[2]; inputEnergy = AR021[3]; inputProtein = AR021[4]; inputLemak = AR021[5]; inputKarbo = AR021[6]; inputSerat = AR021[7]; inputCarbing = AR021[8]; inputType = AR021[9]; inputCode = AR021[10]; break;
          case 1442 : inputName = AP001[1]; inputAir = AP001[2]; inputEnergy = AP001[3]; inputProtein = AP001[4]; inputLemak = AP001[5]; inputKarbo = AP001[6]; inputSerat = AP001[7]; inputCarbing = AP001[8]; inputType = AP001[9]; inputCode = AP001[10]; break;
          case 1443 : inputName = AP002[1]; inputAir = AP002[2]; inputEnergy = AP002[3]; inputProtein = AP002[4]; inputLemak = AP002[5]; inputKarbo = AP002[6]; inputSerat = AP002[7]; inputCarbing = AP002[8]; inputType = AP002[9]; inputCode = AP002[10]; break;
          case 1444 : inputName = AP003[1]; inputAir = AP003[2]; inputEnergy = AP003[3]; inputProtein = AP003[4]; inputLemak = AP003[5]; inputKarbo = AP003[6]; inputSerat = AP003[7]; inputCarbing = AP003[8]; inputType = AP003[9]; inputCode = AP003[10]; break;
          case 1445 : inputName = AP004[1]; inputAir = AP004[2]; inputEnergy = AP004[3]; inputProtein = AP004[4]; inputLemak = AP004[5]; inputKarbo = AP004[6]; inputSerat = AP004[7]; inputCarbing = AP004[8]; inputType = AP004[9]; inputCode = AP004[10]; break;
          case 1451 : inputName = AP005[1]; inputAir = AP005[2]; inputEnergy = AP005[3]; inputProtein = AP005[4]; inputLemak = AP005[5]; inputKarbo = AP005[6]; inputSerat = AP005[7]; inputCarbing = AP005[8]; inputType = AP005[9]; inputCode = AP005[10]; break;
          case 1452 : inputName = AP006[1]; inputAir = AP006[2]; inputEnergy = AP006[3]; inputProtein = AP006[4]; inputLemak = AP006[5]; inputKarbo = AP006[6]; inputSerat = AP006[7]; inputCarbing = AP006[8]; inputType = AP006[9]; inputCode = AP006[10]; break;
          case 1453 : inputName = AP007[1]; inputAir = AP007[2]; inputEnergy = AP007[3]; inputProtein = AP007[4]; inputLemak = AP007[5]; inputKarbo = AP007[6]; inputSerat = AP007[7]; inputCarbing = AP007[8]; inputType = AP007[9]; inputCode = AP007[10]; break;
          case 1454 : inputName = AP008[1]; inputAir = AP008[2]; inputEnergy = AP008[3]; inputProtein = AP008[4]; inputLemak = AP008[5]; inputKarbo = AP008[6]; inputSerat = AP008[7]; inputCarbing = AP008[8]; inputType = AP008[9]; inputCode = AP008[10]; break;
          case 1455 : inputName = AP009[1]; inputAir = AP009[2]; inputEnergy = AP009[3]; inputProtein = AP009[4]; inputLemak = AP009[5]; inputKarbo = AP009[6]; inputSerat = AP009[7]; inputCarbing = AP009[8]; inputType = AP009[9]; inputCode = AP009[10]; break;
          case 14111 : inputName = AP010[1]; inputAir = AP010[2]; inputEnergy = AP010[3]; inputProtein = AP010[4]; inputLemak = AP010[5]; inputKarbo = AP010[6]; inputSerat = AP010[7]; inputCarbing = AP010[8]; inputType = AP010[9]; inputCode = AP010[10]; break;
          case 14112 : inputName = AP011[1]; inputAir = AP011[2]; inputEnergy = AP011[3]; inputProtein = AP011[4]; inputLemak = AP011[5]; inputKarbo = AP011[6]; inputSerat = AP011[7]; inputCarbing = AP011[8]; inputType = AP011[9]; inputCode = AP011[10]; break;
          case 14113 : inputName = AP012[1]; inputAir = AP012[2]; inputEnergy = AP012[3]; inputProtein = AP012[4]; inputLemak = AP012[5]; inputKarbo = AP012[6]; inputSerat = AP012[7]; inputCarbing = AP012[8]; inputType = AP012[9]; inputCode = AP012[10]; break;
          case 14114 : inputName = AP013[1]; inputAir = AP013[2]; inputEnergy = AP013[3]; inputProtein = AP013[4]; inputLemak = AP013[5]; inputKarbo = AP013[6]; inputSerat = AP013[7]; inputCarbing = AP013[8]; inputType = AP013[9]; inputCode = AP013[10]; break;
          case 14115 : inputName = AP014[1]; inputAir = AP014[2]; inputEnergy = AP014[3]; inputProtein = AP014[4]; inputLemak = AP014[5]; inputKarbo = AP014[6]; inputSerat = AP014[7]; inputCarbing = AP014[8]; inputType = AP014[9]; inputCode = AP014[10]; break;
          case 14121 : inputName = AP015[1]; inputAir = AP015[2]; inputEnergy = AP015[3]; inputProtein = AP015[4]; inputLemak = AP015[5]; inputKarbo = AP015[6]; inputSerat = AP015[7]; inputCarbing = AP015[8]; inputType = AP015[9]; inputCode = AP015[10]; break;
          case 14122 : inputName = AP016[1]; inputAir = AP016[2]; inputEnergy = AP016[3]; inputProtein = AP016[4]; inputLemak = AP016[5]; inputKarbo = AP016[6]; inputSerat = AP016[7]; inputCarbing = AP016[8]; inputType = AP016[9]; inputCode = AP016[10]; break;
          case 14123 : inputName = AP017[1]; inputAir = AP017[2]; inputEnergy = AP017[3]; inputProtein = AP017[4]; inputLemak = AP017[5]; inputKarbo = AP017[6]; inputSerat = AP017[7]; inputCarbing = AP017[8]; inputType = AP017[9]; inputCode = AP017[10]; break;
          case 14124 : inputName = AP018[1]; inputAir = AP018[2]; inputEnergy = AP018[3]; inputProtein = AP018[4]; inputLemak = AP018[5]; inputKarbo = AP018[6]; inputSerat = AP018[7]; inputCarbing = AP018[8]; inputType = AP018[9]; inputCode = AP018[10]; break;
          case 14125 : inputName = AP019[1]; inputAir = AP019[2]; inputEnergy = AP019[3]; inputProtein = AP019[4]; inputLemak = AP019[5]; inputKarbo = AP019[6]; inputSerat = AP019[7]; inputCarbing = AP019[8]; inputType = AP019[9]; inputCode = AP019[10]; break;
          case 14131 : inputName = AP020[1]; inputAir = AP020[2]; inputEnergy = AP020[3]; inputProtein = AP020[4]; inputLemak = AP020[5]; inputKarbo = AP020[6]; inputSerat = AP020[7]; inputCarbing = AP020[8]; inputType = AP020[9]; inputCode = AP020[10]; break;
          case 14132 : inputName = AP021[1]; inputAir = AP021[2]; inputEnergy = AP021[3]; inputProtein = AP021[4]; inputLemak = AP021[5]; inputKarbo = AP021[6]; inputSerat = AP021[7]; inputCarbing = AP021[8]; inputType = AP021[9]; inputCode = AP021[10]; break;
          case 14133 : inputName = AP022[1]; inputAir = AP022[2]; inputEnergy = AP022[3]; inputProtein = AP022[4]; inputLemak = AP022[5]; inputKarbo = AP022[6]; inputSerat = AP022[7]; inputCarbing = AP022[8]; inputType = AP022[9]; inputCode = AP022[10]; break;
          case 14134 : inputName = AP023[1]; inputAir = AP023[2]; inputEnergy = AP023[3]; inputProtein = AP023[4]; inputLemak = AP023[5]; inputKarbo = AP023[6]; inputSerat = AP023[7]; inputCarbing = AP023[8]; inputType = AP023[9]; inputCode = AP023[10]; break;
          case 14135 : inputName = AP024[1]; inputAir = AP024[2]; inputEnergy = AP024[3]; inputProtein = AP024[4]; inputLemak = AP024[5]; inputKarbo = AP024[6]; inputSerat = AP024[7]; inputCarbing = AP024[8]; inputType = AP024[9]; inputCode = AP024[10]; break;
          case 14141 : inputName = AP025[1]; inputAir = AP025[2]; inputEnergy = AP025[3]; inputProtein = AP025[4]; inputLemak = AP025[5]; inputKarbo = AP025[6]; inputSerat = AP025[7]; inputCarbing = AP025[8]; inputType = AP025[9]; inputCode = AP025[10]; break;
          case 14142 : inputName = AP026[1]; inputAir = AP026[2]; inputEnergy = AP026[3]; inputProtein = AP026[4]; inputLemak = AP026[5]; inputKarbo = AP026[6]; inputSerat = AP026[7]; inputCarbing = AP026[8]; inputType = AP026[9]; inputCode = AP026[10]; break;
          case 14143 : inputName = AP027[1]; inputAir = AP027[2]; inputEnergy = AP027[3]; inputProtein = AP027[4]; inputLemak = AP027[5]; inputKarbo = AP027[6]; inputSerat = AP027[7]; inputCarbing = AP027[8]; inputType = AP027[9]; inputCode = AP027[10]; break;
          case 14144 : inputName = AP028[1]; inputAir = AP028[2]; inputEnergy = AP028[3]; inputProtein = AP028[4]; inputLemak = AP028[5]; inputKarbo = AP028[6]; inputSerat = AP028[7]; inputCarbing = AP028[8]; inputType = AP028[9]; inputCode = AP028[10]; break;
          case 14145 : inputName = AP029[1]; inputAir = AP029[2]; inputEnergy = AP029[3]; inputProtein = AP029[4]; inputLemak = AP029[5]; inputKarbo = AP029[6]; inputSerat = AP029[7]; inputCarbing = AP029[8]; inputType = AP029[9]; inputCode = AP029[10]; break;
          case 14151 : inputName = AP030[1]; inputAir = AP030[2]; inputEnergy = AP030[3]; inputProtein = AP030[4]; inputLemak = AP030[5]; inputKarbo = AP030[6]; inputSerat = AP030[7]; inputCarbing = AP030[8]; inputType = AP030[9]; inputCode = AP030[10]; break;
          case 14152 : inputName = AP031[1]; inputAir = AP031[2]; inputEnergy = AP031[3]; inputProtein = AP031[4]; inputLemak = AP031[5]; inputKarbo = AP031[6]; inputSerat = AP031[7]; inputCarbing = AP031[8]; inputType = AP031[9]; inputCode = AP031[10]; break;
          case 14153 : inputName = AP032[1]; inputAir = AP032[2]; inputEnergy = AP032[3]; inputProtein = AP032[4]; inputLemak = AP032[5]; inputKarbo = AP032[6]; inputSerat = AP032[7]; inputCarbing = AP032[8]; inputType = AP032[9]; inputCode = AP032[10]; break;
          case 14154 : inputName = AP033[1]; inputAir = AP033[2]; inputEnergy = AP033[3]; inputProtein = AP033[4]; inputLemak = AP033[5]; inputKarbo = AP033[6]; inputSerat = AP033[7]; inputCarbing = AP033[8]; inputType = AP033[9]; inputCode = AP033[10]; break;
          case 14155 : inputName = AP034[1]; inputAir = AP034[2]; inputEnergy = AP034[3]; inputProtein = AP034[4]; inputLemak = AP034[5]; inputKarbo = AP034[6]; inputSerat = AP034[7]; inputCarbing = AP034[8]; inputType = AP034[9]; inputCode = AP034[10]; break;
          case 14211 : inputName = AP035[1]; inputAir = AP035[2]; inputEnergy = AP035[3]; inputProtein = AP035[4]; inputLemak = AP035[5]; inputKarbo = AP035[6]; inputSerat = AP035[7]; inputCarbing = AP035[8]; inputType = AP035[9]; inputCode = AP035[10]; break;
          case 14212 : inputName = AP036[1]; inputAir = AP036[2]; inputEnergy = AP036[3]; inputProtein = AP036[4]; inputLemak = AP036[5]; inputKarbo = AP036[6]; inputSerat = AP036[7]; inputCarbing = AP036[8]; inputType = AP036[9]; inputCode = AP036[10]; break;
          case 14213 : inputName = AP037[1]; inputAir = AP037[2]; inputEnergy = AP037[3]; inputProtein = AP037[4]; inputLemak = AP037[5]; inputKarbo = AP037[6]; inputSerat = AP037[7]; inputCarbing = AP037[8]; inputType = AP037[9]; inputCode = AP037[10]; break;
          case 14214 : inputName = AP038[1]; inputAir = AP038[2]; inputEnergy = AP038[3]; inputProtein = AP038[4]; inputLemak = AP038[5]; inputKarbo = AP038[6]; inputSerat = AP038[7]; inputCarbing = AP038[8]; inputType = AP038[9]; inputCode = AP038[10]; break;
          case 14215 : inputName = AP039[1]; inputAir = AP039[2]; inputEnergy = AP039[3]; inputProtein = AP039[4]; inputLemak = AP039[5]; inputKarbo = AP039[6]; inputSerat = AP039[7]; inputCarbing = AP039[8]; inputType = AP039[9]; inputCode = AP039[10]; break;
          case 14221 : inputName = AP040[1]; inputAir = AP040[2]; inputEnergy = AP040[3]; inputProtein = AP040[4]; inputLemak = AP040[5]; inputKarbo = AP040[6]; inputSerat = AP040[7]; inputCarbing = AP040[8]; inputType = AP040[9]; inputCode = AP040[10]; break;
          case 14222 : inputName = AP041[1]; inputAir = AP041[2]; inputEnergy = AP041[3]; inputProtein = AP041[4]; inputLemak = AP041[5]; inputKarbo = AP041[6]; inputSerat = AP041[7]; inputCarbing = AP041[8]; inputType = AP041[9]; inputCode = AP041[10]; break;
          case 14223 : inputName = AP042[1]; inputAir = AP042[2]; inputEnergy = AP042[3]; inputProtein = AP042[4]; inputLemak = AP042[5]; inputKarbo = AP042[6]; inputSerat = AP042[7]; inputCarbing = AP042[8]; inputType = AP042[9]; inputCode = AP042[10]; break;
          case 14224 : inputName = AP043[1]; inputAir = AP043[2]; inputEnergy = AP043[3]; inputProtein = AP043[4]; inputLemak = AP043[5]; inputKarbo = AP043[6]; inputSerat = AP043[7]; inputCarbing = AP043[8]; inputType = AP043[9]; inputCode = AP043[10]; break;
          case 14225 : inputName = AP044[1]; inputAir = AP044[2]; inputEnergy = AP044[3]; inputProtein = AP044[4]; inputLemak = AP044[5]; inputKarbo = AP044[6]; inputSerat = AP044[7]; inputCarbing = AP044[8]; inputType = AP044[9]; inputCode = AP044[10]; break;
          case 14231 : inputName = AP045[1]; inputAir = AP045[2]; inputEnergy = AP045[3]; inputProtein = AP045[4]; inputLemak = AP045[5]; inputKarbo = AP045[6]; inputSerat = AP045[7]; inputCarbing = AP045[8]; inputType = AP045[9]; inputCode = AP045[10]; break;
          case 14232 : inputName = AP046[1]; inputAir = AP046[2]; inputEnergy = AP046[3]; inputProtein = AP046[4]; inputLemak = AP046[5]; inputKarbo = AP046[6]; inputSerat = AP046[7]; inputCarbing = AP046[8]; inputType = AP046[9]; inputCode = AP046[10]; break;
          case 14233 : inputName = AP047[1]; inputAir = AP047[2]; inputEnergy = AP047[3]; inputProtein = AP047[4]; inputLemak = AP047[5]; inputKarbo = AP047[6]; inputSerat = AP047[7]; inputCarbing = AP047[8]; inputType = AP047[9]; inputCode = AP047[10]; break;
          case 14234 : inputName = AP048[1]; inputAir = AP048[2]; inputEnergy = AP048[3]; inputProtein = AP048[4]; inputLemak = AP048[5]; inputKarbo = AP048[6]; inputSerat = AP048[7]; inputCarbing = AP048[8]; inputType = AP048[9]; inputCode = AP048[10]; break;
          case 14235 : inputName = AP049[1]; inputAir = AP049[2]; inputEnergy = AP049[3]; inputProtein = AP049[4]; inputLemak = AP049[5]; inputKarbo = AP049[6]; inputSerat = AP049[7]; inputCarbing = AP049[8]; inputType = AP049[9]; inputCode = AP049[10]; break;
          case 14241 : inputName = AP050[1]; inputAir = AP050[2]; inputEnergy = AP050[3]; inputProtein = AP050[4]; inputLemak = AP050[5]; inputKarbo = AP050[6]; inputSerat = AP050[7]; inputCarbing = AP050[8]; inputType = AP050[9]; inputCode = AP050[10]; break;
          case 14242 : inputName = AP051[1]; inputAir = AP051[2]; inputEnergy = AP051[3]; inputProtein = AP051[4]; inputLemak = AP051[5]; inputKarbo = AP051[6]; inputSerat = AP051[7]; inputCarbing = AP051[8]; inputType = AP051[9]; inputCode = AP051[10]; break;
          case 14243 : inputName = AP052[1]; inputAir = AP052[2]; inputEnergy = AP052[3]; inputProtein = AP052[4]; inputLemak = AP052[5]; inputKarbo = AP052[6]; inputSerat = AP052[7]; inputCarbing = AP052[8]; inputType = AP052[9]; inputCode = AP052[10]; break;
          case 14244 : inputName = AP053[1]; inputAir = AP053[2]; inputEnergy = AP053[3]; inputProtein = AP053[4]; inputLemak = AP053[5]; inputKarbo = AP053[6]; inputSerat = AP053[7]; inputCarbing = AP053[8]; inputType = AP053[9]; inputCode = AP053[10]; break;
          case 14245 : inputName = AP054[1]; inputAir = AP054[2]; inputEnergy = AP054[3]; inputProtein = AP054[4]; inputLemak = AP054[5]; inputKarbo = AP054[6]; inputSerat = AP054[7]; inputCarbing = AP054[8]; inputType = AP054[9]; inputCode = AP054[10]; break;
          case 14251 : inputName = AP055[1]; inputAir = AP055[2]; inputEnergy = AP055[3]; inputProtein = AP055[4]; inputLemak = AP055[5]; inputKarbo = AP055[6]; inputSerat = AP055[7]; inputCarbing = AP055[8]; inputType = AP055[9]; inputCode = AP055[10]; break;
          case 14252 : inputName = AP056[1]; inputAir = AP056[2]; inputEnergy = AP056[3]; inputProtein = AP056[4]; inputLemak = AP056[5]; inputKarbo = AP056[6]; inputSerat = AP056[7]; inputCarbing = AP056[8]; inputType = AP056[9]; inputCode = AP056[10]; break;
          case 14253 : inputName = AP057[1]; inputAir = AP057[2]; inputEnergy = AP057[3]; inputProtein = AP057[4]; inputLemak = AP057[5]; inputKarbo = AP057[6]; inputSerat = AP057[7]; inputCarbing = AP057[8]; inputType = AP057[9]; inputCode = AP057[10]; break;
          case 14254 : inputName = AP058[1]; inputAir = AP058[2]; inputEnergy = AP058[3]; inputProtein = AP058[4]; inputLemak = AP058[5]; inputKarbo = AP058[6]; inputSerat = AP058[7]; inputCarbing = AP058[8]; inputType = AP058[9]; inputCode = AP058[10]; break;
          case 14255 : inputName = AP059[1]; inputAir = AP059[2]; inputEnergy = AP059[3]; inputProtein = AP059[4]; inputLemak = AP059[5]; inputKarbo = AP059[6]; inputSerat = AP059[7]; inputCarbing = AP059[8]; inputType = AP059[9]; inputCode = AP059[10]; break;
          case 14311 : inputName = AP060[1]; inputAir = AP060[2]; inputEnergy = AP060[3]; inputProtein = AP060[4]; inputLemak = AP060[5]; inputKarbo = AP060[6]; inputSerat = AP060[7]; inputCarbing = AP060[8]; inputType = AP060[9]; inputCode = AP060[10]; break;
          case 14312 : inputName = AP061[1]; inputAir = AP061[2]; inputEnergy = AP061[3]; inputProtein = AP061[4]; inputLemak = AP061[5]; inputKarbo = AP061[6]; inputSerat = AP061[7]; inputCarbing = AP061[8]; inputType = AP061[9]; inputCode = AP061[10]; break;
          case 14313 : inputName = AP062[1]; inputAir = AP062[2]; inputEnergy = AP062[3]; inputProtein = AP062[4]; inputLemak = AP062[5]; inputKarbo = AP062[6]; inputSerat = AP062[7]; inputCarbing = AP062[8]; inputType = AP062[9]; inputCode = AP062[10]; break;
          case 14314 : inputName = AP063[1]; inputAir = AP063[2]; inputEnergy = AP063[3]; inputProtein = AP063[4]; inputLemak = AP063[5]; inputKarbo = AP063[6]; inputSerat = AP063[7]; inputCarbing = AP063[8]; inputType = AP063[9]; inputCode = AP063[10]; break;
          case 14315 : inputName = AP064[1]; inputAir = AP064[2]; inputEnergy = AP064[3]; inputProtein = AP064[4]; inputLemak = AP064[5]; inputKarbo = AP064[6]; inputSerat = AP064[7]; inputCarbing = AP064[8]; inputType = AP064[9]; inputCode = AP064[10]; break;
          case 14321 : inputName = AP065[1]; inputAir = AP065[2]; inputEnergy = AP065[3]; inputProtein = AP065[4]; inputLemak = AP065[5]; inputKarbo = AP065[6]; inputSerat = AP065[7]; inputCarbing = AP065[8]; inputType = AP065[9]; inputCode = AP065[10]; break;
          case 14322 : inputName = AP066[1]; inputAir = AP066[2]; inputEnergy = AP066[3]; inputProtein = AP066[4]; inputLemak = AP066[5]; inputKarbo = AP066[6]; inputSerat = AP066[7]; inputCarbing = AP066[8]; inputType = AP066[9]; inputCode = AP066[10]; break;
          case 14323 : inputName = AP067[1]; inputAir = AP067[2]; inputEnergy = AP067[3]; inputProtein = AP067[4]; inputLemak = AP067[5]; inputKarbo = AP067[6]; inputSerat = AP067[7]; inputCarbing = AP067[8]; inputType = AP067[9]; inputCode = AP067[10]; break;
          case 14324 : inputName = AP068[1]; inputAir = AP068[2]; inputEnergy = AP068[3]; inputProtein = AP068[4]; inputLemak = AP068[5]; inputKarbo = AP068[6]; inputSerat = AP068[7]; inputCarbing = AP068[8]; inputType = AP068[9]; inputCode = AP068[10]; break;
          case 14325 : inputName = AP069[1]; inputAir = AP069[2]; inputEnergy = AP069[3]; inputProtein = AP069[4]; inputLemak = AP069[5]; inputKarbo = AP069[6]; inputSerat = AP069[7]; inputCarbing = AP069[8]; inputType = AP069[9]; inputCode = AP069[10]; break;
          case 14331 : inputName = AP070[1]; inputAir = AP070[2]; inputEnergy = AP070[3]; inputProtein = AP070[4]; inputLemak = AP070[5]; inputKarbo = AP070[6]; inputSerat = AP070[7]; inputCarbing = AP070[8]; inputType = AP070[9]; inputCode = AP070[10]; break;
          case 14332 : inputName = AP071[1]; inputAir = AP071[2]; inputEnergy = AP071[3]; inputProtein = AP071[4]; inputLemak = AP071[5]; inputKarbo = AP071[6]; inputSerat = AP071[7]; inputCarbing = AP071[8]; inputType = AP071[9]; inputCode = AP071[10]; break;
          case 14333 : inputName = AP072[1]; inputAir = AP072[2]; inputEnergy = AP072[3]; inputProtein = AP072[4]; inputLemak = AP072[5]; inputKarbo = AP072[6]; inputSerat = AP072[7]; inputCarbing = AP072[8]; inputType = AP072[9]; inputCode = AP072[10]; break;
          case 14334 : inputName = AP073[1]; inputAir = AP073[2]; inputEnergy = AP073[3]; inputProtein = AP073[4]; inputLemak = AP073[5]; inputKarbo = AP073[6]; inputSerat = AP073[7]; inputCarbing = AP073[8]; inputType = AP073[9]; inputCode = AP073[10]; break;
          case 14335 : inputName = AP074[1]; inputAir = AP074[2]; inputEnergy = AP074[3]; inputProtein = AP074[4]; inputLemak = AP074[5]; inputKarbo = AP074[6]; inputSerat = AP074[7]; inputCarbing = AP074[8]; inputType = AP074[9]; inputCode = AP074[10]; break;
          case 14341 : inputName = AP075[1]; inputAir = AP075[2]; inputEnergy = AP075[3]; inputProtein = AP075[4]; inputLemak = AP075[5]; inputKarbo = AP075[6]; inputSerat = AP075[7]; inputCarbing = AP075[8]; inputType = AP075[9]; inputCode = AP075[10]; break;
          case 14342 : inputName = AP076[1]; inputAir = AP076[2]; inputEnergy = AP076[3]; inputProtein = AP076[4]; inputLemak = AP076[5]; inputKarbo = AP076[6]; inputSerat = AP076[7]; inputCarbing = AP076[8]; inputType = AP076[9]; inputCode = AP076[10]; break;
          case 14343 : inputName = AP077[1]; inputAir = AP077[2]; inputEnergy = AP077[3]; inputProtein = AP077[4]; inputLemak = AP077[5]; inputKarbo = AP077[6]; inputSerat = AP077[7]; inputCarbing = AP077[8]; inputType = AP077[9]; inputCode = AP077[10]; break;
          case 14344 : inputName = AP078[1]; inputAir = AP078[2]; inputEnergy = AP078[3]; inputProtein = AP078[4]; inputLemak = AP078[5]; inputKarbo = AP078[6]; inputSerat = AP078[7]; inputCarbing = AP078[8]; inputType = AP078[9]; inputCode = AP078[10]; break;
          case 14345 : inputName = AP079[1]; inputAir = AP079[2]; inputEnergy = AP079[3]; inputProtein = AP079[4]; inputLemak = AP079[5]; inputKarbo = AP079[6]; inputSerat = AP079[7]; inputCarbing = AP079[8]; inputType = AP079[9]; inputCode = AP079[10]; break;
          case 14351 : inputName = AP080[1]; inputAir = AP080[2]; inputEnergy = AP080[3]; inputProtein = AP080[4]; inputLemak = AP080[5]; inputKarbo = AP080[6]; inputSerat = AP080[7]; inputCarbing = AP080[8]; inputType = AP080[9]; inputCode = AP080[10]; break;
          case 14352 : inputName = AP081[1]; inputAir = AP081[2]; inputEnergy = AP081[3]; inputProtein = AP081[4]; inputLemak = AP081[5]; inputKarbo = AP081[6]; inputSerat = AP081[7]; inputCarbing = AP081[8]; inputType = AP081[9]; inputCode = AP081[10]; break;
          case 14353 : inputName = AP082[1]; inputAir = AP082[2]; inputEnergy = AP082[3]; inputProtein = AP082[4]; inputLemak = AP082[5]; inputKarbo = AP082[6]; inputSerat = AP082[7]; inputCarbing = AP082[8]; inputType = AP082[9]; inputCode = AP082[10]; break;
          case 14354 : inputName = AP083[1]; inputAir = AP083[2]; inputEnergy = AP083[3]; inputProtein = AP083[4]; inputLemak = AP083[5]; inputKarbo = AP083[6]; inputSerat = AP083[7]; inputCarbing = AP083[8]; inputType = AP083[9]; inputCode = AP083[10]; break;
          case 14355 : inputName = AP084[1]; inputAir = AP084[2]; inputEnergy = AP084[3]; inputProtein = AP084[4]; inputLemak = AP084[5]; inputKarbo = AP084[6]; inputSerat = AP084[7]; inputCarbing = AP084[8]; inputType = AP084[9]; inputCode = AP084[10]; break;
          case 14411 : inputName = AP085[1]; inputAir = AP085[2]; inputEnergy = AP085[3]; inputProtein = AP085[4]; inputLemak = AP085[5]; inputKarbo = AP085[6]; inputSerat = AP085[7]; inputCarbing = AP085[8]; inputType = AP085[9]; inputCode = AP085[10]; break;
          case 14412 : inputName = AP086[1]; inputAir = AP086[2]; inputEnergy = AP086[3]; inputProtein = AP086[4]; inputLemak = AP086[5]; inputKarbo = AP086[6]; inputSerat = AP086[7]; inputCarbing = AP086[8]; inputType = AP086[9]; inputCode = AP086[10]; break;
          case 14413 : inputName = AP087[1]; inputAir = AP087[2]; inputEnergy = AP087[3]; inputProtein = AP087[4]; inputLemak = AP087[5]; inputKarbo = AP087[6]; inputSerat = AP087[7]; inputCarbing = AP087[8]; inputType = AP087[9]; inputCode = AP087[10]; break;
          case 14414 : inputName = AP088[1]; inputAir = AP088[2]; inputEnergy = AP088[3]; inputProtein = AP088[4]; inputLemak = AP088[5]; inputKarbo = AP088[6]; inputSerat = AP088[7]; inputCarbing = AP088[8]; inputType = AP088[9]; inputCode = AP088[10]; break;
          case 14415 : inputName = AP089[1]; inputAir = AP089[2]; inputEnergy = AP089[3]; inputProtein = AP089[4]; inputLemak = AP089[5]; inputKarbo = AP089[6]; inputSerat = AP089[7]; inputCarbing = AP089[8]; inputType = AP089[9]; inputCode = AP089[10]; break;
          case 14421 : inputName = AP090[1]; inputAir = AP090[2]; inputEnergy = AP090[3]; inputProtein = AP090[4]; inputLemak = AP090[5]; inputKarbo = AP090[6]; inputSerat = AP090[7]; inputCarbing = AP090[8]; inputType = AP090[9]; inputCode = AP090[10]; break;
          case 14422 : inputName = AP091[1]; inputAir = AP091[2]; inputEnergy = AP091[3]; inputProtein = AP091[4]; inputLemak = AP091[5]; inputKarbo = AP091[6]; inputSerat = AP091[7]; inputCarbing = AP091[8]; inputType = AP091[9]; inputCode = AP091[10]; break;
          case 14423 : inputName = AP092[1]; inputAir = AP092[2]; inputEnergy = AP092[3]; inputProtein = AP092[4]; inputLemak = AP092[5]; inputKarbo = AP092[6]; inputSerat = AP092[7]; inputCarbing = AP092[8]; inputType = AP092[9]; inputCode = AP092[10]; break;
          case 14424 : inputName = AP093[1]; inputAir = AP093[2]; inputEnergy = AP093[3]; inputProtein = AP093[4]; inputLemak = AP093[5]; inputKarbo = AP093[6]; inputSerat = AP093[7]; inputCarbing = AP093[8]; inputType = AP093[9]; inputCode = AP093[10]; break;
          case 14425 : inputName = AP094[1]; inputAir = AP094[2]; inputEnergy = AP094[3]; inputProtein = AP094[4]; inputLemak = AP094[5]; inputKarbo = AP094[6]; inputSerat = AP094[7]; inputCarbing = AP094[8]; inputType = AP094[9]; inputCode = AP094[10]; break;
          case 14431 : inputName = AP095[1]; inputAir = AP095[2]; inputEnergy = AP095[3]; inputProtein = AP095[4]; inputLemak = AP095[5]; inputKarbo = AP095[6]; inputSerat = AP095[7]; inputCarbing = AP095[8]; inputType = AP095[9]; inputCode = AP095[10]; break;
          case 14432 : inputName = AP096[1]; inputAir = AP096[2]; inputEnergy = AP096[3]; inputProtein = AP096[4]; inputLemak = AP096[5]; inputKarbo = AP096[6]; inputSerat = AP096[7]; inputCarbing = AP096[8]; inputType = AP096[9]; inputCode = AP096[10]; break;
          case 14433 : inputName = AP097[1]; inputAir = AP097[2]; inputEnergy = AP097[3]; inputProtein = AP097[4]; inputLemak = AP097[5]; inputKarbo = AP097[6]; inputSerat = AP097[7]; inputCarbing = AP097[8]; inputType = AP097[9]; inputCode = AP097[10]; break;
          case 14434 : inputName = AP098[1]; inputAir = AP098[2]; inputEnergy = AP098[3]; inputProtein = AP098[4]; inputLemak = AP098[5]; inputKarbo = AP098[6]; inputSerat = AP098[7]; inputCarbing = AP098[8]; inputType = AP098[9]; inputCode = AP098[10]; break;
          case 14435 : inputName = AP099[1]; inputAir = AP099[2]; inputEnergy = AP099[3]; inputProtein = AP099[4]; inputLemak = AP099[5]; inputKarbo = AP099[6]; inputSerat = AP099[7]; inputCarbing = AP099[8]; inputType = AP099[9]; inputCode = AP099[10]; break;
          case 14441 : inputName = AP100[1]; inputAir = AP100[2]; inputEnergy = AP100[3]; inputProtein = AP100[4]; inputLemak = AP100[5]; inputKarbo = AP100[6]; inputSerat = AP100[7]; inputCarbing = AP100[8]; inputType = AP100[9]; inputCode = AP100[10]; break;
          case 14442 : inputName = AP101[1]; inputAir = AP101[2]; inputEnergy = AP101[3]; inputProtein = AP101[4]; inputLemak = AP101[5]; inputKarbo = AP101[6]; inputSerat = AP101[7]; inputCarbing = AP101[8]; inputType = AP101[9]; inputCode = AP101[10]; break;
          case 14443 : inputName = AP102[1]; inputAir = AP102[2]; inputEnergy = AP102[3]; inputProtein = AP102[4]; inputLemak = AP102[5]; inputKarbo = AP102[6]; inputSerat = AP102[7]; inputCarbing = AP102[8]; inputType = AP102[9]; inputCode = AP102[10]; break;
          case 14444 : inputName = AP103[1]; inputAir = AP103[2]; inputEnergy = AP103[3]; inputProtein = AP103[4]; inputLemak = AP103[5]; inputKarbo = AP103[6]; inputSerat = AP103[7]; inputCarbing = AP103[8]; inputType = AP103[9]; inputCode = AP103[10]; break;
          case 14445 : inputName = AP104[1]; inputAir = AP104[2]; inputEnergy = AP104[3]; inputProtein = AP104[4]; inputLemak = AP104[5]; inputKarbo = AP104[6]; inputSerat = AP104[7]; inputCarbing = AP104[8]; inputType = AP104[9]; inputCode = AP104[10]; break;
          case 14451 : inputName = AP105[1]; inputAir = AP105[2]; inputEnergy = AP105[3]; inputProtein = AP105[4]; inputLemak = AP105[5]; inputKarbo = AP105[6]; inputSerat = AP105[7]; inputCarbing = AP105[8]; inputType = AP105[9]; inputCode = AP105[10]; break;
          case 14452 : inputName = AP106[1]; inputAir = AP106[2]; inputEnergy = AP106[3]; inputProtein = AP106[4]; inputLemak = AP106[5]; inputKarbo = AP106[6]; inputSerat = AP106[7]; inputCarbing = AP106[8]; inputType = AP106[9]; inputCode = AP106[10]; break;
          case 14453 : inputName = AP107[1]; inputAir = AP107[2]; inputEnergy = AP107[3]; inputProtein = AP107[4]; inputLemak = AP107[5]; inputKarbo = AP107[6]; inputSerat = AP107[7]; inputCarbing = AP107[8]; inputType = AP107[9]; inputCode = AP107[10]; break;
          case 14454 : inputName = AP108[1]; inputAir = AP108[2]; inputEnergy = AP108[3]; inputProtein = AP108[4]; inputLemak = AP108[5]; inputKarbo = AP108[6]; inputSerat = AP108[7]; inputCarbing = AP108[8]; inputType = AP108[9]; inputCode = AP108[10]; break;
          case 14455 : inputName = AP109[1]; inputAir = AP109[2]; inputEnergy = AP109[3]; inputProtein = AP109[4]; inputLemak = AP109[5]; inputKarbo = AP109[6]; inputSerat = AP109[7]; inputCarbing = AP109[8]; inputType = AP109[9]; inputCode = AP109[10]; break;
          case 14511 : inputName = AP110[1]; inputAir = AP110[2]; inputEnergy = AP110[3]; inputProtein = AP110[4]; inputLemak = AP110[5]; inputKarbo = AP110[6]; inputSerat = AP110[7]; inputCarbing = AP110[8]; inputType = AP110[9]; inputCode = AP110[10]; break;
          case 14512 : inputName = AP111[1]; inputAir = AP111[2]; inputEnergy = AP111[3]; inputProtein = AP111[4]; inputLemak = AP111[5]; inputKarbo = AP111[6]; inputSerat = AP111[7]; inputCarbing = AP111[8]; inputType = AP111[9]; inputCode = AP111[10]; break;
          case 14513 : inputName = AP112[1]; inputAir = AP112[2]; inputEnergy = AP112[3]; inputProtein = AP112[4]; inputLemak = AP112[5]; inputKarbo = AP112[6]; inputSerat = AP112[7]; inputCarbing = AP112[8]; inputType = AP112[9]; inputCode = AP112[10]; break;
          case 14514 : inputName = AP113[1]; inputAir = AP113[2]; inputEnergy = AP113[3]; inputProtein = AP113[4]; inputLemak = AP113[5]; inputKarbo = AP113[6]; inputSerat = AP113[7]; inputCarbing = AP113[8]; inputType = AP113[9]; inputCode = AP113[10]; break;
          case 14515 : inputName = AP114[1]; inputAir = AP114[2]; inputEnergy = AP114[3]; inputProtein = AP114[4]; inputLemak = AP114[5]; inputKarbo = AP114[6]; inputSerat = AP114[7]; inputCarbing = AP114[8]; inputType = AP114[9]; inputCode = AP114[10]; break;
          case 161 : inputName = BR001[1]; inputAir = BR001[2]; inputEnergy = BR001[3]; inputProtein = BR001[4]; inputLemak = BR001[5]; inputKarbo = BR001[6]; inputSerat = BR001[7]; inputCarbing = BR001[8]; inputType = BR001[9]; inputCode = BR001[10]; break;
          case 162 : inputName = BR002[1]; inputAir = BR002[2]; inputEnergy = BR002[3]; inputProtein = BR002[4]; inputLemak = BR002[5]; inputKarbo = BR002[6]; inputSerat = BR002[7]; inputCarbing = BR002[8]; inputType = BR002[9]; inputCode = BR002[10]; break;
          case 163 : inputName = BR003[1]; inputAir = BR003[2]; inputEnergy = BR003[3]; inputProtein = BR003[4]; inputLemak = BR003[5]; inputKarbo = BR003[6]; inputSerat = BR003[7]; inputCarbing = BR003[8]; inputType = BR003[9]; inputCode = BR003[10]; break;
          case 164 : inputName = BR004[1]; inputAir = BR004[2]; inputEnergy = BR004[3]; inputProtein = BR004[4]; inputLemak = BR004[5]; inputKarbo = BR004[6]; inputSerat = BR004[7]; inputCarbing = BR004[8]; inputType = BR004[9]; inputCode = BR004[10]; break;
          case 165 : inputName = BR005[1]; inputAir = BR005[2]; inputEnergy = BR005[3]; inputProtein = BR005[4]; inputLemak = BR005[5]; inputKarbo = BR005[6]; inputSerat = BR005[7]; inputCarbing = BR005[8]; inputType = BR005[9]; inputCode = BR005[10]; break;
          case 1611 : inputName = BR006[1]; inputAir = BR006[2]; inputEnergy = BR006[3]; inputProtein = BR006[4]; inputLemak = BR006[5]; inputKarbo = BR006[6]; inputSerat = BR006[7]; inputCarbing = BR006[8]; inputType = BR006[9]; inputCode = BR006[10]; break;
          case 1612 : inputName = BR007[1]; inputAir = BR007[2]; inputEnergy = BR007[3]; inputProtein = BR007[4]; inputLemak = BR007[5]; inputKarbo = BR007[6]; inputSerat = BR007[7]; inputCarbing = BR007[8]; inputType = BR007[9]; inputCode = BR007[10]; break;
          case 1613 : inputName = BR008[1]; inputAir = BR008[2]; inputEnergy = BR008[3]; inputProtein = BR008[4]; inputLemak = BR008[5]; inputKarbo = BR008[6]; inputSerat = BR008[7]; inputCarbing = BR008[8]; inputType = BR008[9]; inputCode = BR008[10]; break;
          case 1614 : inputName = BR009[1]; inputAir = BR009[2]; inputEnergy = BR009[3]; inputProtein = BR009[4]; inputLemak = BR009[5]; inputKarbo = BR009[6]; inputSerat = BR009[7]; inputCarbing = BR009[8]; inputType = BR009[9]; inputCode = BR009[10]; break;
          case 1615 : inputName = BR010[1]; inputAir = BR010[2]; inputEnergy = BR010[3]; inputProtein = BR010[4]; inputLemak = BR010[5]; inputKarbo = BR010[6]; inputSerat = BR010[7]; inputCarbing = BR010[8]; inputType = BR010[9]; inputCode = BR010[10]; break;
          case 1621 : inputName = BR011[1]; inputAir = BR011[2]; inputEnergy = BR011[3]; inputProtein = BR011[4]; inputLemak = BR011[5]; inputKarbo = BR011[6]; inputSerat = BR011[7]; inputCarbing = BR011[8]; inputType = BR011[9]; inputCode = BR011[10]; break;
          case 1622 : inputName = BR012[1]; inputAir = BR012[2]; inputEnergy = BR012[3]; inputProtein = BR012[4]; inputLemak = BR012[5]; inputKarbo = BR012[6]; inputSerat = BR012[7]; inputCarbing = BR012[8]; inputType = BR012[9]; inputCode = BR012[10]; break;
          case 1623 : inputName = BR013[1]; inputAir = BR013[2]; inputEnergy = BR013[3]; inputProtein = BR013[4]; inputLemak = BR013[5]; inputKarbo = BR013[6]; inputSerat = BR013[7]; inputCarbing = BR013[8]; inputType = BR013[9]; inputCode = BR013[10]; break;
          case 1624 : inputName = BR014[1]; inputAir = BR014[2]; inputEnergy = BR014[3]; inputProtein = BR014[4]; inputLemak = BR014[5]; inputKarbo = BR014[6]; inputSerat = BR014[7]; inputCarbing = BR014[8]; inputType = BR014[9]; inputCode = BR014[10]; break;
          case 1625 : inputName = BR015[1]; inputAir = BR015[2]; inputEnergy = BR015[3]; inputProtein = BR015[4]; inputLemak = BR015[5]; inputKarbo = BR015[6]; inputSerat = BR015[7]; inputCarbing = BR015[8]; inputType = BR015[9]; inputCode = BR015[10]; break;
          case 1631 : inputName = BR016[1]; inputAir = BR016[2]; inputEnergy = BR016[3]; inputProtein = BR016[4]; inputLemak = BR016[5]; inputKarbo = BR016[6]; inputSerat = BR016[7]; inputCarbing = BR016[8]; inputType = BR016[9]; inputCode = BR016[10]; break;
          case 1632 : inputName = BR017[1]; inputAir = BR017[2]; inputEnergy = BR017[3]; inputProtein = BR017[4]; inputLemak = BR017[5]; inputKarbo = BR017[6]; inputSerat = BR017[7]; inputCarbing = BR017[8]; inputType = BR017[9]; inputCode = BR017[10]; break;
          case 1633 : inputName = BR018[1]; inputAir = BR018[2]; inputEnergy = BR018[3]; inputProtein = BR018[4]; inputLemak = BR018[5]; inputKarbo = BR018[6]; inputSerat = BR018[7]; inputCarbing = BR018[8]; inputType = BR018[9]; inputCode = BR018[10]; break;
          case 1634 : inputName = BR019[1]; inputAir = BR019[2]; inputEnergy = BR019[3]; inputProtein = BR019[4]; inputLemak = BR019[5]; inputKarbo = BR019[6]; inputSerat = BR019[7]; inputCarbing = BR019[8]; inputType = BR019[9]; inputCode = BR019[10]; break;
          case 1635 : inputName = BR020[1]; inputAir = BR020[2]; inputEnergy = BR020[3]; inputProtein = BR020[4]; inputLemak = BR020[5]; inputKarbo = BR020[6]; inputSerat = BR020[7]; inputCarbing = BR020[8]; inputType = BR020[9]; inputCode = BR020[10]; break;
          case 1641 : inputName = BR021[1]; inputAir = BR021[2]; inputEnergy = BR021[3]; inputProtein = BR021[4]; inputLemak = BR021[5]; inputKarbo = BR021[6]; inputSerat = BR021[7]; inputCarbing = BR021[8]; inputType = BR021[9]; inputCode = BR021[10]; break;
          case 1642 : inputName = BR022[1]; inputAir = BR022[2]; inputEnergy = BR022[3]; inputProtein = BR022[4]; inputLemak = BR022[5]; inputKarbo = BR022[6]; inputSerat = BR022[7]; inputCarbing = BR022[8]; inputType = BR022[9]; inputCode = BR022[10]; break;
          case 1643 : inputName = BR023[1]; inputAir = BR023[2]; inputEnergy = BR023[3]; inputProtein = BR023[4]; inputLemak = BR023[5]; inputKarbo = BR023[6]; inputSerat = BR023[7]; inputCarbing = BR023[8]; inputType = BR023[9]; inputCode = BR023[10]; break;
          case 1644 : inputName = BR024[1]; inputAir = BR024[2]; inputEnergy = BR024[3]; inputProtein = BR024[4]; inputLemak = BR024[5]; inputKarbo = BR024[6]; inputSerat = BR024[7]; inputCarbing = BR024[8]; inputType = BR024[9]; inputCode = BR024[10]; break;
          case 1645 : inputName = BR025[1]; inputAir = BR025[2]; inputEnergy = BR025[3]; inputProtein = BR025[4]; inputLemak = BR025[5]; inputKarbo = BR025[6]; inputSerat = BR025[7]; inputCarbing = BR025[8]; inputType = BR025[9]; inputCode = BR025[10]; break;
          case 1651 : inputName = BR026[1]; inputAir = BR026[2]; inputEnergy = BR026[3]; inputProtein = BR026[4]; inputLemak = BR026[5]; inputKarbo = BR026[6]; inputSerat = BR026[7]; inputCarbing = BR026[8]; inputType = BR026[9]; inputCode = BR026[10]; break;
          case 1652 : inputName = BR027[1]; inputAir = BR027[2]; inputEnergy = BR027[3]; inputProtein = BR027[4]; inputLemak = BR027[5]; inputKarbo = BR027[6]; inputSerat = BR027[7]; inputCarbing = BR027[8]; inputType = BR027[9]; inputCode = BR027[10]; break;
          case 1653 : inputName = BR028[1]; inputAir = BR028[2]; inputEnergy = BR028[3]; inputProtein = BR028[4]; inputLemak = BR028[5]; inputKarbo = BR028[6]; inputSerat = BR028[7]; inputCarbing = BR028[8]; inputType = BR028[9]; inputCode = BR028[10]; break;
          case 1654 : inputName = BR029[1]; inputAir = BR029[2]; inputEnergy = BR029[3]; inputProtein = BR029[4]; inputLemak = BR029[5]; inputKarbo = BR029[6]; inputSerat = BR029[7]; inputCarbing = BR029[8]; inputType = BR029[9]; inputCode = BR029[10]; break;
          case 1655 : inputName = BR030[1]; inputAir = BR030[2]; inputEnergy = BR030[3]; inputProtein = BR030[4]; inputLemak = BR030[5]; inputKarbo = BR030[6]; inputSerat = BR030[7]; inputCarbing = BR030[8]; inputType = BR030[9]; inputCode = BR030[10]; break;
          case 16111 : inputName = BR031[1]; inputAir = BR031[2]; inputEnergy = BR031[3]; inputProtein = BR031[4]; inputLemak = BR031[5]; inputKarbo = BR031[6]; inputSerat = BR031[7]; inputCarbing = BR031[8]; inputType = BR031[9]; inputCode = BR031[10]; break;
          case 16112 : inputName = BR032[1]; inputAir = BR032[2]; inputEnergy = BR032[3]; inputProtein = BR032[4]; inputLemak = BR032[5]; inputKarbo = BR032[6]; inputSerat = BR032[7]; inputCarbing = BR032[8]; inputType = BR032[9]; inputCode = BR032[10]; break;
          case 16113 : inputName = BR033[1]; inputAir = BR033[2]; inputEnergy = BR033[3]; inputProtein = BR033[4]; inputLemak = BR033[5]; inputKarbo = BR033[6]; inputSerat = BR033[7]; inputCarbing = BR033[8]; inputType = BR033[9]; inputCode = BR033[10]; break;
          case 16114 : inputName = BP001[1]; inputAir = BP001[2]; inputEnergy = BP001[3]; inputProtein = BP001[4]; inputLemak = BP001[5]; inputKarbo = BP001[6]; inputSerat = BP001[7]; inputCarbing = BP001[8]; inputType = BP001[9]; inputCode = BP001[10]; break;
          case 16115 : inputName = BP002[1]; inputAir = BP002[2]; inputEnergy = BP002[3]; inputProtein = BP002[4]; inputLemak = BP002[5]; inputKarbo = BP002[6]; inputSerat = BP002[7]; inputCarbing = BP002[8]; inputType = BP002[9]; inputCode = BP002[10]; break;
          case 16121 : inputName = BP003[1]; inputAir = BP003[2]; inputEnergy = BP003[3]; inputProtein = BP003[4]; inputLemak = BP003[5]; inputKarbo = BP003[6]; inputSerat = BP003[7]; inputCarbing = BP003[8]; inputType = BP003[9]; inputCode = BP003[10]; break;
          case 16122 : inputName = BP004[1]; inputAir = BP004[2]; inputEnergy = BP004[3]; inputProtein = BP004[4]; inputLemak = BP004[5]; inputKarbo = BP004[6]; inputSerat = BP004[7]; inputCarbing = BP004[8]; inputType = BP004[9]; inputCode = BP004[10]; break;
          case 16123 : inputName = BP005[1]; inputAir = BP005[2]; inputEnergy = BP005[3]; inputProtein = BP005[4]; inputLemak = BP005[5]; inputKarbo = BP005[6]; inputSerat = BP005[7]; inputCarbing = BP005[8]; inputType = BP005[9]; inputCode = BP005[10]; break;
          case 16124 : inputName = BP006[1]; inputAir = BP006[2]; inputEnergy = BP006[3]; inputProtein = BP006[4]; inputLemak = BP006[5]; inputKarbo = BP006[6]; inputSerat = BP006[7]; inputCarbing = BP006[8]; inputType = BP006[9]; inputCode = BP006[10]; break;
          case 16125 : inputName = BP007[1]; inputAir = BP007[2]; inputEnergy = BP007[3]; inputProtein = BP007[4]; inputLemak = BP007[5]; inputKarbo = BP007[6]; inputSerat = BP007[7]; inputCarbing = BP007[8]; inputType = BP007[9]; inputCode = BP007[10]; break;
          case 16131 : inputName = BP008[1]; inputAir = BP008[2]; inputEnergy = BP008[3]; inputProtein = BP008[4]; inputLemak = BP008[5]; inputKarbo = BP008[6]; inputSerat = BP008[7]; inputCarbing = BP008[8]; inputType = BP008[9]; inputCode = BP008[10]; break;
          case 16132 : inputName = BP009[1]; inputAir = BP009[2]; inputEnergy = BP009[3]; inputProtein = BP009[4]; inputLemak = BP009[5]; inputKarbo = BP009[6]; inputSerat = BP009[7]; inputCarbing = BP009[8]; inputType = BP009[9]; inputCode = BP009[10]; break;
          case 16133 : inputName = BP010[1]; inputAir = BP010[2]; inputEnergy = BP010[3]; inputProtein = BP010[4]; inputLemak = BP010[5]; inputKarbo = BP010[6]; inputSerat = BP010[7]; inputCarbing = BP010[8]; inputType = BP010[9]; inputCode = BP010[10]; break;
          case 16134 : inputName = BP011[1]; inputAir = BP011[2]; inputEnergy = BP011[3]; inputProtein = BP011[4]; inputLemak = BP011[5]; inputKarbo = BP011[6]; inputSerat = BP011[7]; inputCarbing = BP011[8]; inputType = BP011[9]; inputCode = BP011[10]; break;
          case 16135 : inputName = BP012[1]; inputAir = BP012[2]; inputEnergy = BP012[3]; inputProtein = BP012[4]; inputLemak = BP012[5]; inputKarbo = BP012[6]; inputSerat = BP012[7]; inputCarbing = BP012[8]; inputType = BP012[9]; inputCode = BP012[10]; break;
          case 16141 : inputName = BP013[1]; inputAir = BP013[2]; inputEnergy = BP013[3]; inputProtein = BP013[4]; inputLemak = BP013[5]; inputKarbo = BP013[6]; inputSerat = BP013[7]; inputCarbing = BP013[8]; inputType = BP013[9]; inputCode = BP013[10]; break;
          case 16142 : inputName = BP014[1]; inputAir = BP014[2]; inputEnergy = BP014[3]; inputProtein = BP014[4]; inputLemak = BP014[5]; inputKarbo = BP014[6]; inputSerat = BP014[7]; inputCarbing = BP014[8]; inputType = BP014[9]; inputCode = BP014[10]; break;
          case 16143 : inputName = BP015[1]; inputAir = BP015[2]; inputEnergy = BP015[3]; inputProtein = BP015[4]; inputLemak = BP015[5]; inputKarbo = BP015[6]; inputSerat = BP015[7]; inputCarbing = BP015[8]; inputType = BP015[9]; inputCode = BP015[10]; break;
          case 16144 : inputName = BP016[1]; inputAir = BP016[2]; inputEnergy = BP016[3]; inputProtein = BP016[4]; inputLemak = BP016[5]; inputKarbo = BP016[6]; inputSerat = BP016[7]; inputCarbing = BP016[8]; inputType = BP016[9]; inputCode = BP016[10]; break;
          case 16145 : inputName = BP017[1]; inputAir = BP017[2]; inputEnergy = BP017[3]; inputProtein = BP017[4]; inputLemak = BP017[5]; inputKarbo = BP017[6]; inputSerat = BP017[7]; inputCarbing = BP017[8]; inputType = BP017[9]; inputCode = BP017[10]; break;
          case 16151 : inputName = BP018[1]; inputAir = BP018[2]; inputEnergy = BP018[3]; inputProtein = BP018[4]; inputLemak = BP018[5]; inputKarbo = BP018[6]; inputSerat = BP018[7]; inputCarbing = BP018[8]; inputType = BP018[9]; inputCode = BP018[10]; break;
          case 16152 : inputName = BP019[1]; inputAir = BP019[2]; inputEnergy = BP019[3]; inputProtein = BP019[4]; inputLemak = BP019[5]; inputKarbo = BP019[6]; inputSerat = BP019[7]; inputCarbing = BP019[8]; inputType = BP019[9]; inputCode = BP019[10]; break;
          case 16153 : inputName = BP020[1]; inputAir = BP020[2]; inputEnergy = BP020[3]; inputProtein = BP020[4]; inputLemak = BP020[5]; inputKarbo = BP020[6]; inputSerat = BP020[7]; inputCarbing = BP020[8]; inputType = BP020[9]; inputCode = BP020[10]; break;
          case 16154 : inputName = BP021[1]; inputAir = BP021[2]; inputEnergy = BP021[3]; inputProtein = BP021[4]; inputLemak = BP021[5]; inputKarbo = BP021[6]; inputSerat = BP021[7]; inputCarbing = BP021[8]; inputType = BP021[9]; inputCode = BP021[10]; break;
          case 16155 : inputName = BP022[1]; inputAir = BP022[2]; inputEnergy = BP022[3]; inputProtein = BP022[4]; inputLemak = BP022[5]; inputKarbo = BP022[6]; inputSerat = BP022[7]; inputCarbing = BP022[8]; inputType = BP022[9]; inputCode = BP022[10]; break;
          case 16211 : inputName = BP023[1]; inputAir = BP023[2]; inputEnergy = BP023[3]; inputProtein = BP023[4]; inputLemak = BP023[5]; inputKarbo = BP023[6]; inputSerat = BP023[7]; inputCarbing = BP023[8]; inputType = BP023[9]; inputCode = BP023[10]; break;
          case 16212 : inputName = BP024[1]; inputAir = BP024[2]; inputEnergy = BP024[3]; inputProtein = BP024[4]; inputLemak = BP024[5]; inputKarbo = BP024[6]; inputSerat = BP024[7]; inputCarbing = BP024[8]; inputType = BP024[9]; inputCode = BP024[10]; break;
          case 16213 : inputName = BP025[1]; inputAir = BP025[2]; inputEnergy = BP025[3]; inputProtein = BP025[4]; inputLemak = BP025[5]; inputKarbo = BP025[6]; inputSerat = BP025[7]; inputCarbing = BP025[8]; inputType = BP025[9]; inputCode = BP025[10]; break;
          case 16214 : inputName = BP026[1]; inputAir = BP026[2]; inputEnergy = BP026[3]; inputProtein = BP026[4]; inputLemak = BP026[5]; inputKarbo = BP026[6]; inputSerat = BP026[7]; inputCarbing = BP026[8]; inputType = BP026[9]; inputCode = BP026[10]; break;
          case 16215 : inputName = BP027[1]; inputAir = BP027[2]; inputEnergy = BP027[3]; inputProtein = BP027[4]; inputLemak = BP027[5]; inputKarbo = BP027[6]; inputSerat = BP027[7]; inputCarbing = BP027[8]; inputType = BP027[9]; inputCode = BP027[10]; break;
          case 16221 : inputName = BP028[1]; inputAir = BP028[2]; inputEnergy = BP028[3]; inputProtein = BP028[4]; inputLemak = BP028[5]; inputKarbo = BP028[6]; inputSerat = BP028[7]; inputCarbing = BP028[8]; inputType = BP028[9]; inputCode = BP028[10]; break;
          case 16222 : inputName = BP029[1]; inputAir = BP029[2]; inputEnergy = BP029[3]; inputProtein = BP029[4]; inputLemak = BP029[5]; inputKarbo = BP029[6]; inputSerat = BP029[7]; inputCarbing = BP029[8]; inputType = BP029[9]; inputCode = BP029[10]; break;
          case 16223 : inputName = BP030[1]; inputAir = BP030[2]; inputEnergy = BP030[3]; inputProtein = BP030[4]; inputLemak = BP030[5]; inputKarbo = BP030[6]; inputSerat = BP030[7]; inputCarbing = BP030[8]; inputType = BP030[9]; inputCode = BP030[10]; break;
          case 16224 : inputName = BP031[1]; inputAir = BP031[2]; inputEnergy = BP031[3]; inputProtein = BP031[4]; inputLemak = BP031[5]; inputKarbo = BP031[6]; inputSerat = BP031[7]; inputCarbing = BP031[8]; inputType = BP031[9]; inputCode = BP031[10]; break;
          case 16225 : inputName = BP032[1]; inputAir = BP032[2]; inputEnergy = BP032[3]; inputProtein = BP032[4]; inputLemak = BP032[5]; inputKarbo = BP032[6]; inputSerat = BP032[7]; inputCarbing = BP032[8]; inputType = BP032[9]; inputCode = BP032[10]; break;
          case 16231 : inputName = BP033[1]; inputAir = BP033[2]; inputEnergy = BP033[3]; inputProtein = BP033[4]; inputLemak = BP033[5]; inputKarbo = BP033[6]; inputSerat = BP033[7]; inputCarbing = BP033[8]; inputType = BP033[9]; inputCode = BP033[10]; break;
          case 16232 : inputName = BP034[1]; inputAir = BP034[2]; inputEnergy = BP034[3]; inputProtein = BP034[4]; inputLemak = BP034[5]; inputKarbo = BP034[6]; inputSerat = BP034[7]; inputCarbing = BP034[8]; inputType = BP034[9]; inputCode = BP034[10]; break;
          case 16233 : inputName = BP035[1]; inputAir = BP035[2]; inputEnergy = BP035[3]; inputProtein = BP035[4]; inputLemak = BP035[5]; inputKarbo = BP035[6]; inputSerat = BP035[7]; inputCarbing = BP035[8]; inputType = BP035[9]; inputCode = BP035[10]; break;
          case 16234 : inputName = BP036[1]; inputAir = BP036[2]; inputEnergy = BP036[3]; inputProtein = BP036[4]; inputLemak = BP036[5]; inputKarbo = BP036[6]; inputSerat = BP036[7]; inputCarbing = BP036[8]; inputType = BP036[9]; inputCode = BP036[10]; break;
          case 16235 : inputName = BP037[1]; inputAir = BP037[2]; inputEnergy = BP037[3]; inputProtein = BP037[4]; inputLemak = BP037[5]; inputKarbo = BP037[6]; inputSerat = BP037[7]; inputCarbing = BP037[8]; inputType = BP037[9]; inputCode = BP037[10]; break;
          case 16241 : inputName = BP038[1]; inputAir = BP038[2]; inputEnergy = BP038[3]; inputProtein = BP038[4]; inputLemak = BP038[5]; inputKarbo = BP038[6]; inputSerat = BP038[7]; inputCarbing = BP038[8]; inputType = BP038[9]; inputCode = BP038[10]; break;
          case 16242 : inputName = BP039[1]; inputAir = BP039[2]; inputEnergy = BP039[3]; inputProtein = BP039[4]; inputLemak = BP039[5]; inputKarbo = BP039[6]; inputSerat = BP039[7]; inputCarbing = BP039[8]; inputType = BP039[9]; inputCode = BP039[10]; break;
          case 16243 : inputName = BP040[1]; inputAir = BP040[2]; inputEnergy = BP040[3]; inputProtein = BP040[4]; inputLemak = BP040[5]; inputKarbo = BP040[6]; inputSerat = BP040[7]; inputCarbing = BP040[8]; inputType = BP040[9]; inputCode = BP040[10]; break;
          case 16244 : inputName = BP041[1]; inputAir = BP041[2]; inputEnergy = BP041[3]; inputProtein = BP041[4]; inputLemak = BP041[5]; inputKarbo = BP041[6]; inputSerat = BP041[7]; inputCarbing = BP041[8]; inputType = BP041[9]; inputCode = BP041[10]; break;
          case 16245 : inputName = BP042[1]; inputAir = BP042[2]; inputEnergy = BP042[3]; inputProtein = BP042[4]; inputLemak = BP042[5]; inputKarbo = BP042[6]; inputSerat = BP042[7]; inputCarbing = BP042[8]; inputType = BP042[9]; inputCode = BP042[10]; break;
          case 16251 : inputName = BP043[1]; inputAir = BP043[2]; inputEnergy = BP043[3]; inputProtein = BP043[4]; inputLemak = BP043[5]; inputKarbo = BP043[6]; inputSerat = BP043[7]; inputCarbing = BP043[8]; inputType = BP043[9]; inputCode = BP043[10]; break;
          case 16252 : inputName = BP044[1]; inputAir = BP044[2]; inputEnergy = BP044[3]; inputProtein = BP044[4]; inputLemak = BP044[5]; inputKarbo = BP044[6]; inputSerat = BP044[7]; inputCarbing = BP044[8]; inputType = BP044[9]; inputCode = BP044[10]; break;
          case 16253 : inputName = BP045[1]; inputAir = BP045[2]; inputEnergy = BP045[3]; inputProtein = BP045[4]; inputLemak = BP045[5]; inputKarbo = BP045[6]; inputSerat = BP045[7]; inputCarbing = BP045[8]; inputType = BP045[9]; inputCode = BP045[10]; break;
          case 16254 : inputName = BP046[1]; inputAir = BP046[2]; inputEnergy = BP046[3]; inputProtein = BP046[4]; inputLemak = BP046[5]; inputKarbo = BP046[6]; inputSerat = BP046[7]; inputCarbing = BP046[8]; inputType = BP046[9]; inputCode = BP046[10]; break;
          case 16255 : inputName = BP047[1]; inputAir = BP047[2]; inputEnergy = BP047[3]; inputProtein = BP047[4]; inputLemak = BP047[5]; inputKarbo = BP047[6]; inputSerat = BP047[7]; inputCarbing = BP047[8]; inputType = BP047[9]; inputCode = BP047[10]; break;
          case 16311 : inputName = BP048[1]; inputAir = BP048[2]; inputEnergy = BP048[3]; inputProtein = BP048[4]; inputLemak = BP048[5]; inputKarbo = BP048[6]; inputSerat = BP048[7]; inputCarbing = BP048[8]; inputType = BP048[9]; inputCode = BP048[10]; break;
          case 16312 : inputName = BP049[1]; inputAir = BP049[2]; inputEnergy = BP049[3]; inputProtein = BP049[4]; inputLemak = BP049[5]; inputKarbo = BP049[6]; inputSerat = BP049[7]; inputCarbing = BP049[8]; inputType = BP049[9]; inputCode = BP049[10]; break;
          case 16313 : inputName = BP050[1]; inputAir = BP050[2]; inputEnergy = BP050[3]; inputProtein = BP050[4]; inputLemak = BP050[5]; inputKarbo = BP050[6]; inputSerat = BP050[7]; inputCarbing = BP050[8]; inputType = BP050[9]; inputCode = BP050[10]; break;
          case 16314 : inputName = BP051[1]; inputAir = BP051[2]; inputEnergy = BP051[3]; inputProtein = BP051[4]; inputLemak = BP051[5]; inputKarbo = BP051[6]; inputSerat = BP051[7]; inputCarbing = BP051[8]; inputType = BP051[9]; inputCode = BP051[10]; break;
          case 16315 : inputName = BP052[1]; inputAir = BP052[2]; inputEnergy = BP052[3]; inputProtein = BP052[4]; inputLemak = BP052[5]; inputKarbo = BP052[6]; inputSerat = BP052[7]; inputCarbing = BP052[8]; inputType = BP052[9]; inputCode = BP052[10]; break;
          case 16321 : inputName = BP053[1]; inputAir = BP053[2]; inputEnergy = BP053[3]; inputProtein = BP053[4]; inputLemak = BP053[5]; inputKarbo = BP053[6]; inputSerat = BP053[7]; inputCarbing = BP053[8]; inputType = BP053[9]; inputCode = BP053[10]; break;
          case 16322 : inputName = BP054[1]; inputAir = BP054[2]; inputEnergy = BP054[3]; inputProtein = BP054[4]; inputLemak = BP054[5]; inputKarbo = BP054[6]; inputSerat = BP054[7]; inputCarbing = BP054[8]; inputType = BP054[9]; inputCode = BP054[10]; break;
          case 16323 : inputName = BP055[1]; inputAir = BP055[2]; inputEnergy = BP055[3]; inputProtein = BP055[4]; inputLemak = BP055[5]; inputKarbo = BP055[6]; inputSerat = BP055[7]; inputCarbing = BP055[8]; inputType = BP055[9]; inputCode = BP055[10]; break;
          case 16324 : inputName = BP056[1]; inputAir = BP056[2]; inputEnergy = BP056[3]; inputProtein = BP056[4]; inputLemak = BP056[5]; inputKarbo = BP056[6]; inputSerat = BP056[7]; inputCarbing = BP056[8]; inputType = BP056[9]; inputCode = BP056[10]; break;
          case 16325 : inputName = BP057[1]; inputAir = BP057[2]; inputEnergy = BP057[3]; inputProtein = BP057[4]; inputLemak = BP057[5]; inputKarbo = BP057[6]; inputSerat = BP057[7]; inputCarbing = BP057[8]; inputType = BP057[9]; inputCode = BP057[10]; break;
          case 16331 : inputName = BP058[1]; inputAir = BP058[2]; inputEnergy = BP058[3]; inputProtein = BP058[4]; inputLemak = BP058[5]; inputKarbo = BP058[6]; inputSerat = BP058[7]; inputCarbing = BP058[8]; inputType = BP058[9]; inputCode = BP058[10]; break;
          case 16332 : inputName = BP059[1]; inputAir = BP059[2]; inputEnergy = BP059[3]; inputProtein = BP059[4]; inputLemak = BP059[5]; inputKarbo = BP059[6]; inputSerat = BP059[7]; inputCarbing = BP059[8]; inputType = BP059[9]; inputCode = BP059[10]; break;
          case 16333 : inputName = BP060[1]; inputAir = BP060[2]; inputEnergy = BP060[3]; inputProtein = BP060[4]; inputLemak = BP060[5]; inputKarbo = BP060[6]; inputSerat = BP060[7]; inputCarbing = BP060[8]; inputType = BP060[9]; inputCode = BP060[10]; break;
          case 16334 : inputName = BP061[1]; inputAir = BP061[2]; inputEnergy = BP061[3]; inputProtein = BP061[4]; inputLemak = BP061[5]; inputKarbo = BP061[6]; inputSerat = BP061[7]; inputCarbing = BP061[8]; inputType = BP061[9]; inputCode = BP061[10]; break;
          case 16335 : inputName = BP062[1]; inputAir = BP062[2]; inputEnergy = BP062[3]; inputProtein = BP062[4]; inputLemak = BP062[5]; inputKarbo = BP062[6]; inputSerat = BP062[7]; inputCarbing = BP062[8]; inputType = BP062[9]; inputCode = BP062[10]; break;
          case 16341 : inputName = BP063[1]; inputAir = BP063[2]; inputEnergy = BP063[3]; inputProtein = BP063[4]; inputLemak = BP063[5]; inputKarbo = BP063[6]; inputSerat = BP063[7]; inputCarbing = BP063[8]; inputType = BP063[9]; inputCode = BP063[10]; break;
          case 16342 : inputName = BP064[1]; inputAir = BP064[2]; inputEnergy = BP064[3]; inputProtein = BP064[4]; inputLemak = BP064[5]; inputKarbo = BP064[6]; inputSerat = BP064[7]; inputCarbing = BP064[8]; inputType = BP064[9]; inputCode = BP064[10]; break;
          case 16343 : inputName = BP065[1]; inputAir = BP065[2]; inputEnergy = BP065[3]; inputProtein = BP065[4]; inputLemak = BP065[5]; inputKarbo = BP065[6]; inputSerat = BP065[7]; inputCarbing = BP065[8]; inputType = BP065[9]; inputCode = BP065[10]; break;
          case 16344 : inputName = BP066[1]; inputAir = BP066[2]; inputEnergy = BP066[3]; inputProtein = BP066[4]; inputLemak = BP066[5]; inputKarbo = BP066[6]; inputSerat = BP066[7]; inputCarbing = BP066[8]; inputType = BP066[9]; inputCode = BP066[10]; break;
          case 16345 : inputName = BP067[1]; inputAir = BP067[2]; inputEnergy = BP067[3]; inputProtein = BP067[4]; inputLemak = BP067[5]; inputKarbo = BP067[6]; inputSerat = BP067[7]; inputCarbing = BP067[8]; inputType = BP067[9]; inputCode = BP067[10]; break;
          case 16351 : inputName = BP068[1]; inputAir = BP068[2]; inputEnergy = BP068[3]; inputProtein = BP068[4]; inputLemak = BP068[5]; inputKarbo = BP068[6]; inputSerat = BP068[7]; inputCarbing = BP068[8]; inputType = BP068[9]; inputCode = BP068[10]; break;
          case 16352 : inputName = BP069[1]; inputAir = BP069[2]; inputEnergy = BP069[3]; inputProtein = BP069[4]; inputLemak = BP069[5]; inputKarbo = BP069[6]; inputSerat = BP069[7]; inputCarbing = BP069[8]; inputType = BP069[9]; inputCode = BP069[10]; break;
          case 16353 : inputName = BP070[1]; inputAir = BP070[2]; inputEnergy = BP070[3]; inputProtein = BP070[4]; inputLemak = BP070[5]; inputKarbo = BP070[6]; inputSerat = BP070[7]; inputCarbing = BP070[8]; inputType = BP070[9]; inputCode = BP070[10]; break;
          case 16354 : inputName = BP071[1]; inputAir = BP071[2]; inputEnergy = BP071[3]; inputProtein = BP071[4]; inputLemak = BP071[5]; inputKarbo = BP071[6]; inputSerat = BP071[7]; inputCarbing = BP071[8]; inputType = BP071[9]; inputCode = BP071[10]; break;
          case 16355 : inputName = BP072[1]; inputAir = BP072[2]; inputEnergy = BP072[3]; inputProtein = BP072[4]; inputLemak = BP072[5]; inputKarbo = BP072[6]; inputSerat = BP072[7]; inputCarbing = BP072[8]; inputType = BP072[9]; inputCode = BP072[10]; break;
          case 16411 : inputName = BP073[1]; inputAir = BP073[2]; inputEnergy = BP073[3]; inputProtein = BP073[4]; inputLemak = BP073[5]; inputKarbo = BP073[6]; inputSerat = BP073[7]; inputCarbing = BP073[8]; inputType = BP073[9]; inputCode = BP073[10]; break;
          case 16412 : inputName = BP074[1]; inputAir = BP074[2]; inputEnergy = BP074[3]; inputProtein = BP074[4]; inputLemak = BP074[5]; inputKarbo = BP074[6]; inputSerat = BP074[7]; inputCarbing = BP074[8]; inputType = BP074[9]; inputCode = BP074[10]; break;
          case 16413 : inputName = BP075[1]; inputAir = BP075[2]; inputEnergy = BP075[3]; inputProtein = BP075[4]; inputLemak = BP075[5]; inputKarbo = BP075[6]; inputSerat = BP075[7]; inputCarbing = BP075[8]; inputType = BP075[9]; inputCode = BP075[10]; break;
          case 16414 : inputName = BP076[1]; inputAir = BP076[2]; inputEnergy = BP076[3]; inputProtein = BP076[4]; inputLemak = BP076[5]; inputKarbo = BP076[6]; inputSerat = BP076[7]; inputCarbing = BP076[8]; inputType = BP076[9]; inputCode = BP076[10]; break;
          case 131 : inputName = CR001[1]; inputAir = CR001[2]; inputEnergy = CR001[3]; inputProtein = CR001[4]; inputLemak = CR001[5]; inputKarbo = CR001[6]; inputSerat = CR001[7]; inputCarbing = CR001[8]; inputType = CR001[9]; inputCode = CR001[10]; break;
          case 132 : inputName = CR002[1]; inputAir = CR002[2]; inputEnergy = CR002[3]; inputProtein = CR002[4]; inputLemak = CR002[5]; inputKarbo = CR002[6]; inputSerat = CR002[7]; inputCarbing = CR002[8]; inputType = CR002[9]; inputCode = CR002[10]; break;
          case 133 : inputName = CR003[1]; inputAir = CR003[2]; inputEnergy = CR003[3]; inputProtein = CR003[4]; inputLemak = CR003[5]; inputKarbo = CR003[6]; inputSerat = CR003[7]; inputCarbing = CR003[8]; inputType = CR003[9]; inputCode = CR003[10]; break;
          case 134 : inputName = CR004[1]; inputAir = CR004[2]; inputEnergy = CR004[3]; inputProtein = CR004[4]; inputLemak = CR004[5]; inputKarbo = CR004[6]; inputSerat = CR004[7]; inputCarbing = CR004[8]; inputType = CR004[9]; inputCode = CR004[10]; break;
          case 135 : inputName = CR005[1]; inputAir = CR005[2]; inputEnergy = CR005[3]; inputProtein = CR005[4]; inputLemak = CR005[5]; inputKarbo = CR005[6]; inputSerat = CR005[7]; inputCarbing = CR005[8]; inputType = CR005[9]; inputCode = CR005[10]; break;
          case 1311 : inputName = CR006[1]; inputAir = CR006[2]; inputEnergy = CR006[3]; inputProtein = CR006[4]; inputLemak = CR006[5]; inputKarbo = CR006[6]; inputSerat = CR006[7]; inputCarbing = CR006[8]; inputType = CR006[9]; inputCode = CR006[10]; break;
          case 1312 : inputName = CR007[1]; inputAir = CR007[2]; inputEnergy = CR007[3]; inputProtein = CR007[4]; inputLemak = CR007[5]; inputKarbo = CR007[6]; inputSerat = CR007[7]; inputCarbing = CR007[8]; inputType = CR007[9]; inputCode = CR007[10]; break;
          case 1313 : inputName = CR008[1]; inputAir = CR008[2]; inputEnergy = CR008[3]; inputProtein = CR008[4]; inputLemak = CR008[5]; inputKarbo = CR008[6]; inputSerat = CR008[7]; inputCarbing = CR008[8]; inputType = CR008[9]; inputCode = CR008[10]; break;
          case 1314 : inputName = CR009[1]; inputAir = CR009[2]; inputEnergy = CR009[3]; inputProtein = CR009[4]; inputLemak = CR009[5]; inputKarbo = CR009[6]; inputSerat = CR009[7]; inputCarbing = CR009[8]; inputType = CR009[9]; inputCode = CR009[10]; break;
          case 1315 : inputName = CR010[1]; inputAir = CR010[2]; inputEnergy = CR010[3]; inputProtein = CR010[4]; inputLemak = CR010[5]; inputKarbo = CR010[6]; inputSerat = CR010[7]; inputCarbing = CR010[8]; inputType = CR010[9]; inputCode = CR010[10]; break;
          case 1321 : inputName = CR011[1]; inputAir = CR011[2]; inputEnergy = CR011[3]; inputProtein = CR011[4]; inputLemak = CR011[5]; inputKarbo = CR011[6]; inputSerat = CR011[7]; inputCarbing = CR011[8]; inputType = CR011[9]; inputCode = CR011[10]; break;
          case 1322 : inputName = CR012[1]; inputAir = CR012[2]; inputEnergy = CR012[3]; inputProtein = CR012[4]; inputLemak = CR012[5]; inputKarbo = CR012[6]; inputSerat = CR012[7]; inputCarbing = CR012[8]; inputType = CR012[9]; inputCode = CR012[10]; break;
          case 1323 : inputName = CR013[1]; inputAir = CR013[2]; inputEnergy = CR013[3]; inputProtein = CR013[4]; inputLemak = CR013[5]; inputKarbo = CR013[6]; inputSerat = CR013[7]; inputCarbing = CR013[8]; inputType = CR013[9]; inputCode = CR013[10]; break;
          case 1324 : inputName = CR014[1]; inputAir = CR014[2]; inputEnergy = CR014[3]; inputProtein = CR014[4]; inputLemak = CR014[5]; inputKarbo = CR014[6]; inputSerat = CR014[7]; inputCarbing = CR014[8]; inputType = CR014[9]; inputCode = CR014[10]; break;
          case 1325 : inputName = CR015[1]; inputAir = CR015[2]; inputEnergy = CR015[3]; inputProtein = CR015[4]; inputLemak = CR015[5]; inputKarbo = CR015[6]; inputSerat = CR015[7]; inputCarbing = CR015[8]; inputType = CR015[9]; inputCode = CR015[10]; break;
          case 1331 : inputName = CR016[1]; inputAir = CR016[2]; inputEnergy = CR016[3]; inputProtein = CR016[4]; inputLemak = CR016[5]; inputKarbo = CR016[6]; inputSerat = CR016[7]; inputCarbing = CR016[8]; inputType = CR016[9]; inputCode = CR016[10]; break;
          case 1332 : inputName = CR017[1]; inputAir = CR017[2]; inputEnergy = CR017[3]; inputProtein = CR017[4]; inputLemak = CR017[5]; inputKarbo = CR017[6]; inputSerat = CR017[7]; inputCarbing = CR017[8]; inputType = CR017[9]; inputCode = CR017[10]; break;
          case 1333 : inputName = CR018[1]; inputAir = CR018[2]; inputEnergy = CR018[3]; inputProtein = CR018[4]; inputLemak = CR018[5]; inputKarbo = CR018[6]; inputSerat = CR018[7]; inputCarbing = CR018[8]; inputType = CR018[9]; inputCode = CR018[10]; break;
          case 1334 : inputName = CR019[1]; inputAir = CR019[2]; inputEnergy = CR019[3]; inputProtein = CR019[4]; inputLemak = CR019[5]; inputKarbo = CR019[6]; inputSerat = CR019[7]; inputCarbing = CR019[8]; inputType = CR019[9]; inputCode = CR019[10]; break;
          case 1335 : inputName = CR020[1]; inputAir = CR020[2]; inputEnergy = CR020[3]; inputProtein = CR020[4]; inputLemak = CR020[5]; inputKarbo = CR020[6]; inputSerat = CR020[7]; inputCarbing = CR020[8]; inputType = CR020[9]; inputCode = CR020[10]; break;
          case 1341 : inputName = CR021[1]; inputAir = CR021[2]; inputEnergy = CR021[3]; inputProtein = CR021[4]; inputLemak = CR021[5]; inputKarbo = CR021[6]; inputSerat = CR021[7]; inputCarbing = CR021[8]; inputType = CR021[9]; inputCode = CR021[10]; break;
          case 1342 : inputName = CR022[1]; inputAir = CR022[2]; inputEnergy = CR022[3]; inputProtein = CR022[4]; inputLemak = CR022[5]; inputKarbo = CR022[6]; inputSerat = CR022[7]; inputCarbing = CR022[8]; inputType = CR022[9]; inputCode = CR022[10]; break;
          case 1343 : inputName = CR023[1]; inputAir = CR023[2]; inputEnergy = CR023[3]; inputProtein = CR023[4]; inputLemak = CR023[5]; inputKarbo = CR023[6]; inputSerat = CR023[7]; inputCarbing = CR023[8]; inputType = CR023[9]; inputCode = CR023[10]; break;
          case 1344 : inputName = CR024[1]; inputAir = CR024[2]; inputEnergy = CR024[3]; inputProtein = CR024[4]; inputLemak = CR024[5]; inputKarbo = CR024[6]; inputSerat = CR024[7]; inputCarbing = CR024[8]; inputType = CR024[9]; inputCode = CR024[10]; break;
          case 1345 : inputName = CR025[1]; inputAir = CR025[2]; inputEnergy = CR025[3]; inputProtein = CR025[4]; inputLemak = CR025[5]; inputKarbo = CR025[6]; inputSerat = CR025[7]; inputCarbing = CR025[8]; inputType = CR025[9]; inputCode = CR025[10]; break;
          case 1351 : inputName = CR026[1]; inputAir = CR026[2]; inputEnergy = CR026[3]; inputProtein = CR026[4]; inputLemak = CR026[5]; inputKarbo = CR026[6]; inputSerat = CR026[7]; inputCarbing = CR026[8]; inputType = CR026[9]; inputCode = CR026[10]; break;
          case 1352 : inputName = CR027[1]; inputAir = CR027[2]; inputEnergy = CR027[3]; inputProtein = CR027[4]; inputLemak = CR027[5]; inputKarbo = CR027[6]; inputSerat = CR027[7]; inputCarbing = CR027[8]; inputType = CR027[9]; inputCode = CR027[10]; break;
          case 1353 : inputName = CR028[1]; inputAir = CR028[2]; inputEnergy = CR028[3]; inputProtein = CR028[4]; inputLemak = CR028[5]; inputKarbo = CR028[6]; inputSerat = CR028[7]; inputCarbing = CR028[8]; inputType = CR028[9]; inputCode = CR028[10]; break;
          case 1354 : inputName = CR029[1]; inputAir = CR029[2]; inputEnergy = CR029[3]; inputProtein = CR029[4]; inputLemak = CR029[5]; inputKarbo = CR029[6]; inputSerat = CR029[7]; inputCarbing = CR029[8]; inputType = CR029[9]; inputCode = CR029[10]; break;
          case 1355 : inputName = CR030[1]; inputAir = CR030[2]; inputEnergy = CR030[3]; inputProtein = CR030[4]; inputLemak = CR030[5]; inputKarbo = CR030[6]; inputSerat = CR030[7]; inputCarbing = CR030[8]; inputType = CR030[9]; inputCode = CR030[10]; break;
          case 13111 : inputName = CR031[1]; inputAir = CR031[2]; inputEnergy = CR031[3]; inputProtein = CR031[4]; inputLemak = CR031[5]; inputKarbo = CR031[6]; inputSerat = CR031[7]; inputCarbing = CR031[8]; inputType = CR031[9]; inputCode = CR031[10]; break;
          case 13112 : inputName = CR032[1]; inputAir = CR032[2]; inputEnergy = CR032[3]; inputProtein = CR032[4]; inputLemak = CR032[5]; inputKarbo = CR032[6]; inputSerat = CR032[7]; inputCarbing = CR032[8]; inputType = CR032[9]; inputCode = CR032[10]; break;
          case 13113 : inputName = CR033[1]; inputAir = CR033[2]; inputEnergy = CR033[3]; inputProtein = CR033[4]; inputLemak = CR033[5]; inputKarbo = CR033[6]; inputSerat = CR033[7]; inputCarbing = CR033[8]; inputType = CR033[9]; inputCode = CR033[10]; break;
          case 13114 : inputName = CR034[1]; inputAir = CR034[2]; inputEnergy = CR034[3]; inputProtein = CR034[4]; inputLemak = CR034[5]; inputKarbo = CR034[6]; inputSerat = CR034[7]; inputCarbing = CR034[8]; inputType = CR034[9]; inputCode = CR034[10]; break;
          case 13115 : inputName = CR035[1]; inputAir = CR035[2]; inputEnergy = CR035[3]; inputProtein = CR035[4]; inputLemak = CR035[5]; inputKarbo = CR035[6]; inputSerat = CR035[7]; inputCarbing = CR035[8]; inputType = CR035[9]; inputCode = CR035[10]; break;
          case 13121 : inputName = CR036[1]; inputAir = CR036[2]; inputEnergy = CR036[3]; inputProtein = CR036[4]; inputLemak = CR036[5]; inputKarbo = CR036[6]; inputSerat = CR036[7]; inputCarbing = CR036[8]; inputType = CR036[9]; inputCode = CR036[10]; break;
          case 13122 : inputName = CR037[1]; inputAir = CR037[2]; inputEnergy = CR037[3]; inputProtein = CR037[4]; inputLemak = CR037[5]; inputKarbo = CR037[6]; inputSerat = CR037[7]; inputCarbing = CR037[8]; inputType = CR037[9]; inputCode = CR037[10]; break;
          case 13123 : inputName = CR038[1]; inputAir = CR038[2]; inputEnergy = CR038[3]; inputProtein = CR038[4]; inputLemak = CR038[5]; inputKarbo = CR038[6]; inputSerat = CR038[7]; inputCarbing = CR038[8]; inputType = CR038[9]; inputCode = CR038[10]; break;
          case 13124 : inputName = CR039[1]; inputAir = CR039[2]; inputEnergy = CR039[3]; inputProtein = CR039[4]; inputLemak = CR039[5]; inputKarbo = CR039[6]; inputSerat = CR039[7]; inputCarbing = CR039[8]; inputType = CR039[9]; inputCode = CR039[10]; break;
          case 13125 : inputName = CR040[1]; inputAir = CR040[2]; inputEnergy = CR040[3]; inputProtein = CR040[4]; inputLemak = CR040[5]; inputKarbo = CR040[6]; inputSerat = CR040[7]; inputCarbing = CR040[8]; inputType = CR040[9]; inputCode = CR040[10]; break;
          case 13131 : inputName = CR041[1]; inputAir = CR041[2]; inputEnergy = CR041[3]; inputProtein = CR041[4]; inputLemak = CR041[5]; inputKarbo = CR041[6]; inputSerat = CR041[7]; inputCarbing = CR041[8]; inputType = CR041[9]; inputCode = CR041[10]; break;
          case 13132 : inputName = CR042[1]; inputAir = CR042[2]; inputEnergy = CR042[3]; inputProtein = CR042[4]; inputLemak = CR042[5]; inputKarbo = CR042[6]; inputSerat = CR042[7]; inputCarbing = CR042[8]; inputType = CR042[9]; inputCode = CR042[10]; break;
          case 13133 : inputName = CR043[1]; inputAir = CR043[2]; inputEnergy = CR043[3]; inputProtein = CR043[4]; inputLemak = CR043[5]; inputKarbo = CR043[6]; inputSerat = CR043[7]; inputCarbing = CR043[8]; inputType = CR043[9]; inputCode = CR043[10]; break;
          case 13134 : inputName = CR044[1]; inputAir = CR044[2]; inputEnergy = CR044[3]; inputProtein = CR044[4]; inputLemak = CR044[5]; inputKarbo = CR044[6]; inputSerat = CR044[7]; inputCarbing = CR044[8]; inputType = CR044[9]; inputCode = CR044[10]; break;
          case 13135 : inputName = CR045[1]; inputAir = CR045[2]; inputEnergy = CR045[3]; inputProtein = CR045[4]; inputLemak = CR045[5]; inputKarbo = CR045[6]; inputSerat = CR045[7]; inputCarbing = CR045[8]; inputType = CR045[9]; inputCode = CR045[10]; break;
          case 13141 : inputName = CR046[1]; inputAir = CR046[2]; inputEnergy = CR046[3]; inputProtein = CR046[4]; inputLemak = CR046[5]; inputKarbo = CR046[6]; inputSerat = CR046[7]; inputCarbing = CR046[8]; inputType = CR046[9]; inputCode = CR046[10]; break;
          case 13142 : inputName = CR047[1]; inputAir = CR047[2]; inputEnergy = CR047[3]; inputProtein = CR047[4]; inputLemak = CR047[5]; inputKarbo = CR047[6]; inputSerat = CR047[7]; inputCarbing = CR047[8]; inputType = CR047[9]; inputCode = CR047[10]; break;
          case 13143 : inputName = CR048[1]; inputAir = CR048[2]; inputEnergy = CR048[3]; inputProtein = CR048[4]; inputLemak = CR048[5]; inputKarbo = CR048[6]; inputSerat = CR048[7]; inputCarbing = CR048[8]; inputType = CR048[9]; inputCode = CR048[10]; break;
          case 13144 : inputName = CR049[1]; inputAir = CR049[2]; inputEnergy = CR049[3]; inputProtein = CR049[4]; inputLemak = CR049[5]; inputKarbo = CR049[6]; inputSerat = CR049[7]; inputCarbing = CR049[8]; inputType = CR049[9]; inputCode = CR049[10]; break;
          case 13145 : inputName = CR050[1]; inputAir = CR050[2]; inputEnergy = CR050[3]; inputProtein = CR050[4]; inputLemak = CR050[5]; inputKarbo = CR050[6]; inputSerat = CR050[7]; inputCarbing = CR050[8]; inputType = CR050[9]; inputCode = CR050[10]; break;
          case 13151 : inputName = CR051[1]; inputAir = CR051[2]; inputEnergy = CR051[3]; inputProtein = CR051[4]; inputLemak = CR051[5]; inputKarbo = CR051[6]; inputSerat = CR051[7]; inputCarbing = CR051[8]; inputType = CR051[9]; inputCode = CR051[10]; break;
          case 13152 : inputName = CR052[1]; inputAir = CR052[2]; inputEnergy = CR052[3]; inputProtein = CR052[4]; inputLemak = CR052[5]; inputKarbo = CR052[6]; inputSerat = CR052[7]; inputCarbing = CR052[8]; inputType = CR052[9]; inputCode = CR052[10]; break;
          case 13153 : inputName = CP001[1]; inputAir = CP001[2]; inputEnergy = CP001[3]; inputProtein = CP001[4]; inputLemak = CP001[5]; inputKarbo = CP001[6]; inputSerat = CP001[7]; inputCarbing = CP001[8]; inputType = CP001[9]; inputCode = CP001[10]; break;
          case 13154 : inputName = CP002[1]; inputAir = CP002[2]; inputEnergy = CP002[3]; inputProtein = CP002[4]; inputLemak = CP002[5]; inputKarbo = CP002[6]; inputSerat = CP002[7]; inputCarbing = CP002[8]; inputType = CP002[9]; inputCode = CP002[10]; break;
          case 13155 : inputName = CP003[1]; inputAir = CP003[2]; inputEnergy = CP003[3]; inputProtein = CP003[4]; inputLemak = CP003[5]; inputKarbo = CP003[6]; inputSerat = CP003[7]; inputCarbing = CP003[8]; inputType = CP003[9]; inputCode = CP003[10]; break;
          case 13211 : inputName = CP004[1]; inputAir = CP004[2]; inputEnergy = CP004[3]; inputProtein = CP004[4]; inputLemak = CP004[5]; inputKarbo = CP004[6]; inputSerat = CP004[7]; inputCarbing = CP004[8]; inputType = CP004[9]; inputCode = CP004[10]; break;
          case 13212 : inputName = CP005[1]; inputAir = CP005[2]; inputEnergy = CP005[3]; inputProtein = CP005[4]; inputLemak = CP005[5]; inputKarbo = CP005[6]; inputSerat = CP005[7]; inputCarbing = CP005[8]; inputType = CP005[9]; inputCode = CP005[10]; break;
          case 13213 : inputName = CP006[1]; inputAir = CP006[2]; inputEnergy = CP006[3]; inputProtein = CP006[4]; inputLemak = CP006[5]; inputKarbo = CP006[6]; inputSerat = CP006[7]; inputCarbing = CP006[8]; inputType = CP006[9]; inputCode = CP006[10]; break;
          case 13214 : inputName = CP007[1]; inputAir = CP007[2]; inputEnergy = CP007[3]; inputProtein = CP007[4]; inputLemak = CP007[5]; inputKarbo = CP007[6]; inputSerat = CP007[7]; inputCarbing = CP007[8]; inputType = CP007[9]; inputCode = CP007[10]; break;
          case 13215 : inputName = CP008[1]; inputAir = CP008[2]; inputEnergy = CP008[3]; inputProtein = CP008[4]; inputLemak = CP008[5]; inputKarbo = CP008[6]; inputSerat = CP008[7]; inputCarbing = CP008[8]; inputType = CP008[9]; inputCode = CP008[10]; break;
          case 13221 : inputName = CP009[1]; inputAir = CP009[2]; inputEnergy = CP009[3]; inputProtein = CP009[4]; inputLemak = CP009[5]; inputKarbo = CP009[6]; inputSerat = CP009[7]; inputCarbing = CP009[8]; inputType = CP009[9]; inputCode = CP009[10]; break;
          case 13222 : inputName = CP010[1]; inputAir = CP010[2]; inputEnergy = CP010[3]; inputProtein = CP010[4]; inputLemak = CP010[5]; inputKarbo = CP010[6]; inputSerat = CP010[7]; inputCarbing = CP010[8]; inputType = CP010[9]; inputCode = CP010[10]; break;
          case 13223 : inputName = CP011[1]; inputAir = CP011[2]; inputEnergy = CP011[3]; inputProtein = CP011[4]; inputLemak = CP011[5]; inputKarbo = CP011[6]; inputSerat = CP011[7]; inputCarbing = CP011[8]; inputType = CP011[9]; inputCode = CP011[10]; break;
          case 13224 : inputName = CP012[1]; inputAir = CP012[2]; inputEnergy = CP012[3]; inputProtein = CP012[4]; inputLemak = CP012[5]; inputKarbo = CP012[6]; inputSerat = CP012[7]; inputCarbing = CP012[8]; inputType = CP012[9]; inputCode = CP012[10]; break;
          case 13225 : inputName = CP013[1]; inputAir = CP013[2]; inputEnergy = CP013[3]; inputProtein = CP013[4]; inputLemak = CP013[5]; inputKarbo = CP013[6]; inputSerat = CP013[7]; inputCarbing = CP013[8]; inputType = CP013[9]; inputCode = CP013[10]; break;
          case 13231 : inputName = CP014[1]; inputAir = CP014[2]; inputEnergy = CP014[3]; inputProtein = CP014[4]; inputLemak = CP014[5]; inputKarbo = CP014[6]; inputSerat = CP014[7]; inputCarbing = CP014[8]; inputType = CP014[9]; inputCode = CP014[10]; break;
          case 13232 : inputName = CP015[1]; inputAir = CP015[2]; inputEnergy = CP015[3]; inputProtein = CP015[4]; inputLemak = CP015[5]; inputKarbo = CP015[6]; inputSerat = CP015[7]; inputCarbing = CP015[8]; inputType = CP015[9]; inputCode = CP015[10]; break;
          case 13233 : inputName = CP016[1]; inputAir = CP016[2]; inputEnergy = CP016[3]; inputProtein = CP016[4]; inputLemak = CP016[5]; inputKarbo = CP016[6]; inputSerat = CP016[7]; inputCarbing = CP016[8]; inputType = CP016[9]; inputCode = CP016[10]; break;
          case 13234 : inputName = CP017[1]; inputAir = CP017[2]; inputEnergy = CP017[3]; inputProtein = CP017[4]; inputLemak = CP017[5]; inputKarbo = CP017[6]; inputSerat = CP017[7]; inputCarbing = CP017[8]; inputType = CP017[9]; inputCode = CP017[10]; break;
          case 13235 : inputName = CP018[1]; inputAir = CP018[2]; inputEnergy = CP018[3]; inputProtein = CP018[4]; inputLemak = CP018[5]; inputKarbo = CP018[6]; inputSerat = CP018[7]; inputCarbing = CP018[8]; inputType = CP018[9]; inputCode = CP018[10]; break;
          case 13241 : inputName = CP019[1]; inputAir = CP019[2]; inputEnergy = CP019[3]; inputProtein = CP019[4]; inputLemak = CP019[5]; inputKarbo = CP019[6]; inputSerat = CP019[7]; inputCarbing = CP019[8]; inputType = CP019[9]; inputCode = CP019[10]; break;
          case 13242 : inputName = CP020[1]; inputAir = CP020[2]; inputEnergy = CP020[3]; inputProtein = CP020[4]; inputLemak = CP020[5]; inputKarbo = CP020[6]; inputSerat = CP020[7]; inputCarbing = CP020[8]; inputType = CP020[9]; inputCode = CP020[10]; break;
          case 13243 : inputName = CP021[1]; inputAir = CP021[2]; inputEnergy = CP021[3]; inputProtein = CP021[4]; inputLemak = CP021[5]; inputKarbo = CP021[6]; inputSerat = CP021[7]; inputCarbing = CP021[8]; inputType = CP021[9]; inputCode = CP021[10]; break;
          case 13244 : inputName = CP022[1]; inputAir = CP022[2]; inputEnergy = CP022[3]; inputProtein = CP022[4]; inputLemak = CP022[5]; inputKarbo = CP022[6]; inputSerat = CP022[7]; inputCarbing = CP022[8]; inputType = CP022[9]; inputCode = CP022[10]; break;
          case 13245 : inputName = CP023[1]; inputAir = CP023[2]; inputEnergy = CP023[3]; inputProtein = CP023[4]; inputLemak = CP023[5]; inputKarbo = CP023[6]; inputSerat = CP023[7]; inputCarbing = CP023[8]; inputType = CP023[9]; inputCode = CP023[10]; break;
          case 13251 : inputName = CP024[1]; inputAir = CP024[2]; inputEnergy = CP024[3]; inputProtein = CP024[4]; inputLemak = CP024[5]; inputKarbo = CP024[6]; inputSerat = CP024[7]; inputCarbing = CP024[8]; inputType = CP024[9]; inputCode = CP024[10]; break;
          case 13252 : inputName = CP025[1]; inputAir = CP025[2]; inputEnergy = CP025[3]; inputProtein = CP025[4]; inputLemak = CP025[5]; inputKarbo = CP025[6]; inputSerat = CP025[7]; inputCarbing = CP025[8]; inputType = CP025[9]; inputCode = CP025[10]; break;
          case 13253 : inputName = CP026[1]; inputAir = CP026[2]; inputEnergy = CP026[3]; inputProtein = CP026[4]; inputLemak = CP026[5]; inputKarbo = CP026[6]; inputSerat = CP026[7]; inputCarbing = CP026[8]; inputType = CP026[9]; inputCode = CP026[10]; break;
          case 13254 : inputName = CP027[1]; inputAir = CP027[2]; inputEnergy = CP027[3]; inputProtein = CP027[4]; inputLemak = CP027[5]; inputKarbo = CP027[6]; inputSerat = CP027[7]; inputCarbing = CP027[8]; inputType = CP027[9]; inputCode = CP027[10]; break;
          case 13255 : inputName = CP028[1]; inputAir = CP028[2]; inputEnergy = CP028[3]; inputProtein = CP028[4]; inputLemak = CP028[5]; inputKarbo = CP028[6]; inputSerat = CP028[7]; inputCarbing = CP028[8]; inputType = CP028[9]; inputCode = CP028[10]; break;
          case 13311 : inputName = CP029[1]; inputAir = CP029[2]; inputEnergy = CP029[3]; inputProtein = CP029[4]; inputLemak = CP029[5]; inputKarbo = CP029[6]; inputSerat = CP029[7]; inputCarbing = CP029[8]; inputType = CP029[9]; inputCode = CP029[10]; break;
          case 13312 : inputName = CP030[1]; inputAir = CP030[2]; inputEnergy = CP030[3]; inputProtein = CP030[4]; inputLemak = CP030[5]; inputKarbo = CP030[6]; inputSerat = CP030[7]; inputCarbing = CP030[8]; inputType = CP030[9]; inputCode = CP030[10]; break;
          case 13313 : inputName = CP031[1]; inputAir = CP031[2]; inputEnergy = CP031[3]; inputProtein = CP031[4]; inputLemak = CP031[5]; inputKarbo = CP031[6]; inputSerat = CP031[7]; inputCarbing = CP031[8]; inputType = CP031[9]; inputCode = CP031[10]; break;
          case 13314 : inputName = CP032[1]; inputAir = CP032[2]; inputEnergy = CP032[3]; inputProtein = CP032[4]; inputLemak = CP032[5]; inputKarbo = CP032[6]; inputSerat = CP032[7]; inputCarbing = CP032[8]; inputType = CP032[9]; inputCode = CP032[10]; break;
          case 13315 : inputName = CP033[1]; inputAir = CP033[2]; inputEnergy = CP033[3]; inputProtein = CP033[4]; inputLemak = CP033[5]; inputKarbo = CP033[6]; inputSerat = CP033[7]; inputCarbing = CP033[8]; inputType = CP033[9]; inputCode = CP033[10]; break;
          case 13321 : inputName = CP034[1]; inputAir = CP034[2]; inputEnergy = CP034[3]; inputProtein = CP034[4]; inputLemak = CP034[5]; inputKarbo = CP034[6]; inputSerat = CP034[7]; inputCarbing = CP034[8]; inputType = CP034[9]; inputCode = CP034[10]; break;
          case 13322 : inputName = CP035[1]; inputAir = CP035[2]; inputEnergy = CP035[3]; inputProtein = CP035[4]; inputLemak = CP035[5]; inputKarbo = CP035[6]; inputSerat = CP035[7]; inputCarbing = CP035[8]; inputType = CP035[9]; inputCode = CP035[10]; break;
          case 13323 : inputName = CP036[1]; inputAir = CP036[2]; inputEnergy = CP036[3]; inputProtein = CP036[4]; inputLemak = CP036[5]; inputKarbo = CP036[6]; inputSerat = CP036[7]; inputCarbing = CP036[8]; inputType = CP036[9]; inputCode = CP036[10]; break;
          case 13324 : inputName = CP037[1]; inputAir = CP037[2]; inputEnergy = CP037[3]; inputProtein = CP037[4]; inputLemak = CP037[5]; inputKarbo = CP037[6]; inputSerat = CP037[7]; inputCarbing = CP037[8]; inputType = CP037[9]; inputCode = CP037[10]; break;
          case 13325 : inputName = CP038[1]; inputAir = CP038[2]; inputEnergy = CP038[3]; inputProtein = CP038[4]; inputLemak = CP038[5]; inputKarbo = CP038[6]; inputSerat = CP038[7]; inputCarbing = CP038[8]; inputType = CP038[9]; inputCode = CP038[10]; break;
          case 13331 : inputName = CP039[1]; inputAir = CP039[2]; inputEnergy = CP039[3]; inputProtein = CP039[4]; inputLemak = CP039[5]; inputKarbo = CP039[6]; inputSerat = CP039[7]; inputCarbing = CP039[8]; inputType = CP039[9]; inputCode = CP039[10]; break;
          case 13332 : inputName = CP040[1]; inputAir = CP040[2]; inputEnergy = CP040[3]; inputProtein = CP040[4]; inputLemak = CP040[5]; inputKarbo = CP040[6]; inputSerat = CP040[7]; inputCarbing = CP040[8]; inputType = CP040[9]; inputCode = CP040[10]; break;
          case 13333 : inputName = CP041[1]; inputAir = CP041[2]; inputEnergy = CP041[3]; inputProtein = CP041[4]; inputLemak = CP041[5]; inputKarbo = CP041[6]; inputSerat = CP041[7]; inputCarbing = CP041[8]; inputType = CP041[9]; inputCode = CP041[10]; break;
          case 13334 : inputName = CP042[1]; inputAir = CP042[2]; inputEnergy = CP042[3]; inputProtein = CP042[4]; inputLemak = CP042[5]; inputKarbo = CP042[6]; inputSerat = CP042[7]; inputCarbing = CP042[8]; inputType = CP042[9]; inputCode = CP042[10]; break;
          case 13335 : inputName = CP043[1]; inputAir = CP043[2]; inputEnergy = CP043[3]; inputProtein = CP043[4]; inputLemak = CP043[5]; inputKarbo = CP043[6]; inputSerat = CP043[7]; inputCarbing = CP043[8]; inputType = CP043[9]; inputCode = CP043[10]; break;
          case 13341 : inputName = CP044[1]; inputAir = CP044[2]; inputEnergy = CP044[3]; inputProtein = CP044[4]; inputLemak = CP044[5]; inputKarbo = CP044[6]; inputSerat = CP044[7]; inputCarbing = CP044[8]; inputType = CP044[9]; inputCode = CP044[10]; break;
          case 13342 : inputName = CP045[1]; inputAir = CP045[2]; inputEnergy = CP045[3]; inputProtein = CP045[4]; inputLemak = CP045[5]; inputKarbo = CP045[6]; inputSerat = CP045[7]; inputCarbing = CP045[8]; inputType = CP045[9]; inputCode = CP045[10]; break;
          case 13343 : inputName = CP046[1]; inputAir = CP046[2]; inputEnergy = CP046[3]; inputProtein = CP046[4]; inputLemak = CP046[5]; inputKarbo = CP046[6]; inputSerat = CP046[7]; inputCarbing = CP046[8]; inputType = CP046[9]; inputCode = CP046[10]; break;
          case 13344 : inputName = CP047[1]; inputAir = CP047[2]; inputEnergy = CP047[3]; inputProtein = CP047[4]; inputLemak = CP047[5]; inputKarbo = CP047[6]; inputSerat = CP047[7]; inputCarbing = CP047[8]; inputType = CP047[9]; inputCode = CP047[10]; break;
          case 13345 : inputName = CP048[1]; inputAir = CP048[2]; inputEnergy = CP048[3]; inputProtein = CP048[4]; inputLemak = CP048[5]; inputKarbo = CP048[6]; inputSerat = CP048[7]; inputCarbing = CP048[8]; inputType = CP048[9]; inputCode = CP048[10]; break;
          case 13351 : inputName = CP049[1]; inputAir = CP049[2]; inputEnergy = CP049[3]; inputProtein = CP049[4]; inputLemak = CP049[5]; inputKarbo = CP049[6]; inputSerat = CP049[7]; inputCarbing = CP049[8]; inputType = CP049[9]; inputCode = CP049[10]; break;
          case 13352 : inputName = CP050[1]; inputAir = CP050[2]; inputEnergy = CP050[3]; inputProtein = CP050[4]; inputLemak = CP050[5]; inputKarbo = CP050[6]; inputSerat = CP050[7]; inputCarbing = CP050[8]; inputType = CP050[9]; inputCode = CP050[10]; break;
          case 13353 : inputName = CP051[1]; inputAir = CP051[2]; inputEnergy = CP051[3]; inputProtein = CP051[4]; inputLemak = CP051[5]; inputKarbo = CP051[6]; inputSerat = CP051[7]; inputCarbing = CP051[8]; inputType = CP051[9]; inputCode = CP051[10]; break;
          case 13354 : inputName = CP052[1]; inputAir = CP052[2]; inputEnergy = CP052[3]; inputProtein = CP052[4]; inputLemak = CP052[5]; inputKarbo = CP052[6]; inputSerat = CP052[7]; inputCarbing = CP052[8]; inputType = CP052[9]; inputCode = CP052[10]; break;
          case 13355 : inputName = CP053[1]; inputAir = CP053[2]; inputEnergy = CP053[3]; inputProtein = CP053[4]; inputLemak = CP053[5]; inputKarbo = CP053[6]; inputSerat = CP053[7]; inputCarbing = CP053[8]; inputType = CP053[9]; inputCode = CP053[10]; break;
          case 13411 : inputName = CP054[1]; inputAir = CP054[2]; inputEnergy = CP054[3]; inputProtein = CP054[4]; inputLemak = CP054[5]; inputKarbo = CP054[6]; inputSerat = CP054[7]; inputCarbing = CP054[8]; inputType = CP054[9]; inputCode = CP054[10]; break;
          case 13412 : inputName = CP055[1]; inputAir = CP055[2]; inputEnergy = CP055[3]; inputProtein = CP055[4]; inputLemak = CP055[5]; inputKarbo = CP055[6]; inputSerat = CP055[7]; inputCarbing = CP055[8]; inputType = CP055[9]; inputCode = CP055[10]; break;
          case 13413 : inputName = CP056[1]; inputAir = CP056[2]; inputEnergy = CP056[3]; inputProtein = CP056[4]; inputLemak = CP056[5]; inputKarbo = CP056[6]; inputSerat = CP056[7]; inputCarbing = CP056[8]; inputType = CP056[9]; inputCode = CP056[10]; break;
          case 13414 : inputName = CP057[1]; inputAir = CP057[2]; inputEnergy = CP057[3]; inputProtein = CP057[4]; inputLemak = CP057[5]; inputKarbo = CP057[6]; inputSerat = CP057[7]; inputCarbing = CP057[8]; inputType = CP057[9]; inputCode = CP057[10]; break;
          case 13415 : inputName = CP058[1]; inputAir = CP058[2]; inputEnergy = CP058[3]; inputProtein = CP058[4]; inputLemak = CP058[5]; inputKarbo = CP058[6]; inputSerat = CP058[7]; inputCarbing = CP058[8]; inputType = CP058[9]; inputCode = CP058[10]; break;
          case 13421 : inputName = CP059[1]; inputAir = CP059[2]; inputEnergy = CP059[3]; inputProtein = CP059[4]; inputLemak = CP059[5]; inputKarbo = CP059[6]; inputSerat = CP059[7]; inputCarbing = CP059[8]; inputType = CP059[9]; inputCode = CP059[10]; break;
          case 13422 : inputName = CP060[1]; inputAir = CP060[2]; inputEnergy = CP060[3]; inputProtein = CP060[4]; inputLemak = CP060[5]; inputKarbo = CP060[6]; inputSerat = CP060[7]; inputCarbing = CP060[8]; inputType = CP060[9]; inputCode = CP060[10]; break;
          case 13423 : inputName = CP061[1]; inputAir = CP061[2]; inputEnergy = CP061[3]; inputProtein = CP061[4]; inputLemak = CP061[5]; inputKarbo = CP061[6]; inputSerat = CP061[7]; inputCarbing = CP061[8]; inputType = CP061[9]; inputCode = CP061[10]; break;
          case 13424 : inputName = CP062[1]; inputAir = CP062[2]; inputEnergy = CP062[3]; inputProtein = CP062[4]; inputLemak = CP062[5]; inputKarbo = CP062[6]; inputSerat = CP062[7]; inputCarbing = CP062[8]; inputType = CP062[9]; inputCode = CP062[10]; break;
          case 13425 : inputName = CP063[1]; inputAir = CP063[2]; inputEnergy = CP063[3]; inputProtein = CP063[4]; inputLemak = CP063[5]; inputKarbo = CP063[6]; inputSerat = CP063[7]; inputCarbing = CP063[8]; inputType = CP063[9]; inputCode = CP063[10]; break;
          case 13431 : inputName = CP064[1]; inputAir = CP064[2]; inputEnergy = CP064[3]; inputProtein = CP064[4]; inputLemak = CP064[5]; inputKarbo = CP064[6]; inputSerat = CP064[7]; inputCarbing = CP064[8]; inputType = CP064[9]; inputCode = CP064[10]; break;
          case 13432 : inputName = CP065[1]; inputAir = CP065[2]; inputEnergy = CP065[3]; inputProtein = CP065[4]; inputLemak = CP065[5]; inputKarbo = CP065[6]; inputSerat = CP065[7]; inputCarbing = CP065[8]; inputType = CP065[9]; inputCode = CP065[10]; break;
          case 13433 : inputName = CP066[1]; inputAir = CP066[2]; inputEnergy = CP066[3]; inputProtein = CP066[4]; inputLemak = CP066[5]; inputKarbo = CP066[6]; inputSerat = CP066[7]; inputCarbing = CP066[8]; inputType = CP066[9]; inputCode = CP066[10]; break;
          case 13434 : inputName = CP067[1]; inputAir = CP067[2]; inputEnergy = CP067[3]; inputProtein = CP067[4]; inputLemak = CP067[5]; inputKarbo = CP067[6]; inputSerat = CP067[7]; inputCarbing = CP067[8]; inputType = CP067[9]; inputCode = CP067[10]; break;
          case 13435 : inputName = CP068[1]; inputAir = CP068[2]; inputEnergy = CP068[3]; inputProtein = CP068[4]; inputLemak = CP068[5]; inputKarbo = CP068[6]; inputSerat = CP068[7]; inputCarbing = CP068[8]; inputType = CP068[9]; inputCode = CP068[10]; break;
          case 13441 : inputName = CP069[1]; inputAir = CP069[2]; inputEnergy = CP069[3]; inputProtein = CP069[4]; inputLemak = CP069[5]; inputKarbo = CP069[6]; inputSerat = CP069[7]; inputCarbing = CP069[8]; inputType = CP069[9]; inputCode = CP069[10]; break;
          case 13442 : inputName = CP070[1]; inputAir = CP070[2]; inputEnergy = CP070[3]; inputProtein = CP070[4]; inputLemak = CP070[5]; inputKarbo = CP070[6]; inputSerat = CP070[7]; inputCarbing = CP070[8]; inputType = CP070[9]; inputCode = CP070[10]; break;
          case 13443 : inputName = CP071[1]; inputAir = CP071[2]; inputEnergy = CP071[3]; inputProtein = CP071[4]; inputLemak = CP071[5]; inputKarbo = CP071[6]; inputSerat = CP071[7]; inputCarbing = CP071[8]; inputType = CP071[9]; inputCode = CP071[10]; break;
          case 13444 : inputName = CP072[1]; inputAir = CP072[2]; inputEnergy = CP072[3]; inputProtein = CP072[4]; inputLemak = CP072[5]; inputKarbo = CP072[6]; inputSerat = CP072[7]; inputCarbing = CP072[8]; inputType = CP072[9]; inputCode = CP072[10]; break;
          case 13445 : inputName = CP073[1]; inputAir = CP073[2]; inputEnergy = CP073[3]; inputProtein = CP073[4]; inputLemak = CP073[5]; inputKarbo = CP073[6]; inputSerat = CP073[7]; inputCarbing = CP073[8]; inputType = CP073[9]; inputCode = CP073[10]; break;
          case 13451 : inputName = CP074[1]; inputAir = CP074[2]; inputEnergy = CP074[3]; inputProtein = CP074[4]; inputLemak = CP074[5]; inputKarbo = CP074[6]; inputSerat = CP074[7]; inputCarbing = CP074[8]; inputType = CP074[9]; inputCode = CP074[10]; break;
          case 13452 : inputName = CP075[1]; inputAir = CP075[2]; inputEnergy = CP075[3]; inputProtein = CP075[4]; inputLemak = CP075[5]; inputKarbo = CP075[6]; inputSerat = CP075[7]; inputCarbing = CP075[8]; inputType = CP075[9]; inputCode = CP075[10]; break;
          case 13453 : inputName = CP076[1]; inputAir = CP076[2]; inputEnergy = CP076[3]; inputProtein = CP076[4]; inputLemak = CP076[5]; inputKarbo = CP076[6]; inputSerat = CP076[7]; inputCarbing = CP076[8]; inputType = CP076[9]; inputCode = CP076[10]; break;
          case 13454 : inputName = CP077[1]; inputAir = CP077[2]; inputEnergy = CP077[3]; inputProtein = CP077[4]; inputLemak = CP077[5]; inputKarbo = CP077[6]; inputSerat = CP077[7]; inputCarbing = CP077[8]; inputType = CP077[9]; inputCode = CP077[10]; break;
          case 13455 : inputName = CP078[1]; inputAir = CP078[2]; inputEnergy = CP078[3]; inputProtein = CP078[4]; inputLemak = CP078[5]; inputKarbo = CP078[6]; inputSerat = CP078[7]; inputCarbing = CP078[8]; inputType = CP078[9]; inputCode = CP078[10]; break;
          case 13511 : inputName = CP079[1]; inputAir = CP079[2]; inputEnergy = CP079[3]; inputProtein = CP079[4]; inputLemak = CP079[5]; inputKarbo = CP079[6]; inputSerat = CP079[7]; inputCarbing = CP079[8]; inputType = CP079[9]; inputCode = CP079[10]; break;
          case 13512 : inputName = CP080[1]; inputAir = CP080[2]; inputEnergy = CP080[3]; inputProtein = CP080[4]; inputLemak = CP080[5]; inputKarbo = CP080[6]; inputSerat = CP080[7]; inputCarbing = CP080[8]; inputType = CP080[9]; inputCode = CP080[10]; break;
          case 13513 : inputName = CP081[1]; inputAir = CP081[2]; inputEnergy = CP081[3]; inputProtein = CP081[4]; inputLemak = CP081[5]; inputKarbo = CP081[6]; inputSerat = CP081[7]; inputCarbing = CP081[8]; inputType = CP081[9]; inputCode = CP081[10]; break;
          case 13514 : inputName = CP082[1]; inputAir = CP082[2]; inputEnergy = CP082[3]; inputProtein = CP082[4]; inputLemak = CP082[5]; inputKarbo = CP082[6]; inputSerat = CP082[7]; inputCarbing = CP082[8]; inputType = CP082[9]; inputCode = CP082[10]; break;
          case 13515 : inputName = CP083[1]; inputAir = CP083[2]; inputEnergy = CP083[3]; inputProtein = CP083[4]; inputLemak = CP083[5]; inputKarbo = CP083[6]; inputSerat = CP083[7]; inputCarbing = CP083[8]; inputType = CP083[9]; inputCode = CP083[10]; break;
          case 13521 : inputName = CP084[1]; inputAir = CP084[2]; inputEnergy = CP084[3]; inputProtein = CP084[4]; inputLemak = CP084[5]; inputKarbo = CP084[6]; inputSerat = CP084[7]; inputCarbing = CP084[8]; inputType = CP084[9]; inputCode = CP084[10]; break;
          case 13522 : inputName = CP085[1]; inputAir = CP085[2]; inputEnergy = CP085[3]; inputProtein = CP085[4]; inputLemak = CP085[5]; inputKarbo = CP085[6]; inputSerat = CP085[7]; inputCarbing = CP085[8]; inputType = CP085[9]; inputCode = CP085[10]; break;
          case 13523 : inputName = CP086[1]; inputAir = CP086[2]; inputEnergy = CP086[3]; inputProtein = CP086[4]; inputLemak = CP086[5]; inputKarbo = CP086[6]; inputSerat = CP086[7]; inputCarbing = CP086[8]; inputType = CP086[9]; inputCode = CP086[10]; break;
          case 151 : inputName = DR001[1]; inputAir = DR001[2]; inputEnergy = DR001[3]; inputProtein = DR001[4]; inputLemak = DR001[5]; inputKarbo = DR001[6]; inputSerat = DR001[7]; inputCarbing = DR001[8]; inputType = DR001[9]; inputCode = DR001[10]; break;
          case 152 : inputName = DR002[1]; inputAir = DR002[2]; inputEnergy = DR002[3]; inputProtein = DR002[4]; inputLemak = DR002[5]; inputKarbo = DR002[6]; inputSerat = DR002[7]; inputCarbing = DR002[8]; inputType = DR002[9]; inputCode = DR002[10]; break;
          case 153 : inputName = DR003[1]; inputAir = DR003[2]; inputEnergy = DR003[3]; inputProtein = DR003[4]; inputLemak = DR003[5]; inputKarbo = DR003[6]; inputSerat = DR003[7]; inputCarbing = DR003[8]; inputType = DR003[9]; inputCode = DR003[10]; break;
          case 154 : inputName = DR004[1]; inputAir = DR004[2]; inputEnergy = DR004[3]; inputProtein = DR004[4]; inputLemak = DR004[5]; inputKarbo = DR004[6]; inputSerat = DR004[7]; inputCarbing = DR004[8]; inputType = DR004[9]; inputCode = DR004[10]; break;
          case 155 : inputName = DR005[1]; inputAir = DR005[2]; inputEnergy = DR005[3]; inputProtein = DR005[4]; inputLemak = DR005[5]; inputKarbo = DR005[6]; inputSerat = DR005[7]; inputCarbing = DR005[8]; inputType = DR005[9]; inputCode = DR005[10]; break;
          case 1511 : inputName = DR006[1]; inputAir = DR006[2]; inputEnergy = DR006[3]; inputProtein = DR006[4]; inputLemak = DR006[5]; inputKarbo = DR006[6]; inputSerat = DR006[7]; inputCarbing = DR006[8]; inputType = DR006[9]; inputCode = DR006[10]; break;
          case 1512 : inputName = DR007[1]; inputAir = DR007[2]; inputEnergy = DR007[3]; inputProtein = DR007[4]; inputLemak = DR007[5]; inputKarbo = DR007[6]; inputSerat = DR007[7]; inputCarbing = DR007[8]; inputType = DR007[9]; inputCode = DR007[10]; break;
          case 1513 : inputName = DR008[1]; inputAir = DR008[2]; inputEnergy = DR008[3]; inputProtein = DR008[4]; inputLemak = DR008[5]; inputKarbo = DR008[6]; inputSerat = DR008[7]; inputCarbing = DR008[8]; inputType = DR008[9]; inputCode = DR008[10]; break;
          case 1514 : inputName = DR009[1]; inputAir = DR009[2]; inputEnergy = DR009[3]; inputProtein = DR009[4]; inputLemak = DR009[5]; inputKarbo = DR009[6]; inputSerat = DR009[7]; inputCarbing = DR009[8]; inputType = DR009[9]; inputCode = DR009[10]; break;
          case 1515 : inputName = DR010[1]; inputAir = DR010[2]; inputEnergy = DR010[3]; inputProtein = DR010[4]; inputLemak = DR010[5]; inputKarbo = DR010[6]; inputSerat = DR010[7]; inputCarbing = DR010[8]; inputType = DR010[9]; inputCode = DR010[10]; break;
          case 1521 : inputName = DR011[1]; inputAir = DR011[2]; inputEnergy = DR011[3]; inputProtein = DR011[4]; inputLemak = DR011[5]; inputKarbo = DR011[6]; inputSerat = DR011[7]; inputCarbing = DR011[8]; inputType = DR011[9]; inputCode = DR011[10]; break;
          case 1522 : inputName = DR012[1]; inputAir = DR012[2]; inputEnergy = DR012[3]; inputProtein = DR012[4]; inputLemak = DR012[5]; inputKarbo = DR012[6]; inputSerat = DR012[7]; inputCarbing = DR012[8]; inputType = DR012[9]; inputCode = DR012[10]; break;
          case 1523 : inputName = DR013[1]; inputAir = DR013[2]; inputEnergy = DR013[3]; inputProtein = DR013[4]; inputLemak = DR013[5]; inputKarbo = DR013[6]; inputSerat = DR013[7]; inputCarbing = DR013[8]; inputType = DR013[9]; inputCode = DR013[10]; break;
          case 1524 : inputName = DR014[1]; inputAir = DR014[2]; inputEnergy = DR014[3]; inputProtein = DR014[4]; inputLemak = DR014[5]; inputKarbo = DR014[6]; inputSerat = DR014[7]; inputCarbing = DR014[8]; inputType = DR014[9]; inputCode = DR014[10]; break;
          case 1525 : inputName = DR015[1]; inputAir = DR015[2]; inputEnergy = DR015[3]; inputProtein = DR015[4]; inputLemak = DR015[5]; inputKarbo = DR015[6]; inputSerat = DR015[7]; inputCarbing = DR015[8]; inputType = DR015[9]; inputCode = DR015[10]; break;
          case 1531 : inputName = DR016[1]; inputAir = DR016[2]; inputEnergy = DR016[3]; inputProtein = DR016[4]; inputLemak = DR016[5]; inputKarbo = DR016[6]; inputSerat = DR016[7]; inputCarbing = DR016[8]; inputType = DR016[9]; inputCode = DR016[10]; break;
          case 1532 : inputName = DR017[1]; inputAir = DR017[2]; inputEnergy = DR017[3]; inputProtein = DR017[4]; inputLemak = DR017[5]; inputKarbo = DR017[6]; inputSerat = DR017[7]; inputCarbing = DR017[8]; inputType = DR017[9]; inputCode = DR017[10]; break;
          case 1533 : inputName = DR018[1]; inputAir = DR018[2]; inputEnergy = DR018[3]; inputProtein = DR018[4]; inputLemak = DR018[5]; inputKarbo = DR018[6]; inputSerat = DR018[7]; inputCarbing = DR018[8]; inputType = DR018[9]; inputCode = DR018[10]; break;
          case 1534 : inputName = DR019[1]; inputAir = DR019[2]; inputEnergy = DR019[3]; inputProtein = DR019[4]; inputLemak = DR019[5]; inputKarbo = DR019[6]; inputSerat = DR019[7]; inputCarbing = DR019[8]; inputType = DR019[9]; inputCode = DR019[10]; break;
          case 1535 : inputName = DR020[1]; inputAir = DR020[2]; inputEnergy = DR020[3]; inputProtein = DR020[4]; inputLemak = DR020[5]; inputKarbo = DR020[6]; inputSerat = DR020[7]; inputCarbing = DR020[8]; inputType = DR020[9]; inputCode = DR020[10]; break;
          case 1541 : inputName = DR021[1]; inputAir = DR021[2]; inputEnergy = DR021[3]; inputProtein = DR021[4]; inputLemak = DR021[5]; inputKarbo = DR021[6]; inputSerat = DR021[7]; inputCarbing = DR021[8]; inputType = DR021[9]; inputCode = DR021[10]; break;
          case 1542 : inputName = DR022[1]; inputAir = DR022[2]; inputEnergy = DR022[3]; inputProtein = DR022[4]; inputLemak = DR022[5]; inputKarbo = DR022[6]; inputSerat = DR022[7]; inputCarbing = DR022[8]; inputType = DR022[9]; inputCode = DR022[10]; break;
          case 1543 : inputName = DR023[1]; inputAir = DR023[2]; inputEnergy = DR023[3]; inputProtein = DR023[4]; inputLemak = DR023[5]; inputKarbo = DR023[6]; inputSerat = DR023[7]; inputCarbing = DR023[8]; inputType = DR023[9]; inputCode = DR023[10]; break;
          case 1544 : inputName = DR024[1]; inputAir = DR024[2]; inputEnergy = DR024[3]; inputProtein = DR024[4]; inputLemak = DR024[5]; inputKarbo = DR024[6]; inputSerat = DR024[7]; inputCarbing = DR024[8]; inputType = DR024[9]; inputCode = DR024[10]; break;
          case 1545 : inputName = DR025[1]; inputAir = DR025[2]; inputEnergy = DR025[3]; inputProtein = DR025[4]; inputLemak = DR025[5]; inputKarbo = DR025[6]; inputSerat = DR025[7]; inputCarbing = DR025[8]; inputType = DR025[9]; inputCode = DR025[10]; break;
          case 1551 : inputName = DR026[1]; inputAir = DR026[2]; inputEnergy = DR026[3]; inputProtein = DR026[4]; inputLemak = DR026[5]; inputKarbo = DR026[6]; inputSerat = DR026[7]; inputCarbing = DR026[8]; inputType = DR026[9]; inputCode = DR026[10]; break;
          case 1552 : inputName = DR027[1]; inputAir = DR027[2]; inputEnergy = DR027[3]; inputProtein = DR027[4]; inputLemak = DR027[5]; inputKarbo = DR027[6]; inputSerat = DR027[7]; inputCarbing = DR027[8]; inputType = DR027[9]; inputCode = DR027[10]; break;
          case 1553 : inputName = DR028[1]; inputAir = DR028[2]; inputEnergy = DR028[3]; inputProtein = DR028[4]; inputLemak = DR028[5]; inputKarbo = DR028[6]; inputSerat = DR028[7]; inputCarbing = DR028[8]; inputType = DR028[9]; inputCode = DR028[10]; break;
          case 1554 : inputName = DR029[1]; inputAir = DR029[2]; inputEnergy = DR029[3]; inputProtein = DR029[4]; inputLemak = DR029[5]; inputKarbo = DR029[6]; inputSerat = DR029[7]; inputCarbing = DR029[8]; inputType = DR029[9]; inputCode = DR029[10]; break;
          case 1555 : inputName = DR030[1]; inputAir = DR030[2]; inputEnergy = DR030[3]; inputProtein = DR030[4]; inputLemak = DR030[5]; inputKarbo = DR030[6]; inputSerat = DR030[7]; inputCarbing = DR030[8]; inputType = DR030[9]; inputCode = DR030[10]; break;
          case 15111 : inputName = DR031[1]; inputAir = DR031[2]; inputEnergy = DR031[3]; inputProtein = DR031[4]; inputLemak = DR031[5]; inputKarbo = DR031[6]; inputSerat = DR031[7]; inputCarbing = DR031[8]; inputType = DR031[9]; inputCode = DR031[10]; break;
          case 15112 : inputName = DR032[1]; inputAir = DR032[2]; inputEnergy = DR032[3]; inputProtein = DR032[4]; inputLemak = DR032[5]; inputKarbo = DR032[6]; inputSerat = DR032[7]; inputCarbing = DR032[8]; inputType = DR032[9]; inputCode = DR032[10]; break;
          case 15113 : inputName = DR033[1]; inputAir = DR033[2]; inputEnergy = DR033[3]; inputProtein = DR033[4]; inputLemak = DR033[5]; inputKarbo = DR033[6]; inputSerat = DR033[7]; inputCarbing = DR033[8]; inputType = DR033[9]; inputCode = DR033[10]; break;
          case 15114 : inputName = DR034[1]; inputAir = DR034[2]; inputEnergy = DR034[3]; inputProtein = DR034[4]; inputLemak = DR034[5]; inputKarbo = DR034[6]; inputSerat = DR034[7]; inputCarbing = DR034[8]; inputType = DR034[9]; inputCode = DR034[10]; break;
          case 15115 : inputName = DR035[1]; inputAir = DR035[2]; inputEnergy = DR035[3]; inputProtein = DR035[4]; inputLemak = DR035[5]; inputKarbo = DR035[6]; inputSerat = DR035[7]; inputCarbing = DR035[8]; inputType = DR035[9]; inputCode = DR035[10]; break;
          case 15121 : inputName = DR036[1]; inputAir = DR036[2]; inputEnergy = DR036[3]; inputProtein = DR036[4]; inputLemak = DR036[5]; inputKarbo = DR036[6]; inputSerat = DR036[7]; inputCarbing = DR036[8]; inputType = DR036[9]; inputCode = DR036[10]; break;
          case 15122 : inputName = DR037[1]; inputAir = DR037[2]; inputEnergy = DR037[3]; inputProtein = DR037[4]; inputLemak = DR037[5]; inputKarbo = DR037[6]; inputSerat = DR037[7]; inputCarbing = DR037[8]; inputType = DR037[9]; inputCode = DR037[10]; break;
          case 15123 : inputName = DR038[1]; inputAir = DR038[2]; inputEnergy = DR038[3]; inputProtein = DR038[4]; inputLemak = DR038[5]; inputKarbo = DR038[6]; inputSerat = DR038[7]; inputCarbing = DR038[8]; inputType = DR038[9]; inputCode = DR038[10]; break;
          case 15124 : inputName = DR039[1]; inputAir = DR039[2]; inputEnergy = DR039[3]; inputProtein = DR039[4]; inputLemak = DR039[5]; inputKarbo = DR039[6]; inputSerat = DR039[7]; inputCarbing = DR039[8]; inputType = DR039[9]; inputCode = DR039[10]; break;
          case 15125 : inputName = DR040[1]; inputAir = DR040[2]; inputEnergy = DR040[3]; inputProtein = DR040[4]; inputLemak = DR040[5]; inputKarbo = DR040[6]; inputSerat = DR040[7]; inputCarbing = DR040[8]; inputType = DR040[9]; inputCode = DR040[10]; break;
          case 15131 : inputName = DR041[1]; inputAir = DR041[2]; inputEnergy = DR041[3]; inputProtein = DR041[4]; inputLemak = DR041[5]; inputKarbo = DR041[6]; inputSerat = DR041[7]; inputCarbing = DR041[8]; inputType = DR041[9]; inputCode = DR041[10]; break;
          case 15132 : inputName = DR042[1]; inputAir = DR042[2]; inputEnergy = DR042[3]; inputProtein = DR042[4]; inputLemak = DR042[5]; inputKarbo = DR042[6]; inputSerat = DR042[7]; inputCarbing = DR042[8]; inputType = DR042[9]; inputCode = DR042[10]; break;
          case 15133 : inputName = DR043[1]; inputAir = DR043[2]; inputEnergy = DR043[3]; inputProtein = DR043[4]; inputLemak = DR043[5]; inputKarbo = DR043[6]; inputSerat = DR043[7]; inputCarbing = DR043[8]; inputType = DR043[9]; inputCode = DR043[10]; break;
          case 15134 : inputName = DR044[1]; inputAir = DR044[2]; inputEnergy = DR044[3]; inputProtein = DR044[4]; inputLemak = DR044[5]; inputKarbo = DR044[6]; inputSerat = DR044[7]; inputCarbing = DR044[8]; inputType = DR044[9]; inputCode = DR044[10]; break;
          case 15135 : inputName = DR045[1]; inputAir = DR045[2]; inputEnergy = DR045[3]; inputProtein = DR045[4]; inputLemak = DR045[5]; inputKarbo = DR045[6]; inputSerat = DR045[7]; inputCarbing = DR045[8]; inputType = DR045[9]; inputCode = DR045[10]; break;
          case 15141 : inputName = DR046[1]; inputAir = DR046[2]; inputEnergy = DR046[3]; inputProtein = DR046[4]; inputLemak = DR046[5]; inputKarbo = DR046[6]; inputSerat = DR046[7]; inputCarbing = DR046[8]; inputType = DR046[9]; inputCode = DR046[10]; break;
          case 15142 : inputName = DR047[1]; inputAir = DR047[2]; inputEnergy = DR047[3]; inputProtein = DR047[4]; inputLemak = DR047[5]; inputKarbo = DR047[6]; inputSerat = DR047[7]; inputCarbing = DR047[8]; inputType = DR047[9]; inputCode = DR047[10]; break;
          case 15143 : inputName = DR048[1]; inputAir = DR048[2]; inputEnergy = DR048[3]; inputProtein = DR048[4]; inputLemak = DR048[5]; inputKarbo = DR048[6]; inputSerat = DR048[7]; inputCarbing = DR048[8]; inputType = DR048[9]; inputCode = DR048[10]; break;
          case 15144 : inputName = DR049[1]; inputAir = DR049[2]; inputEnergy = DR049[3]; inputProtein = DR049[4]; inputLemak = DR049[5]; inputKarbo = DR049[6]; inputSerat = DR049[7]; inputCarbing = DR049[8]; inputType = DR049[9]; inputCode = DR049[10]; break;
          case 15145 : inputName = DR050[1]; inputAir = DR050[2]; inputEnergy = DR050[3]; inputProtein = DR050[4]; inputLemak = DR050[5]; inputKarbo = DR050[6]; inputSerat = DR050[7]; inputCarbing = DR050[8]; inputType = DR050[9]; inputCode = DR050[10]; break;
          case 15151 : inputName = DR051[1]; inputAir = DR051[2]; inputEnergy = DR051[3]; inputProtein = DR051[4]; inputLemak = DR051[5]; inputKarbo = DR051[6]; inputSerat = DR051[7]; inputCarbing = DR051[8]; inputType = DR051[9]; inputCode = DR051[10]; break;
          case 15152 : inputName = DR052[1]; inputAir = DR052[2]; inputEnergy = DR052[3]; inputProtein = DR052[4]; inputLemak = DR052[5]; inputKarbo = DR052[6]; inputSerat = DR052[7]; inputCarbing = DR052[8]; inputType = DR052[9]; inputCode = DR052[10]; break;
          case 15153 : inputName = DR053[1]; inputAir = DR053[2]; inputEnergy = DR053[3]; inputProtein = DR053[4]; inputLemak = DR053[5]; inputKarbo = DR053[6]; inputSerat = DR053[7]; inputCarbing = DR053[8]; inputType = DR053[9]; inputCode = DR053[10]; break;
          case 15154 : inputName = DR054[1]; inputAir = DR054[2]; inputEnergy = DR054[3]; inputProtein = DR054[4]; inputLemak = DR054[5]; inputKarbo = DR054[6]; inputSerat = DR054[7]; inputCarbing = DR054[8]; inputType = DR054[9]; inputCode = DR054[10]; break;
          case 15155 : inputName = DR055[1]; inputAir = DR055[2]; inputEnergy = DR055[3]; inputProtein = DR055[4]; inputLemak = DR055[5]; inputKarbo = DR055[6]; inputSerat = DR055[7]; inputCarbing = DR055[8]; inputType = DR055[9]; inputCode = DR055[10]; break;
          case 15211 : inputName = DR056[1]; inputAir = DR056[2]; inputEnergy = DR056[3]; inputProtein = DR056[4]; inputLemak = DR056[5]; inputKarbo = DR056[6]; inputSerat = DR056[7]; inputCarbing = DR056[8]; inputType = DR056[9]; inputCode = DR056[10]; break;
          case 15212 : inputName = DR057[1]; inputAir = DR057[2]; inputEnergy = DR057[3]; inputProtein = DR057[4]; inputLemak = DR057[5]; inputKarbo = DR057[6]; inputSerat = DR057[7]; inputCarbing = DR057[8]; inputType = DR057[9]; inputCode = DR057[10]; break;
          case 15213 : inputName = DR058[1]; inputAir = DR058[2]; inputEnergy = DR058[3]; inputProtein = DR058[4]; inputLemak = DR058[5]; inputKarbo = DR058[6]; inputSerat = DR058[7]; inputCarbing = DR058[8]; inputType = DR058[9]; inputCode = DR058[10]; break;
          case 15214 : inputName = DR059[1]; inputAir = DR059[2]; inputEnergy = DR059[3]; inputProtein = DR059[4]; inputLemak = DR059[5]; inputKarbo = DR059[6]; inputSerat = DR059[7]; inputCarbing = DR059[8]; inputType = DR059[9]; inputCode = DR059[10]; break;
          case 15215 : inputName = DR060[1]; inputAir = DR060[2]; inputEnergy = DR060[3]; inputProtein = DR060[4]; inputLemak = DR060[5]; inputKarbo = DR060[6]; inputSerat = DR060[7]; inputCarbing = DR060[8]; inputType = DR060[9]; inputCode = DR060[10]; break;
          case 15221 : inputName = DR061[1]; inputAir = DR061[2]; inputEnergy = DR061[3]; inputProtein = DR061[4]; inputLemak = DR061[5]; inputKarbo = DR061[6]; inputSerat = DR061[7]; inputCarbing = DR061[8]; inputType = DR061[9]; inputCode = DR061[10]; break;
          case 15222 : inputName = DR062[1]; inputAir = DR062[2]; inputEnergy = DR062[3]; inputProtein = DR062[4]; inputLemak = DR062[5]; inputKarbo = DR062[6]; inputSerat = DR062[7]; inputCarbing = DR062[8]; inputType = DR062[9]; inputCode = DR062[10]; break;
          case 15223 : inputName = DR063[1]; inputAir = DR063[2]; inputEnergy = DR063[3]; inputProtein = DR063[4]; inputLemak = DR063[5]; inputKarbo = DR063[6]; inputSerat = DR063[7]; inputCarbing = DR063[8]; inputType = DR063[9]; inputCode = DR063[10]; break;
          case 15224 : inputName = DR064[1]; inputAir = DR064[2]; inputEnergy = DR064[3]; inputProtein = DR064[4]; inputLemak = DR064[5]; inputKarbo = DR064[6]; inputSerat = DR064[7]; inputCarbing = DR064[8]; inputType = DR064[9]; inputCode = DR064[10]; break;
          case 15225 : inputName = DR065[1]; inputAir = DR065[2]; inputEnergy = DR065[3]; inputProtein = DR065[4]; inputLemak = DR065[5]; inputKarbo = DR065[6]; inputSerat = DR065[7]; inputCarbing = DR065[8]; inputType = DR065[9]; inputCode = DR065[10]; break;
          case 15231 : inputName = DR066[1]; inputAir = DR066[2]; inputEnergy = DR066[3]; inputProtein = DR066[4]; inputLemak = DR066[5]; inputKarbo = DR066[6]; inputSerat = DR066[7]; inputCarbing = DR066[8]; inputType = DR066[9]; inputCode = DR066[10]; break;
          case 15232 : inputName = DR067[1]; inputAir = DR067[2]; inputEnergy = DR067[3]; inputProtein = DR067[4]; inputLemak = DR067[5]; inputKarbo = DR067[6]; inputSerat = DR067[7]; inputCarbing = DR067[8]; inputType = DR067[9]; inputCode = DR067[10]; break;
          case 15233 : inputName = DR068[1]; inputAir = DR068[2]; inputEnergy = DR068[3]; inputProtein = DR068[4]; inputLemak = DR068[5]; inputKarbo = DR068[6]; inputSerat = DR068[7]; inputCarbing = DR068[8]; inputType = DR068[9]; inputCode = DR068[10]; break;
          case 15234 : inputName = DR069[1]; inputAir = DR069[2]; inputEnergy = DR069[3]; inputProtein = DR069[4]; inputLemak = DR069[5]; inputKarbo = DR069[6]; inputSerat = DR069[7]; inputCarbing = DR069[8]; inputType = DR069[9]; inputCode = DR069[10]; break;
          case 15235 : inputName = DR070[1]; inputAir = DR070[2]; inputEnergy = DR070[3]; inputProtein = DR070[4]; inputLemak = DR070[5]; inputKarbo = DR070[6]; inputSerat = DR070[7]; inputCarbing = DR070[8]; inputType = DR070[9]; inputCode = DR070[10]; break;
          case 15241 : inputName = DR071[1]; inputAir = DR071[2]; inputEnergy = DR071[3]; inputProtein = DR071[4]; inputLemak = DR071[5]; inputKarbo = DR071[6]; inputSerat = DR071[7]; inputCarbing = DR071[8]; inputType = DR071[9]; inputCode = DR071[10]; break;
          case 15242 : inputName = DR072[1]; inputAir = DR072[2]; inputEnergy = DR072[3]; inputProtein = DR072[4]; inputLemak = DR072[5]; inputKarbo = DR072[6]; inputSerat = DR072[7]; inputCarbing = DR072[8]; inputType = DR072[9]; inputCode = DR072[10]; break;
          case 15243 : inputName = DR073[1]; inputAir = DR073[2]; inputEnergy = DR073[3]; inputProtein = DR073[4]; inputLemak = DR073[5]; inputKarbo = DR073[6]; inputSerat = DR073[7]; inputCarbing = DR073[8]; inputType = DR073[9]; inputCode = DR073[10]; break;
          case 15244 : inputName = DR074[1]; inputAir = DR074[2]; inputEnergy = DR074[3]; inputProtein = DR074[4]; inputLemak = DR074[5]; inputKarbo = DR074[6]; inputSerat = DR074[7]; inputCarbing = DR074[8]; inputType = DR074[9]; inputCode = DR074[10]; break;
          case 15245 : inputName = DR075[1]; inputAir = DR075[2]; inputEnergy = DR075[3]; inputProtein = DR075[4]; inputLemak = DR075[5]; inputKarbo = DR075[6]; inputSerat = DR075[7]; inputCarbing = DR075[8]; inputType = DR075[9]; inputCode = DR075[10]; break;
          case 15251 : inputName = DR076[1]; inputAir = DR076[2]; inputEnergy = DR076[3]; inputProtein = DR076[4]; inputLemak = DR076[5]; inputKarbo = DR076[6]; inputSerat = DR076[7]; inputCarbing = DR076[8]; inputType = DR076[9]; inputCode = DR076[10]; break;
          case 15252 : inputName = DR077[1]; inputAir = DR077[2]; inputEnergy = DR077[3]; inputProtein = DR077[4]; inputLemak = DR077[5]; inputKarbo = DR077[6]; inputSerat = DR077[7]; inputCarbing = DR077[8]; inputType = DR077[9]; inputCode = DR077[10]; break;
          case 15253 : inputName = DR078[1]; inputAir = DR078[2]; inputEnergy = DR078[3]; inputProtein = DR078[4]; inputLemak = DR078[5]; inputKarbo = DR078[6]; inputSerat = DR078[7]; inputCarbing = DR078[8]; inputType = DR078[9]; inputCode = DR078[10]; break;
          case 15254 : inputName = DR079[1]; inputAir = DR079[2]; inputEnergy = DR079[3]; inputProtein = DR079[4]; inputLemak = DR079[5]; inputKarbo = DR079[6]; inputSerat = DR079[7]; inputCarbing = DR079[8]; inputType = DR079[9]; inputCode = DR079[10]; break;
          case 15255 : inputName = DR080[1]; inputAir = DR080[2]; inputEnergy = DR080[3]; inputProtein = DR080[4]; inputLemak = DR080[5]; inputKarbo = DR080[6]; inputSerat = DR080[7]; inputCarbing = DR080[8]; inputType = DR080[9]; inputCode = DR080[10]; break;
          case 15311 : inputName = DR081[1]; inputAir = DR081[2]; inputEnergy = DR081[3]; inputProtein = DR081[4]; inputLemak = DR081[5]; inputKarbo = DR081[6]; inputSerat = DR081[7]; inputCarbing = DR081[8]; inputType = DR081[9]; inputCode = DR081[10]; break;
          case 15312 : inputName = DR082[1]; inputAir = DR082[2]; inputEnergy = DR082[3]; inputProtein = DR082[4]; inputLemak = DR082[5]; inputKarbo = DR082[6]; inputSerat = DR082[7]; inputCarbing = DR082[8]; inputType = DR082[9]; inputCode = DR082[10]; break;
          case 15313 : inputName = DR083[1]; inputAir = DR083[2]; inputEnergy = DR083[3]; inputProtein = DR083[4]; inputLemak = DR083[5]; inputKarbo = DR083[6]; inputSerat = DR083[7]; inputCarbing = DR083[8]; inputType = DR083[9]; inputCode = DR083[10]; break;
          case 15314 : inputName = DR084[1]; inputAir = DR084[2]; inputEnergy = DR084[3]; inputProtein = DR084[4]; inputLemak = DR084[5]; inputKarbo = DR084[6]; inputSerat = DR084[7]; inputCarbing = DR084[8]; inputType = DR084[9]; inputCode = DR084[10]; break;
          case 15315 : inputName = DR085[1]; inputAir = DR085[2]; inputEnergy = DR085[3]; inputProtein = DR085[4]; inputLemak = DR085[5]; inputKarbo = DR085[6]; inputSerat = DR085[7]; inputCarbing = DR085[8]; inputType = DR085[9]; inputCode = DR085[10]; break;
          case 15321 : inputName = DR086[1]; inputAir = DR086[2]; inputEnergy = DR086[3]; inputProtein = DR086[4]; inputLemak = DR086[5]; inputKarbo = DR086[6]; inputSerat = DR086[7]; inputCarbing = DR086[8]; inputType = DR086[9]; inputCode = DR086[10]; break;
          case 15322 : inputName = DR087[1]; inputAir = DR087[2]; inputEnergy = DR087[3]; inputProtein = DR087[4]; inputLemak = DR087[5]; inputKarbo = DR087[6]; inputSerat = DR087[7]; inputCarbing = DR087[8]; inputType = DR087[9]; inputCode = DR087[10]; break;
          case 15323 : inputName = DR088[1]; inputAir = DR088[2]; inputEnergy = DR088[3]; inputProtein = DR088[4]; inputLemak = DR088[5]; inputKarbo = DR088[6]; inputSerat = DR088[7]; inputCarbing = DR088[8]; inputType = DR088[9]; inputCode = DR088[10]; break;
          case 15324 : inputName = DR089[1]; inputAir = DR089[2]; inputEnergy = DR089[3]; inputProtein = DR089[4]; inputLemak = DR089[5]; inputKarbo = DR089[6]; inputSerat = DR089[7]; inputCarbing = DR089[8]; inputType = DR089[9]; inputCode = DR089[10]; break;
          case 15325 : inputName = DR090[1]; inputAir = DR090[2]; inputEnergy = DR090[3]; inputProtein = DR090[4]; inputLemak = DR090[5]; inputKarbo = DR090[6]; inputSerat = DR090[7]; inputCarbing = DR090[8]; inputType = DR090[9]; inputCode = DR090[10]; break;
          case 15331 : inputName = DR091[1]; inputAir = DR091[2]; inputEnergy = DR091[3]; inputProtein = DR091[4]; inputLemak = DR091[5]; inputKarbo = DR091[6]; inputSerat = DR091[7]; inputCarbing = DR091[8]; inputType = DR091[9]; inputCode = DR091[10]; break;
          case 15332 : inputName = DR092[1]; inputAir = DR092[2]; inputEnergy = DR092[3]; inputProtein = DR092[4]; inputLemak = DR092[5]; inputKarbo = DR092[6]; inputSerat = DR092[7]; inputCarbing = DR092[8]; inputType = DR092[9]; inputCode = DR092[10]; break;
          case 15333 : inputName = DR093[1]; inputAir = DR093[2]; inputEnergy = DR093[3]; inputProtein = DR093[4]; inputLemak = DR093[5]; inputKarbo = DR093[6]; inputSerat = DR093[7]; inputCarbing = DR093[8]; inputType = DR093[9]; inputCode = DR093[10]; break;
          case 15334 : inputName = DR094[1]; inputAir = DR094[2]; inputEnergy = DR094[3]; inputProtein = DR094[4]; inputLemak = DR094[5]; inputKarbo = DR094[6]; inputSerat = DR094[7]; inputCarbing = DR094[8]; inputType = DR094[9]; inputCode = DR094[10]; break;
          case 15335 : inputName = DR095[1]; inputAir = DR095[2]; inputEnergy = DR095[3]; inputProtein = DR095[4]; inputLemak = DR095[5]; inputKarbo = DR095[6]; inputSerat = DR095[7]; inputCarbing = DR095[8]; inputType = DR095[9]; inputCode = DR095[10]; break;
          case 15341 : inputName = DR096[1]; inputAir = DR096[2]; inputEnergy = DR096[3]; inputProtein = DR096[4]; inputLemak = DR096[5]; inputKarbo = DR096[6]; inputSerat = DR096[7]; inputCarbing = DR096[8]; inputType = DR096[9]; inputCode = DR096[10]; break;
          case 15342 : inputName = DR097[1]; inputAir = DR097[2]; inputEnergy = DR097[3]; inputProtein = DR097[4]; inputLemak = DR097[5]; inputKarbo = DR097[6]; inputSerat = DR097[7]; inputCarbing = DR097[8]; inputType = DR097[9]; inputCode = DR097[10]; break;
          case 15343 : inputName = DR098[1]; inputAir = DR098[2]; inputEnergy = DR098[3]; inputProtein = DR098[4]; inputLemak = DR098[5]; inputKarbo = DR098[6]; inputSerat = DR098[7]; inputCarbing = DR098[8]; inputType = DR098[9]; inputCode = DR098[10]; break;
          case 15344 : inputName = DR099[1]; inputAir = DR099[2]; inputEnergy = DR099[3]; inputProtein = DR099[4]; inputLemak = DR099[5]; inputKarbo = DR099[6]; inputSerat = DR099[7]; inputCarbing = DR099[8]; inputType = DR099[9]; inputCode = DR099[10]; break;
          case 15345 : inputName = DR100[1]; inputAir = DR100[2]; inputEnergy = DR100[3]; inputProtein = DR100[4]; inputLemak = DR100[5]; inputKarbo = DR100[6]; inputSerat = DR100[7]; inputCarbing = DR100[8]; inputType = DR100[9]; inputCode = DR100[10]; break;
          case 15351 : inputName = DR101[1]; inputAir = DR101[2]; inputEnergy = DR101[3]; inputProtein = DR101[4]; inputLemak = DR101[5]; inputKarbo = DR101[6]; inputSerat = DR101[7]; inputCarbing = DR101[8]; inputType = DR101[9]; inputCode = DR101[10]; break;
          case 15352 : inputName = DR102[1]; inputAir = DR102[2]; inputEnergy = DR102[3]; inputProtein = DR102[4]; inputLemak = DR102[5]; inputKarbo = DR102[6]; inputSerat = DR102[7]; inputCarbing = DR102[8]; inputType = DR102[9]; inputCode = DR102[10]; break;
          case 15353 : inputName = DR103[1]; inputAir = DR103[2]; inputEnergy = DR103[3]; inputProtein = DR103[4]; inputLemak = DR103[5]; inputKarbo = DR103[6]; inputSerat = DR103[7]; inputCarbing = DR103[8]; inputType = DR103[9]; inputCode = DR103[10]; break;
          case 15354 : inputName = DR104[1]; inputAir = DR104[2]; inputEnergy = DR104[3]; inputProtein = DR104[4]; inputLemak = DR104[5]; inputKarbo = DR104[6]; inputSerat = DR104[7]; inputCarbing = DR104[8]; inputType = DR104[9]; inputCode = DR104[10]; break;
          case 15355 : inputName = DR105[1]; inputAir = DR105[2]; inputEnergy = DR105[3]; inputProtein = DR105[4]; inputLemak = DR105[5]; inputKarbo = DR105[6]; inputSerat = DR105[7]; inputCarbing = DR105[8]; inputType = DR105[9]; inputCode = DR105[10]; break;
          case 15411 : inputName = DR106[1]; inputAir = DR106[2]; inputEnergy = DR106[3]; inputProtein = DR106[4]; inputLemak = DR106[5]; inputKarbo = DR106[6]; inputSerat = DR106[7]; inputCarbing = DR106[8]; inputType = DR106[9]; inputCode = DR106[10]; break;
          case 15412 : inputName = DR107[1]; inputAir = DR107[2]; inputEnergy = DR107[3]; inputProtein = DR107[4]; inputLemak = DR107[5]; inputKarbo = DR107[6]; inputSerat = DR107[7]; inputCarbing = DR107[8]; inputType = DR107[9]; inputCode = DR107[10]; break;
          case 15413 : inputName = DR108[1]; inputAir = DR108[2]; inputEnergy = DR108[3]; inputProtein = DR108[4]; inputLemak = DR108[5]; inputKarbo = DR108[6]; inputSerat = DR108[7]; inputCarbing = DR108[8]; inputType = DR108[9]; inputCode = DR108[10]; break;
          case 15414 : inputName = DR109[1]; inputAir = DR109[2]; inputEnergy = DR109[3]; inputProtein = DR109[4]; inputLemak = DR109[5]; inputKarbo = DR109[6]; inputSerat = DR109[7]; inputCarbing = DR109[8]; inputType = DR109[9]; inputCode = DR109[10]; break;
          case 15415 : inputName = DR110[1]; inputAir = DR110[2]; inputEnergy = DR110[3]; inputProtein = DR110[4]; inputLemak = DR110[5]; inputKarbo = DR110[6]; inputSerat = DR110[7]; inputCarbing = DR110[8]; inputType = DR110[9]; inputCode = DR110[10]; break;
          case 15421 : inputName = DR111[1]; inputAir = DR111[2]; inputEnergy = DR111[3]; inputProtein = DR111[4]; inputLemak = DR111[5]; inputKarbo = DR111[6]; inputSerat = DR111[7]; inputCarbing = DR111[8]; inputType = DR111[9]; inputCode = DR111[10]; break;
          case 15422 : inputName = DR112[1]; inputAir = DR112[2]; inputEnergy = DR112[3]; inputProtein = DR112[4]; inputLemak = DR112[5]; inputKarbo = DR112[6]; inputSerat = DR112[7]; inputCarbing = DR112[8]; inputType = DR112[9]; inputCode = DR112[10]; break;
          case 15423 : inputName = DR113[1]; inputAir = DR113[2]; inputEnergy = DR113[3]; inputProtein = DR113[4]; inputLemak = DR113[5]; inputKarbo = DR113[6]; inputSerat = DR113[7]; inputCarbing = DR113[8]; inputType = DR113[9]; inputCode = DR113[10]; break;
          case 15424 : inputName = DR114[1]; inputAir = DR114[2]; inputEnergy = DR114[3]; inputProtein = DR114[4]; inputLemak = DR114[5]; inputKarbo = DR114[6]; inputSerat = DR114[7]; inputCarbing = DR114[8]; inputType = DR114[9]; inputCode = DR114[10]; break;
          case 15425 : inputName = DR115[1]; inputAir = DR115[2]; inputEnergy = DR115[3]; inputProtein = DR115[4]; inputLemak = DR115[5]; inputKarbo = DR115[6]; inputSerat = DR115[7]; inputCarbing = DR115[8]; inputType = DR115[9]; inputCode = DR115[10]; break;
          case 15431 : inputName = DR116[1]; inputAir = DR116[2]; inputEnergy = DR116[3]; inputProtein = DR116[4]; inputLemak = DR116[5]; inputKarbo = DR116[6]; inputSerat = DR116[7]; inputCarbing = DR116[8]; inputType = DR116[9]; inputCode = DR116[10]; break;
          case 15432 : inputName = DR117[1]; inputAir = DR117[2]; inputEnergy = DR117[3]; inputProtein = DR117[4]; inputLemak = DR117[5]; inputKarbo = DR117[6]; inputSerat = DR117[7]; inputCarbing = DR117[8]; inputType = DR117[9]; inputCode = DR117[10]; break;
          case 15433 : inputName = DR118[1]; inputAir = DR118[2]; inputEnergy = DR118[3]; inputProtein = DR118[4]; inputLemak = DR118[5]; inputKarbo = DR118[6]; inputSerat = DR118[7]; inputCarbing = DR118[8]; inputType = DR118[9]; inputCode = DR118[10]; break;
          case 15434 : inputName = DR119[1]; inputAir = DR119[2]; inputEnergy = DR119[3]; inputProtein = DR119[4]; inputLemak = DR119[5]; inputKarbo = DR119[6]; inputSerat = DR119[7]; inputCarbing = DR119[8]; inputType = DR119[9]; inputCode = DR119[10]; break;
          case 15435 : inputName = DR120[1]; inputAir = DR120[2]; inputEnergy = DR120[3]; inputProtein = DR120[4]; inputLemak = DR120[5]; inputKarbo = DR120[6]; inputSerat = DR120[7]; inputCarbing = DR120[8]; inputType = DR120[9]; inputCode = DR120[10]; break;
          case 15441 : inputName = DR121[1]; inputAir = DR121[2]; inputEnergy = DR121[3]; inputProtein = DR121[4]; inputLemak = DR121[5]; inputKarbo = DR121[6]; inputSerat = DR121[7]; inputCarbing = DR121[8]; inputType = DR121[9]; inputCode = DR121[10]; break;
          case 15442 : inputName = DR122[1]; inputAir = DR122[2]; inputEnergy = DR122[3]; inputProtein = DR122[4]; inputLemak = DR122[5]; inputKarbo = DR122[6]; inputSerat = DR122[7]; inputCarbing = DR122[8]; inputType = DR122[9]; inputCode = DR122[10]; break;
          case 15443 : inputName = DR123[1]; inputAir = DR123[2]; inputEnergy = DR123[3]; inputProtein = DR123[4]; inputLemak = DR123[5]; inputKarbo = DR123[6]; inputSerat = DR123[7]; inputCarbing = DR123[8]; inputType = DR123[9]; inputCode = DR123[10]; break;
          case 15444 : inputName = DR124[1]; inputAir = DR124[2]; inputEnergy = DR124[3]; inputProtein = DR124[4]; inputLemak = DR124[5]; inputKarbo = DR124[6]; inputSerat = DR124[7]; inputCarbing = DR124[8]; inputType = DR124[9]; inputCode = DR124[10]; break;
          case 15445 : inputName = DR125[1]; inputAir = DR125[2]; inputEnergy = DR125[3]; inputProtein = DR125[4]; inputLemak = DR125[5]; inputKarbo = DR125[6]; inputSerat = DR125[7]; inputCarbing = DR125[8]; inputType = DR125[9]; inputCode = DR125[10]; break;
          case 15451 : inputName = DR126[1]; inputAir = DR126[2]; inputEnergy = DR126[3]; inputProtein = DR126[4]; inputLemak = DR126[5]; inputKarbo = DR126[6]; inputSerat = DR126[7]; inputCarbing = DR126[8]; inputType = DR126[9]; inputCode = DR126[10]; break;
          case 15452 : inputName = DR127[1]; inputAir = DR127[2]; inputEnergy = DR127[3]; inputProtein = DR127[4]; inputLemak = DR127[5]; inputKarbo = DR127[6]; inputSerat = DR127[7]; inputCarbing = DR127[8]; inputType = DR127[9]; inputCode = DR127[10]; break;
          case 15453 : inputName = DR128[1]; inputAir = DR128[2]; inputEnergy = DR128[3]; inputProtein = DR128[4]; inputLemak = DR128[5]; inputKarbo = DR128[6]; inputSerat = DR128[7]; inputCarbing = DR128[8]; inputType = DR128[9]; inputCode = DR128[10]; break;
          case 15454 : inputName = DR129[1]; inputAir = DR129[2]; inputEnergy = DR129[3]; inputProtein = DR129[4]; inputLemak = DR129[5]; inputKarbo = DR129[6]; inputSerat = DR129[7]; inputCarbing = DR129[8]; inputType = DR129[9]; inputCode = DR129[10]; break;
          case 15455 : inputName = DR130[1]; inputAir = DR130[2]; inputEnergy = DR130[3]; inputProtein = DR130[4]; inputLemak = DR130[5]; inputKarbo = DR130[6]; inputSerat = DR130[7]; inputCarbing = DR130[8]; inputType = DR130[9]; inputCode = DR130[10]; break;
          case 15511 : inputName = DR131[1]; inputAir = DR131[2]; inputEnergy = DR131[3]; inputProtein = DR131[4]; inputLemak = DR131[5]; inputKarbo = DR131[6]; inputSerat = DR131[7]; inputCarbing = DR131[8]; inputType = DR131[9]; inputCode = DR131[10]; break;
          case 15512 : inputName = DR132[1]; inputAir = DR132[2]; inputEnergy = DR132[3]; inputProtein = DR132[4]; inputLemak = DR132[5]; inputKarbo = DR132[6]; inputSerat = DR132[7]; inputCarbing = DR132[8]; inputType = DR132[9]; inputCode = DR132[10]; break;
          case 15513 : inputName = DR133[1]; inputAir = DR133[2]; inputEnergy = DR133[3]; inputProtein = DR133[4]; inputLemak = DR133[5]; inputKarbo = DR133[6]; inputSerat = DR133[7]; inputCarbing = DR133[8]; inputType = DR133[9]; inputCode = DR133[10]; break;
          case 15514 : inputName = DR134[1]; inputAir = DR134[2]; inputEnergy = DR134[3]; inputProtein = DR134[4]; inputLemak = DR134[5]; inputKarbo = DR134[6]; inputSerat = DR134[7]; inputCarbing = DR134[8]; inputType = DR134[9]; inputCode = DR134[10]; break;
          case 15515 : inputName = DR135[1]; inputAir = DR135[2]; inputEnergy = DR135[3]; inputProtein = DR135[4]; inputLemak = DR135[5]; inputKarbo = DR135[6]; inputSerat = DR135[7]; inputCarbing = DR135[8]; inputType = DR135[9]; inputCode = DR135[10]; break;
          case 15521 : inputName = DR136[1]; inputAir = DR136[2]; inputEnergy = DR136[3]; inputProtein = DR136[4]; inputLemak = DR136[5]; inputKarbo = DR136[6]; inputSerat = DR136[7]; inputCarbing = DR136[8]; inputType = DR136[9]; inputCode = DR136[10]; break;
          case 15522 : inputName = DR137[1]; inputAir = DR137[2]; inputEnergy = DR137[3]; inputProtein = DR137[4]; inputLemak = DR137[5]; inputKarbo = DR137[6]; inputSerat = DR137[7]; inputCarbing = DR137[8]; inputType = DR137[9]; inputCode = DR137[10]; break;
          case 15523 : inputName = DR138[1]; inputAir = DR138[2]; inputEnergy = DR138[3]; inputProtein = DR138[4]; inputLemak = DR138[5]; inputKarbo = DR138[6]; inputSerat = DR138[7]; inputCarbing = DR138[8]; inputType = DR138[9]; inputCode = DR138[10]; break;
          case 15524 : inputName = DR139[1]; inputAir = DR139[2]; inputEnergy = DR139[3]; inputProtein = DR139[4]; inputLemak = DR139[5]; inputKarbo = DR139[6]; inputSerat = DR139[7]; inputCarbing = DR139[8]; inputType = DR139[9]; inputCode = DR139[10]; break;
          case 15525 : inputName = DR140[1]; inputAir = DR140[2]; inputEnergy = DR140[3]; inputProtein = DR140[4]; inputLemak = DR140[5]; inputKarbo = DR140[6]; inputSerat = DR140[7]; inputCarbing = DR140[8]; inputType = DR140[9]; inputCode = DR140[10]; break;
          case 15531 : inputName = DR141[1]; inputAir = DR141[2]; inputEnergy = DR141[3]; inputProtein = DR141[4]; inputLemak = DR141[5]; inputKarbo = DR141[6]; inputSerat = DR141[7]; inputCarbing = DR141[8]; inputType = DR141[9]; inputCode = DR141[10]; break;
          case 15532 : inputName = DR142[1]; inputAir = DR142[2]; inputEnergy = DR142[3]; inputProtein = DR142[4]; inputLemak = DR142[5]; inputKarbo = DR142[6]; inputSerat = DR142[7]; inputCarbing = DR142[8]; inputType = DR142[9]; inputCode = DR142[10]; break;
          case 15533 : inputName = DR143[1]; inputAir = DR143[2]; inputEnergy = DR143[3]; inputProtein = DR143[4]; inputLemak = DR143[5]; inputKarbo = DR143[6]; inputSerat = DR143[7]; inputCarbing = DR143[8]; inputType = DR143[9]; inputCode = DR143[10]; break;
          case 15534 : inputName = DR144[1]; inputAir = DR144[2]; inputEnergy = DR144[3]; inputProtein = DR144[4]; inputLemak = DR144[5]; inputKarbo = DR144[6]; inputSerat = DR144[7]; inputCarbing = DR144[8]; inputType = DR144[9]; inputCode = DR144[10]; break;
          case 15535 : inputName = DR145[1]; inputAir = DR145[2]; inputEnergy = DR145[3]; inputProtein = DR145[4]; inputLemak = DR145[5]; inputKarbo = DR145[6]; inputSerat = DR145[7]; inputCarbing = DR145[8]; inputType = DR145[9]; inputCode = DR145[10]; break;
          case 15541 : inputName = DR146[1]; inputAir = DR146[2]; inputEnergy = DR146[3]; inputProtein = DR146[4]; inputLemak = DR146[5]; inputKarbo = DR146[6]; inputSerat = DR146[7]; inputCarbing = DR146[8]; inputType = DR146[9]; inputCode = DR146[10]; break;
          case 15542 : inputName = DR147[1]; inputAir = DR147[2]; inputEnergy = DR147[3]; inputProtein = DR147[4]; inputLemak = DR147[5]; inputKarbo = DR147[6]; inputSerat = DR147[7]; inputCarbing = DR147[8]; inputType = DR147[9]; inputCode = DR147[10]; break;
          case 15543 : inputName = DR148[1]; inputAir = DR148[2]; inputEnergy = DR148[3]; inputProtein = DR148[4]; inputLemak = DR148[5]; inputKarbo = DR148[6]; inputSerat = DR148[7]; inputCarbing = DR148[8]; inputType = DR148[9]; inputCode = DR148[10]; break;
          case 15544 : inputName = DR149[1]; inputAir = DR149[2]; inputEnergy = DR149[3]; inputProtein = DR149[4]; inputLemak = DR149[5]; inputKarbo = DR149[6]; inputSerat = DR149[7]; inputCarbing = DR149[8]; inputType = DR149[9]; inputCode = DR149[10]; break;
          case 15545 : inputName = DR150[1]; inputAir = DR150[2]; inputEnergy = DR150[3]; inputProtein = DR150[4]; inputLemak = DR150[5]; inputKarbo = DR150[6]; inputSerat = DR150[7]; inputCarbing = DR150[8]; inputType = DR150[9]; inputCode = DR150[10]; break;
          case 15551 : inputName = DR151[1]; inputAir = DR151[2]; inputEnergy = DR151[3]; inputProtein = DR151[4]; inputLemak = DR151[5]; inputKarbo = DR151[6]; inputSerat = DR151[7]; inputCarbing = DR151[8]; inputType = DR151[9]; inputCode = DR151[10]; break;
          case 15552 : inputName = DR152[1]; inputAir = DR152[2]; inputEnergy = DR152[3]; inputProtein = DR152[4]; inputLemak = DR152[5]; inputKarbo = DR152[6]; inputSerat = DR152[7]; inputCarbing = DR152[8]; inputType = DR152[9]; inputCode = DR152[10]; break;
          case 15553 : inputName = DR153[1]; inputAir = DR153[2]; inputEnergy = DR153[3]; inputProtein = DR153[4]; inputLemak = DR153[5]; inputKarbo = DR153[6]; inputSerat = DR153[7]; inputCarbing = DR153[8]; inputType = DR153[9]; inputCode = DR153[10]; break;
          case 15554 : inputName = DR154[1]; inputAir = DR154[2]; inputEnergy = DR154[3]; inputProtein = DR154[4]; inputLemak = DR154[5]; inputKarbo = DR154[6]; inputSerat = DR154[7]; inputCarbing = DR154[8]; inputType = DR154[9]; inputCode = DR154[10]; break;
          case 15555 : inputName = DR155[1]; inputAir = DR155[2]; inputEnergy = DR155[3]; inputProtein = DR155[4]; inputLemak = DR155[5]; inputKarbo = DR155[6]; inputSerat = DR155[7]; inputCarbing = DR155[8]; inputType = DR155[9]; inputCode = DR155[10]; break;
          case 151111 : inputName = DR156[1]; inputAir = DR156[2]; inputEnergy = DR156[3]; inputProtein = DR156[4]; inputLemak = DR156[5]; inputKarbo = DR156[6]; inputSerat = DR156[7]; inputCarbing = DR156[8]; inputType = DR156[9]; inputCode = DR156[10]; break;
          case 151112 : inputName = DR157[1]; inputAir = DR157[2]; inputEnergy = DR157[3]; inputProtein = DR157[4]; inputLemak = DR157[5]; inputKarbo = DR157[6]; inputSerat = DR157[7]; inputCarbing = DR157[8]; inputType = DR157[9]; inputCode = DR157[10]; break;
          case 151113 : inputName = DR158[1]; inputAir = DR158[2]; inputEnergy = DR158[3]; inputProtein = DR158[4]; inputLemak = DR158[5]; inputKarbo = DR158[6]; inputSerat = DR158[7]; inputCarbing = DR158[8]; inputType = DR158[9]; inputCode = DR158[10]; break;
          case 151114 : inputName = DR159[1]; inputAir = DR159[2]; inputEnergy = DR159[3]; inputProtein = DR159[4]; inputLemak = DR159[5]; inputKarbo = DR159[6]; inputSerat = DR159[7]; inputCarbing = DR159[8]; inputType = DR159[9]; inputCode = DR159[10]; break;
          case 151115 : inputName = DR160[1]; inputAir = DR160[2]; inputEnergy = DR160[3]; inputProtein = DR160[4]; inputLemak = DR160[5]; inputKarbo = DR160[6]; inputSerat = DR160[7]; inputCarbing = DR160[8]; inputType = DR160[9]; inputCode = DR160[10]; break;
          case 151121 : inputName = DR161[1]; inputAir = DR161[2]; inputEnergy = DR161[3]; inputProtein = DR161[4]; inputLemak = DR161[5]; inputKarbo = DR161[6]; inputSerat = DR161[7]; inputCarbing = DR161[8]; inputType = DR161[9]; inputCode = DR161[10]; break;
          case 151122 : inputName = DR162[1]; inputAir = DR162[2]; inputEnergy = DR162[3]; inputProtein = DR162[4]; inputLemak = DR162[5]; inputKarbo = DR162[6]; inputSerat = DR162[7]; inputCarbing = DR162[8]; inputType = DR162[9]; inputCode = DR162[10]; break;
          case 151123 : inputName = DR163[1]; inputAir = DR163[2]; inputEnergy = DR163[3]; inputProtein = DR163[4]; inputLemak = DR163[5]; inputKarbo = DR163[6]; inputSerat = DR163[7]; inputCarbing = DR163[8]; inputType = DR163[9]; inputCode = DR163[10]; break;
          case 151124 : inputName = DR164[1]; inputAir = DR164[2]; inputEnergy = DR164[3]; inputProtein = DR164[4]; inputLemak = DR164[5]; inputKarbo = DR164[6]; inputSerat = DR164[7]; inputCarbing = DR164[8]; inputType = DR164[9]; inputCode = DR164[10]; break;
          case 151125 : inputName = DR165[1]; inputAir = DR165[2]; inputEnergy = DR165[3]; inputProtein = DR165[4]; inputLemak = DR165[5]; inputKarbo = DR165[6]; inputSerat = DR165[7]; inputCarbing = DR165[8]; inputType = DR165[9]; inputCode = DR165[10]; break;
          case 151131 : inputName = DR166[1]; inputAir = DR166[2]; inputEnergy = DR166[3]; inputProtein = DR166[4]; inputLemak = DR166[5]; inputKarbo = DR166[6]; inputSerat = DR166[7]; inputCarbing = DR166[8]; inputType = DR166[9]; inputCode = DR166[10]; break;
          case 151132 : inputName = DP001[1]; inputAir = DP001[2]; inputEnergy = DP001[3]; inputProtein = DP001[4]; inputLemak = DP001[5]; inputKarbo = DP001[6]; inputSerat = DP001[7]; inputCarbing = DP001[8]; inputType = DP001[9]; inputCode = DP001[10]; break;
          case 151133 : inputName = DP002[1]; inputAir = DP002[2]; inputEnergy = DP002[3]; inputProtein = DP002[4]; inputLemak = DP002[5]; inputKarbo = DP002[6]; inputSerat = DP002[7]; inputCarbing = DP002[8]; inputType = DP002[9]; inputCode = DP002[10]; break;
          case 151134 : inputName = DP003[1]; inputAir = DP003[2]; inputEnergy = DP003[3]; inputProtein = DP003[4]; inputLemak = DP003[5]; inputKarbo = DP003[6]; inputSerat = DP003[7]; inputCarbing = DP003[8]; inputType = DP003[9]; inputCode = DP003[10]; break;
          case 151135 : inputName = DP004[1]; inputAir = DP004[2]; inputEnergy = DP004[3]; inputProtein = DP004[4]; inputLemak = DP004[5]; inputKarbo = DP004[6]; inputSerat = DP004[7]; inputCarbing = DP004[8]; inputType = DP004[9]; inputCode = DP004[10]; break;
          case 151141 : inputName = DP005[1]; inputAir = DP005[2]; inputEnergy = DP005[3]; inputProtein = DP005[4]; inputLemak = DP005[5]; inputKarbo = DP005[6]; inputSerat = DP005[7]; inputCarbing = DP005[8]; inputType = DP005[9]; inputCode = DP005[10]; break;
          case 151142 : inputName = DP006[1]; inputAir = DP006[2]; inputEnergy = DP006[3]; inputProtein = DP006[4]; inputLemak = DP006[5]; inputKarbo = DP006[6]; inputSerat = DP006[7]; inputCarbing = DP006[8]; inputType = DP006[9]; inputCode = DP006[10]; break;
          case 151143 : inputName = DP007[1]; inputAir = DP007[2]; inputEnergy = DP007[3]; inputProtein = DP007[4]; inputLemak = DP007[5]; inputKarbo = DP007[6]; inputSerat = DP007[7]; inputCarbing = DP007[8]; inputType = DP007[9]; inputCode = DP007[10]; break;
          case 151144 : inputName = DP008[1]; inputAir = DP008[2]; inputEnergy = DP008[3]; inputProtein = DP008[4]; inputLemak = DP008[5]; inputKarbo = DP008[6]; inputSerat = DP008[7]; inputCarbing = DP008[8]; inputType = DP008[9]; inputCode = DP008[10]; break;
          case 151145 : inputName = DP009[1]; inputAir = DP009[2]; inputEnergy = DP009[3]; inputProtein = DP009[4]; inputLemak = DP009[5]; inputKarbo = DP009[6]; inputSerat = DP009[7]; inputCarbing = DP009[8]; inputType = DP009[9]; inputCode = DP009[10]; break;
          case 151151 : inputName = DP010[1]; inputAir = DP010[2]; inputEnergy = DP010[3]; inputProtein = DP010[4]; inputLemak = DP010[5]; inputKarbo = DP010[6]; inputSerat = DP010[7]; inputCarbing = DP010[8]; inputType = DP010[9]; inputCode = DP010[10]; break;
          case 151152 : inputName = DP011[1]; inputAir = DP011[2]; inputEnergy = DP011[3]; inputProtein = DP011[4]; inputLemak = DP011[5]; inputKarbo = DP011[6]; inputSerat = DP011[7]; inputCarbing = DP011[8]; inputType = DP011[9]; inputCode = DP011[10]; break;
          case 151153 : inputName = DP012[1]; inputAir = DP012[2]; inputEnergy = DP012[3]; inputProtein = DP012[4]; inputLemak = DP012[5]; inputKarbo = DP012[6]; inputSerat = DP012[7]; inputCarbing = DP012[8]; inputType = DP012[9]; inputCode = DP012[10]; break;
          case 151154 : inputName = DP013[1]; inputAir = DP013[2]; inputEnergy = DP013[3]; inputProtein = DP013[4]; inputLemak = DP013[5]; inputKarbo = DP013[6]; inputSerat = DP013[7]; inputCarbing = DP013[8]; inputType = DP013[9]; inputCode = DP013[10]; break;
          case 151155 : inputName = DP014[1]; inputAir = DP014[2]; inputEnergy = DP014[3]; inputProtein = DP014[4]; inputLemak = DP014[5]; inputKarbo = DP014[6]; inputSerat = DP014[7]; inputCarbing = DP014[8]; inputType = DP014[9]; inputCode = DP014[10]; break;
          case 151211 : inputName = DP015[1]; inputAir = DP015[2]; inputEnergy = DP015[3]; inputProtein = DP015[4]; inputLemak = DP015[5]; inputKarbo = DP015[6]; inputSerat = DP015[7]; inputCarbing = DP015[8]; inputType = DP015[9]; inputCode = DP015[10]; break;
          case 151212 : inputName = DP016[1]; inputAir = DP016[2]; inputEnergy = DP016[3]; inputProtein = DP016[4]; inputLemak = DP016[5]; inputKarbo = DP016[6]; inputSerat = DP016[7]; inputCarbing = DP016[8]; inputType = DP016[9]; inputCode = DP016[10]; break;
          case 151213 : inputName = DP017[1]; inputAir = DP017[2]; inputEnergy = DP017[3]; inputProtein = DP017[4]; inputLemak = DP017[5]; inputKarbo = DP017[6]; inputSerat = DP017[7]; inputCarbing = DP017[8]; inputType = DP017[9]; inputCode = DP017[10]; break;
          case 151214 : inputName = DP018[1]; inputAir = DP018[2]; inputEnergy = DP018[3]; inputProtein = DP018[4]; inputLemak = DP018[5]; inputKarbo = DP018[6]; inputSerat = DP018[7]; inputCarbing = DP018[8]; inputType = DP018[9]; inputCode = DP018[10]; break;
          case 151215 : inputName = DP019[1]; inputAir = DP019[2]; inputEnergy = DP019[3]; inputProtein = DP019[4]; inputLemak = DP019[5]; inputKarbo = DP019[6]; inputSerat = DP019[7]; inputCarbing = DP019[8]; inputType = DP019[9]; inputCode = DP019[10]; break;
          case 151221 : inputName = DP020[1]; inputAir = DP020[2]; inputEnergy = DP020[3]; inputProtein = DP020[4]; inputLemak = DP020[5]; inputKarbo = DP020[6]; inputSerat = DP020[7]; inputCarbing = DP020[8]; inputType = DP020[9]; inputCode = DP020[10]; break;
          case 151222 : inputName = DP021[1]; inputAir = DP021[2]; inputEnergy = DP021[3]; inputProtein = DP021[4]; inputLemak = DP021[5]; inputKarbo = DP021[6]; inputSerat = DP021[7]; inputCarbing = DP021[8]; inputType = DP021[9]; inputCode = DP021[10]; break;
          case 151223 : inputName = DP022[1]; inputAir = DP022[2]; inputEnergy = DP022[3]; inputProtein = DP022[4]; inputLemak = DP022[5]; inputKarbo = DP022[6]; inputSerat = DP022[7]; inputCarbing = DP022[8]; inputType = DP022[9]; inputCode = DP022[10]; break;
          case 151224 : inputName = DP023[1]; inputAir = DP023[2]; inputEnergy = DP023[3]; inputProtein = DP023[4]; inputLemak = DP023[5]; inputKarbo = DP023[6]; inputSerat = DP023[7]; inputCarbing = DP023[8]; inputType = DP023[9]; inputCode = DP023[10]; break;
          case 151225 : inputName = DP024[1]; inputAir = DP024[2]; inputEnergy = DP024[3]; inputProtein = DP024[4]; inputLemak = DP024[5]; inputKarbo = DP024[6]; inputSerat = DP024[7]; inputCarbing = DP024[8]; inputType = DP024[9]; inputCode = DP024[10]; break;
          case 151231 : inputName = DP025[1]; inputAir = DP025[2]; inputEnergy = DP025[3]; inputProtein = DP025[4]; inputLemak = DP025[5]; inputKarbo = DP025[6]; inputSerat = DP025[7]; inputCarbing = DP025[8]; inputType = DP025[9]; inputCode = DP025[10]; break;
          case 151232 : inputName = DP026[1]; inputAir = DP026[2]; inputEnergy = DP026[3]; inputProtein = DP026[4]; inputLemak = DP026[5]; inputKarbo = DP026[6]; inputSerat = DP026[7]; inputCarbing = DP026[8]; inputType = DP026[9]; inputCode = DP026[10]; break;
          case 151233 : inputName = DP027[1]; inputAir = DP027[2]; inputEnergy = DP027[3]; inputProtein = DP027[4]; inputLemak = DP027[5]; inputKarbo = DP027[6]; inputSerat = DP027[7]; inputCarbing = DP027[8]; inputType = DP027[9]; inputCode = DP027[10]; break;
          case 151234 : inputName = DP028[1]; inputAir = DP028[2]; inputEnergy = DP028[3]; inputProtein = DP028[4]; inputLemak = DP028[5]; inputKarbo = DP028[6]; inputSerat = DP028[7]; inputCarbing = DP028[8]; inputType = DP028[9]; inputCode = DP028[10]; break;
          case 151235 : inputName = DP029[1]; inputAir = DP029[2]; inputEnergy = DP029[3]; inputProtein = DP029[4]; inputLemak = DP029[5]; inputKarbo = DP029[6]; inputSerat = DP029[7]; inputCarbing = DP029[8]; inputType = DP029[9]; inputCode = DP029[10]; break;
          case 151241 : inputName = DP030[1]; inputAir = DP030[2]; inputEnergy = DP030[3]; inputProtein = DP030[4]; inputLemak = DP030[5]; inputKarbo = DP030[6]; inputSerat = DP030[7]; inputCarbing = DP030[8]; inputType = DP030[9]; inputCode = DP030[10]; break;
          case 151242 : inputName = DP031[1]; inputAir = DP031[2]; inputEnergy = DP031[3]; inputProtein = DP031[4]; inputLemak = DP031[5]; inputKarbo = DP031[6]; inputSerat = DP031[7]; inputCarbing = DP031[8]; inputType = DP031[9]; inputCode = DP031[10]; break;
          case 151243 : inputName = DP032[1]; inputAir = DP032[2]; inputEnergy = DP032[3]; inputProtein = DP032[4]; inputLemak = DP032[5]; inputKarbo = DP032[6]; inputSerat = DP032[7]; inputCarbing = DP032[8]; inputType = DP032[9]; inputCode = DP032[10]; break;
          case 151244 : inputName = DP033[1]; inputAir = DP033[2]; inputEnergy = DP033[3]; inputProtein = DP033[4]; inputLemak = DP033[5]; inputKarbo = DP033[6]; inputSerat = DP033[7]; inputCarbing = DP033[8]; inputType = DP033[9]; inputCode = DP033[10]; break;
          case 151245 : inputName = DP034[1]; inputAir = DP034[2]; inputEnergy = DP034[3]; inputProtein = DP034[4]; inputLemak = DP034[5]; inputKarbo = DP034[6]; inputSerat = DP034[7]; inputCarbing = DP034[8]; inputType = DP034[9]; inputCode = DP034[10]; break;
          case 151251 : inputName = DP035[1]; inputAir = DP035[2]; inputEnergy = DP035[3]; inputProtein = DP035[4]; inputLemak = DP035[5]; inputKarbo = DP035[6]; inputSerat = DP035[7]; inputCarbing = DP035[8]; inputType = DP035[9]; inputCode = DP035[10]; break;
          case 151252 : inputName = DP036[1]; inputAir = DP036[2]; inputEnergy = DP036[3]; inputProtein = DP036[4]; inputLemak = DP036[5]; inputKarbo = DP036[6]; inputSerat = DP036[7]; inputCarbing = DP036[8]; inputType = DP036[9]; inputCode = DP036[10]; break;
          case 151253 : inputName = DP037[1]; inputAir = DP037[2]; inputEnergy = DP037[3]; inputProtein = DP037[4]; inputLemak = DP037[5]; inputKarbo = DP037[6]; inputSerat = DP037[7]; inputCarbing = DP037[8]; inputType = DP037[9]; inputCode = DP037[10]; break;
          case 151254 : inputName = DP038[1]; inputAir = DP038[2]; inputEnergy = DP038[3]; inputProtein = DP038[4]; inputLemak = DP038[5]; inputKarbo = DP038[6]; inputSerat = DP038[7]; inputCarbing = DP038[8]; inputType = DP038[9]; inputCode = DP038[10]; break;
          case 151255 : inputName = DP039[1]; inputAir = DP039[2]; inputEnergy = DP039[3]; inputProtein = DP039[4]; inputLemak = DP039[5]; inputKarbo = DP039[6]; inputSerat = DP039[7]; inputCarbing = DP039[8]; inputType = DP039[9]; inputCode = DP039[10]; break;
          case 151311 : inputName = DP040[1]; inputAir = DP040[2]; inputEnergy = DP040[3]; inputProtein = DP040[4]; inputLemak = DP040[5]; inputKarbo = DP040[6]; inputSerat = DP040[7]; inputCarbing = DP040[8]; inputType = DP040[9]; inputCode = DP040[10]; break;
          case 151312 : inputName = DP041[1]; inputAir = DP041[2]; inputEnergy = DP041[3]; inputProtein = DP041[4]; inputLemak = DP041[5]; inputKarbo = DP041[6]; inputSerat = DP041[7]; inputCarbing = DP041[8]; inputType = DP041[9]; inputCode = DP041[10]; break;
          case 151313 : inputName = DP042[1]; inputAir = DP042[2]; inputEnergy = DP042[3]; inputProtein = DP042[4]; inputLemak = DP042[5]; inputKarbo = DP042[6]; inputSerat = DP042[7]; inputCarbing = DP042[8]; inputType = DP042[9]; inputCode = DP042[10]; break;
          case 151314 : inputName = DP043[1]; inputAir = DP043[2]; inputEnergy = DP043[3]; inputProtein = DP043[4]; inputLemak = DP043[5]; inputKarbo = DP043[6]; inputSerat = DP043[7]; inputCarbing = DP043[8]; inputType = DP043[9]; inputCode = DP043[10]; break;
          case 151315 : inputName = DP044[1]; inputAir = DP044[2]; inputEnergy = DP044[3]; inputProtein = DP044[4]; inputLemak = DP044[5]; inputKarbo = DP044[6]; inputSerat = DP044[7]; inputCarbing = DP044[8]; inputType = DP044[9]; inputCode = DP044[10]; break;
          case 151321 : inputName = DP045[1]; inputAir = DP045[2]; inputEnergy = DP045[3]; inputProtein = DP045[4]; inputLemak = DP045[5]; inputKarbo = DP045[6]; inputSerat = DP045[7]; inputCarbing = DP045[8]; inputType = DP045[9]; inputCode = DP045[10]; break;
          case 151322 : inputName = DP046[1]; inputAir = DP046[2]; inputEnergy = DP046[3]; inputProtein = DP046[4]; inputLemak = DP046[5]; inputKarbo = DP046[6]; inputSerat = DP046[7]; inputCarbing = DP046[8]; inputType = DP046[9]; inputCode = DP046[10]; break;
          case 151323 : inputName = DP047[1]; inputAir = DP047[2]; inputEnergy = DP047[3]; inputProtein = DP047[4]; inputLemak = DP047[5]; inputKarbo = DP047[6]; inputSerat = DP047[7]; inputCarbing = DP047[8]; inputType = DP047[9]; inputCode = DP047[10]; break;
          case 151324 : inputName = DP048[1]; inputAir = DP048[2]; inputEnergy = DP048[3]; inputProtein = DP048[4]; inputLemak = DP048[5]; inputKarbo = DP048[6]; inputSerat = DP048[7]; inputCarbing = DP048[8]; inputType = DP048[9]; inputCode = DP048[10]; break;
          case 151325 : inputName = DP049[1]; inputAir = DP049[2]; inputEnergy = DP049[3]; inputProtein = DP049[4]; inputLemak = DP049[5]; inputKarbo = DP049[6]; inputSerat = DP049[7]; inputCarbing = DP049[8]; inputType = DP049[9]; inputCode = DP049[10]; break;
          case 151331 : inputName = DP050[1]; inputAir = DP050[2]; inputEnergy = DP050[3]; inputProtein = DP050[4]; inputLemak = DP050[5]; inputKarbo = DP050[6]; inputSerat = DP050[7]; inputCarbing = DP050[8]; inputType = DP050[9]; inputCode = DP050[10]; break;
          case 151332 : inputName = DP051[1]; inputAir = DP051[2]; inputEnergy = DP051[3]; inputProtein = DP051[4]; inputLemak = DP051[5]; inputKarbo = DP051[6]; inputSerat = DP051[7]; inputCarbing = DP051[8]; inputType = DP051[9]; inputCode = DP051[10]; break;
          case 151333 : inputName = DP052[1]; inputAir = DP052[2]; inputEnergy = DP052[3]; inputProtein = DP052[4]; inputLemak = DP052[5]; inputKarbo = DP052[6]; inputSerat = DP052[7]; inputCarbing = DP052[8]; inputType = DP052[9]; inputCode = DP052[10]; break;
          case 151334 : inputName = DP053[1]; inputAir = DP053[2]; inputEnergy = DP053[3]; inputProtein = DP053[4]; inputLemak = DP053[5]; inputKarbo = DP053[6]; inputSerat = DP053[7]; inputCarbing = DP053[8]; inputType = DP053[9]; inputCode = DP053[10]; break;
          case 151335 : inputName = DP054[1]; inputAir = DP054[2]; inputEnergy = DP054[3]; inputProtein = DP054[4]; inputLemak = DP054[5]; inputKarbo = DP054[6]; inputSerat = DP054[7]; inputCarbing = DP054[8]; inputType = DP054[9]; inputCode = DP054[10]; break;
          case 151341 : inputName = DP055[1]; inputAir = DP055[2]; inputEnergy = DP055[3]; inputProtein = DP055[4]; inputLemak = DP055[5]; inputKarbo = DP055[6]; inputSerat = DP055[7]; inputCarbing = DP055[8]; inputType = DP055[9]; inputCode = DP055[10]; break;
          case 151342 : inputName = DP056[1]; inputAir = DP056[2]; inputEnergy = DP056[3]; inputProtein = DP056[4]; inputLemak = DP056[5]; inputKarbo = DP056[6]; inputSerat = DP056[7]; inputCarbing = DP056[8]; inputType = DP056[9]; inputCode = DP056[10]; break;
          case 151343 : inputName = DP057[1]; inputAir = DP057[2]; inputEnergy = DP057[3]; inputProtein = DP057[4]; inputLemak = DP057[5]; inputKarbo = DP057[6]; inputSerat = DP057[7]; inputCarbing = DP057[8]; inputType = DP057[9]; inputCode = DP057[10]; break;
          case 151344 : inputName = DP058[1]; inputAir = DP058[2]; inputEnergy = DP058[3]; inputProtein = DP058[4]; inputLemak = DP058[5]; inputKarbo = DP058[6]; inputSerat = DP058[7]; inputCarbing = DP058[8]; inputType = DP058[9]; inputCode = DP058[10]; break;
          case 151345 : inputName = DP059[1]; inputAir = DP059[2]; inputEnergy = DP059[3]; inputProtein = DP059[4]; inputLemak = DP059[5]; inputKarbo = DP059[6]; inputSerat = DP059[7]; inputCarbing = DP059[8]; inputType = DP059[9]; inputCode = DP059[10]; break;
          case 151351 : inputName = DP060[1]; inputAir = DP060[2]; inputEnergy = DP060[3]; inputProtein = DP060[4]; inputLemak = DP060[5]; inputKarbo = DP060[6]; inputSerat = DP060[7]; inputCarbing = DP060[8]; inputType = DP060[9]; inputCode = DP060[10]; break;
          case 151352 : inputName = DP061[1]; inputAir = DP061[2]; inputEnergy = DP061[3]; inputProtein = DP061[4]; inputLemak = DP061[5]; inputKarbo = DP061[6]; inputSerat = DP061[7]; inputCarbing = DP061[8]; inputType = DP061[9]; inputCode = DP061[10]; break;
          case 111 : inputName = ER001[1]; inputAir = ER001[2]; inputEnergy = ER001[3]; inputProtein = ER001[4]; inputLemak = ER001[5]; inputKarbo = ER001[6]; inputSerat = ER001[7]; inputCarbing = ER001[8]; inputType = ER001[9]; inputCode = ER001[10]; break;
          case 112 : inputName = ER002[1]; inputAir = ER002[2]; inputEnergy = ER002[3]; inputProtein = ER002[4]; inputLemak = ER002[5]; inputKarbo = ER002[6]; inputSerat = ER002[7]; inputCarbing = ER002[8]; inputType = ER002[9]; inputCode = ER002[10]; break;
          case 113 : inputName = ER003[1]; inputAir = ER003[2]; inputEnergy = ER003[3]; inputProtein = ER003[4]; inputLemak = ER003[5]; inputKarbo = ER003[6]; inputSerat = ER003[7]; inputCarbing = ER003[8]; inputType = ER003[9]; inputCode = ER003[10]; break;
          case 114 : inputName = ER004[1]; inputAir = ER004[2]; inputEnergy = ER004[3]; inputProtein = ER004[4]; inputLemak = ER004[5]; inputKarbo = ER004[6]; inputSerat = ER004[7]; inputCarbing = ER004[8]; inputType = ER004[9]; inputCode = ER004[10]; break;
          case 115 : inputName = ER005[1]; inputAir = ER005[2]; inputEnergy = ER005[3]; inputProtein = ER005[4]; inputLemak = ER005[5]; inputKarbo = ER005[6]; inputSerat = ER005[7]; inputCarbing = ER005[8]; inputType = ER005[9]; inputCode = ER005[10]; break;
          case 1111 : inputName = ER006[1]; inputAir = ER006[2]; inputEnergy = ER006[3]; inputProtein = ER006[4]; inputLemak = ER006[5]; inputKarbo = ER006[6]; inputSerat = ER006[7]; inputCarbing = ER006[8]; inputType = ER006[9]; inputCode = ER006[10]; break;
          case 1112 : inputName = ER007[1]; inputAir = ER007[2]; inputEnergy = ER007[3]; inputProtein = ER007[4]; inputLemak = ER007[5]; inputKarbo = ER007[6]; inputSerat = ER007[7]; inputCarbing = ER007[8]; inputType = ER007[9]; inputCode = ER007[10]; break;
          case 1113 : inputName = ER008[1]; inputAir = ER008[2]; inputEnergy = ER008[3]; inputProtein = ER008[4]; inputLemak = ER008[5]; inputKarbo = ER008[6]; inputSerat = ER008[7]; inputCarbing = ER008[8]; inputType = ER008[9]; inputCode = ER008[10]; break;
          case 1114 : inputName = ER009[1]; inputAir = ER009[2]; inputEnergy = ER009[3]; inputProtein = ER009[4]; inputLemak = ER009[5]; inputKarbo = ER009[6]; inputSerat = ER009[7]; inputCarbing = ER009[8]; inputType = ER009[9]; inputCode = ER009[10]; break;
          case 1115 : inputName = ER010[1]; inputAir = ER010[2]; inputEnergy = ER010[3]; inputProtein = ER010[4]; inputLemak = ER010[5]; inputKarbo = ER010[6]; inputSerat = ER010[7]; inputCarbing = ER010[8]; inputType = ER010[9]; inputCode = ER010[10]; break;
          case 1121 : inputName = ER011[1]; inputAir = ER011[2]; inputEnergy = ER011[3]; inputProtein = ER011[4]; inputLemak = ER011[5]; inputKarbo = ER011[6]; inputSerat = ER011[7]; inputCarbing = ER011[8]; inputType = ER011[9]; inputCode = ER011[10]; break;
          case 1122 : inputName = ER012[1]; inputAir = ER012[2]; inputEnergy = ER012[3]; inputProtein = ER012[4]; inputLemak = ER012[5]; inputKarbo = ER012[6]; inputSerat = ER012[7]; inputCarbing = ER012[8]; inputType = ER012[9]; inputCode = ER012[10]; break;
          case 1123 : inputName = ER013[1]; inputAir = ER013[2]; inputEnergy = ER013[3]; inputProtein = ER013[4]; inputLemak = ER013[5]; inputKarbo = ER013[6]; inputSerat = ER013[7]; inputCarbing = ER013[8]; inputType = ER013[9]; inputCode = ER013[10]; break;
          case 1124 : inputName = ER014[1]; inputAir = ER014[2]; inputEnergy = ER014[3]; inputProtein = ER014[4]; inputLemak = ER014[5]; inputKarbo = ER014[6]; inputSerat = ER014[7]; inputCarbing = ER014[8]; inputType = ER014[9]; inputCode = ER014[10]; break;
          case 1125 : inputName = ER015[1]; inputAir = ER015[2]; inputEnergy = ER015[3]; inputProtein = ER015[4]; inputLemak = ER015[5]; inputKarbo = ER015[6]; inputSerat = ER015[7]; inputCarbing = ER015[8]; inputType = ER015[9]; inputCode = ER015[10]; break;
          case 1131 : inputName = ER016[1]; inputAir = ER016[2]; inputEnergy = ER016[3]; inputProtein = ER016[4]; inputLemak = ER016[5]; inputKarbo = ER016[6]; inputSerat = ER016[7]; inputCarbing = ER016[8]; inputType = ER016[9]; inputCode = ER016[10]; break;
          case 1132 : inputName = ER017[1]; inputAir = ER017[2]; inputEnergy = ER017[3]; inputProtein = ER017[4]; inputLemak = ER017[5]; inputKarbo = ER017[6]; inputSerat = ER017[7]; inputCarbing = ER017[8]; inputType = ER017[9]; inputCode = ER017[10]; break;
          case 1133 : inputName = ER018[1]; inputAir = ER018[2]; inputEnergy = ER018[3]; inputProtein = ER018[4]; inputLemak = ER018[5]; inputKarbo = ER018[6]; inputSerat = ER018[7]; inputCarbing = ER018[8]; inputType = ER018[9]; inputCode = ER018[10]; break;
          case 1134 : inputName = ER019[1]; inputAir = ER019[2]; inputEnergy = ER019[3]; inputProtein = ER019[4]; inputLemak = ER019[5]; inputKarbo = ER019[6]; inputSerat = ER019[7]; inputCarbing = ER019[8]; inputType = ER019[9]; inputCode = ER019[10]; break;
          case 1135 : inputName = ER020[1]; inputAir = ER020[2]; inputEnergy = ER020[3]; inputProtein = ER020[4]; inputLemak = ER020[5]; inputKarbo = ER020[6]; inputSerat = ER020[7]; inputCarbing = ER020[8]; inputType = ER020[9]; inputCode = ER020[10]; break;
          case 1141 : inputName = ER021[1]; inputAir = ER021[2]; inputEnergy = ER021[3]; inputProtein = ER021[4]; inputLemak = ER021[5]; inputKarbo = ER021[6]; inputSerat = ER021[7]; inputCarbing = ER021[8]; inputType = ER021[9]; inputCode = ER021[10]; break;
          case 1142 : inputName = ER022[1]; inputAir = ER022[2]; inputEnergy = ER022[3]; inputProtein = ER022[4]; inputLemak = ER022[5]; inputKarbo = ER022[6]; inputSerat = ER022[7]; inputCarbing = ER022[8]; inputType = ER022[9]; inputCode = ER022[10]; break;
          case 1143 : inputName = ER023[1]; inputAir = ER023[2]; inputEnergy = ER023[3]; inputProtein = ER023[4]; inputLemak = ER023[5]; inputKarbo = ER023[6]; inputSerat = ER023[7]; inputCarbing = ER023[8]; inputType = ER023[9]; inputCode = ER023[10]; break;
          case 1144 : inputName = ER024[1]; inputAir = ER024[2]; inputEnergy = ER024[3]; inputProtein = ER024[4]; inputLemak = ER024[5]; inputKarbo = ER024[6]; inputSerat = ER024[7]; inputCarbing = ER024[8]; inputType = ER024[9]; inputCode = ER024[10]; break;
          case 1145 : inputName = ER025[1]; inputAir = ER025[2]; inputEnergy = ER025[3]; inputProtein = ER025[4]; inputLemak = ER025[5]; inputKarbo = ER025[6]; inputSerat = ER025[7]; inputCarbing = ER025[8]; inputType = ER025[9]; inputCode = ER025[10]; break;
          case 1151 : inputName = ER026[1]; inputAir = ER026[2]; inputEnergy = ER026[3]; inputProtein = ER026[4]; inputLemak = ER026[5]; inputKarbo = ER026[6]; inputSerat = ER026[7]; inputCarbing = ER026[8]; inputType = ER026[9]; inputCode = ER026[10]; break;
          case 1152 : inputName = ER027[1]; inputAir = ER027[2]; inputEnergy = ER027[3]; inputProtein = ER027[4]; inputLemak = ER027[5]; inputKarbo = ER027[6]; inputSerat = ER027[7]; inputCarbing = ER027[8]; inputType = ER027[9]; inputCode = ER027[10]; break;
          case 1153 : inputName = ER028[1]; inputAir = ER028[2]; inputEnergy = ER028[3]; inputProtein = ER028[4]; inputLemak = ER028[5]; inputKarbo = ER028[6]; inputSerat = ER028[7]; inputCarbing = ER028[8]; inputType = ER028[9]; inputCode = ER028[10]; break;
          case 1154 : inputName = ER029[1]; inputAir = ER029[2]; inputEnergy = ER029[3]; inputProtein = ER029[4]; inputLemak = ER029[5]; inputKarbo = ER029[6]; inputSerat = ER029[7]; inputCarbing = ER029[8]; inputType = ER029[9]; inputCode = ER029[10]; break;
          case 1155 : inputName = ER030[1]; inputAir = ER030[2]; inputEnergy = ER030[3]; inputProtein = ER030[4]; inputLemak = ER030[5]; inputKarbo = ER030[6]; inputSerat = ER030[7]; inputCarbing = ER030[8]; inputType = ER030[9]; inputCode = ER030[10]; break;
          case 11111 : inputName = ER031[1]; inputAir = ER031[2]; inputEnergy = ER031[3]; inputProtein = ER031[4]; inputLemak = ER031[5]; inputKarbo = ER031[6]; inputSerat = ER031[7]; inputCarbing = ER031[8]; inputType = ER031[9]; inputCode = ER031[10]; break;
          case 11112 : inputName = ER032[1]; inputAir = ER032[2]; inputEnergy = ER032[3]; inputProtein = ER032[4]; inputLemak = ER032[5]; inputKarbo = ER032[6]; inputSerat = ER032[7]; inputCarbing = ER032[8]; inputType = ER032[9]; inputCode = ER032[10]; break;
          case 11113 : inputName = ER033[1]; inputAir = ER033[2]; inputEnergy = ER033[3]; inputProtein = ER033[4]; inputLemak = ER033[5]; inputKarbo = ER033[6]; inputSerat = ER033[7]; inputCarbing = ER033[8]; inputType = ER033[9]; inputCode = ER033[10]; break;
          case 11114 : inputName = ER034[1]; inputAir = ER034[2]; inputEnergy = ER034[3]; inputProtein = ER034[4]; inputLemak = ER034[5]; inputKarbo = ER034[6]; inputSerat = ER034[7]; inputCarbing = ER034[8]; inputType = ER034[9]; inputCode = ER034[10]; break;
          case 11115 : inputName = ER035[1]; inputAir = ER035[2]; inputEnergy = ER035[3]; inputProtein = ER035[4]; inputLemak = ER035[5]; inputKarbo = ER035[6]; inputSerat = ER035[7]; inputCarbing = ER035[8]; inputType = ER035[9]; inputCode = ER035[10]; break;
          case 11121 : inputName = ER036[1]; inputAir = ER036[2]; inputEnergy = ER036[3]; inputProtein = ER036[4]; inputLemak = ER036[5]; inputKarbo = ER036[6]; inputSerat = ER036[7]; inputCarbing = ER036[8]; inputType = ER036[9]; inputCode = ER036[10]; break;
          case 11122 : inputName = ER037[1]; inputAir = ER037[2]; inputEnergy = ER037[3]; inputProtein = ER037[4]; inputLemak = ER037[5]; inputKarbo = ER037[6]; inputSerat = ER037[7]; inputCarbing = ER037[8]; inputType = ER037[9]; inputCode = ER037[10]; break;
          case 11123 : inputName = ER038[1]; inputAir = ER038[2]; inputEnergy = ER038[3]; inputProtein = ER038[4]; inputLemak = ER038[5]; inputKarbo = ER038[6]; inputSerat = ER038[7]; inputCarbing = ER038[8]; inputType = ER038[9]; inputCode = ER038[10]; break;
          case 11124 : inputName = ER039[1]; inputAir = ER039[2]; inputEnergy = ER039[3]; inputProtein = ER039[4]; inputLemak = ER039[5]; inputKarbo = ER039[6]; inputSerat = ER039[7]; inputCarbing = ER039[8]; inputType = ER039[9]; inputCode = ER039[10]; break;
          case 11125 : inputName = ER040[1]; inputAir = ER040[2]; inputEnergy = ER040[3]; inputProtein = ER040[4]; inputLemak = ER040[5]; inputKarbo = ER040[6]; inputSerat = ER040[7]; inputCarbing = ER040[8]; inputType = ER040[9]; inputCode = ER040[10]; break;
          case 11131 : inputName = ER041[1]; inputAir = ER041[2]; inputEnergy = ER041[3]; inputProtein = ER041[4]; inputLemak = ER041[5]; inputKarbo = ER041[6]; inputSerat = ER041[7]; inputCarbing = ER041[8]; inputType = ER041[9]; inputCode = ER041[10]; break;
          case 11132 : inputName = ER042[1]; inputAir = ER042[2]; inputEnergy = ER042[3]; inputProtein = ER042[4]; inputLemak = ER042[5]; inputKarbo = ER042[6]; inputSerat = ER042[7]; inputCarbing = ER042[8]; inputType = ER042[9]; inputCode = ER042[10]; break;
          case 11133 : inputName = ER043[1]; inputAir = ER043[2]; inputEnergy = ER043[3]; inputProtein = ER043[4]; inputLemak = ER043[5]; inputKarbo = ER043[6]; inputSerat = ER043[7]; inputCarbing = ER043[8]; inputType = ER043[9]; inputCode = ER043[10]; break;
          case 11134 : inputName = ER044[1]; inputAir = ER044[2]; inputEnergy = ER044[3]; inputProtein = ER044[4]; inputLemak = ER044[5]; inputKarbo = ER044[6]; inputSerat = ER044[7]; inputCarbing = ER044[8]; inputType = ER044[9]; inputCode = ER044[10]; break;
          case 11135 : inputName = ER045[1]; inputAir = ER045[2]; inputEnergy = ER045[3]; inputProtein = ER045[4]; inputLemak = ER045[5]; inputKarbo = ER045[6]; inputSerat = ER045[7]; inputCarbing = ER045[8]; inputType = ER045[9]; inputCode = ER045[10]; break;
          case 11141 : inputName = ER046[1]; inputAir = ER046[2]; inputEnergy = ER046[3]; inputProtein = ER046[4]; inputLemak = ER046[5]; inputKarbo = ER046[6]; inputSerat = ER046[7]; inputCarbing = ER046[8]; inputType = ER046[9]; inputCode = ER046[10]; break;
          case 11142 : inputName = ER047[1]; inputAir = ER047[2]; inputEnergy = ER047[3]; inputProtein = ER047[4]; inputLemak = ER047[5]; inputKarbo = ER047[6]; inputSerat = ER047[7]; inputCarbing = ER047[8]; inputType = ER047[9]; inputCode = ER047[10]; break;
          case 11143 : inputName = ER048[1]; inputAir = ER048[2]; inputEnergy = ER048[3]; inputProtein = ER048[4]; inputLemak = ER048[5]; inputKarbo = ER048[6]; inputSerat = ER048[7]; inputCarbing = ER048[8]; inputType = ER048[9]; inputCode = ER048[10]; break;
          case 11144 : inputName = ER049[1]; inputAir = ER049[2]; inputEnergy = ER049[3]; inputProtein = ER049[4]; inputLemak = ER049[5]; inputKarbo = ER049[6]; inputSerat = ER049[7]; inputCarbing = ER049[8]; inputType = ER049[9]; inputCode = ER049[10]; break;
          case 11145 : inputName = ER050[1]; inputAir = ER050[2]; inputEnergy = ER050[3]; inputProtein = ER050[4]; inputLemak = ER050[5]; inputKarbo = ER050[6]; inputSerat = ER050[7]; inputCarbing = ER050[8]; inputType = ER050[9]; inputCode = ER050[10]; break;
          case 11151 : inputName = ER051[1]; inputAir = ER051[2]; inputEnergy = ER051[3]; inputProtein = ER051[4]; inputLemak = ER051[5]; inputKarbo = ER051[6]; inputSerat = ER051[7]; inputCarbing = ER051[8]; inputType = ER051[9]; inputCode = ER051[10]; break;
          case 11152 : inputName = ER052[1]; inputAir = ER052[2]; inputEnergy = ER052[3]; inputProtein = ER052[4]; inputLemak = ER052[5]; inputKarbo = ER052[6]; inputSerat = ER052[7]; inputCarbing = ER052[8]; inputType = ER052[9]; inputCode = ER052[10]; break;
          case 11153 : inputName = ER053[1]; inputAir = ER053[2]; inputEnergy = ER053[3]; inputProtein = ER053[4]; inputLemak = ER053[5]; inputKarbo = ER053[6]; inputSerat = ER053[7]; inputCarbing = ER053[8]; inputType = ER053[9]; inputCode = ER053[10]; break;
          case 11154 : inputName = ER054[1]; inputAir = ER054[2]; inputEnergy = ER054[3]; inputProtein = ER054[4]; inputLemak = ER054[5]; inputKarbo = ER054[6]; inputSerat = ER054[7]; inputCarbing = ER054[8]; inputType = ER054[9]; inputCode = ER054[10]; break;
          case 11155 : inputName = ER055[1]; inputAir = ER055[2]; inputEnergy = ER055[3]; inputProtein = ER055[4]; inputLemak = ER055[5]; inputKarbo = ER055[6]; inputSerat = ER055[7]; inputCarbing = ER055[8]; inputType = ER055[9]; inputCode = ER055[10]; break;
          case 11211 : inputName = ER056[1]; inputAir = ER056[2]; inputEnergy = ER056[3]; inputProtein = ER056[4]; inputLemak = ER056[5]; inputKarbo = ER056[6]; inputSerat = ER056[7]; inputCarbing = ER056[8]; inputType = ER056[9]; inputCode = ER056[10]; break;
          case 11212 : inputName = ER057[1]; inputAir = ER057[2]; inputEnergy = ER057[3]; inputProtein = ER057[4]; inputLemak = ER057[5]; inputKarbo = ER057[6]; inputSerat = ER057[7]; inputCarbing = ER057[8]; inputType = ER057[9]; inputCode = ER057[10]; break;
          case 11213 : inputName = ER058[1]; inputAir = ER058[2]; inputEnergy = ER058[3]; inputProtein = ER058[4]; inputLemak = ER058[5]; inputKarbo = ER058[6]; inputSerat = ER058[7]; inputCarbing = ER058[8]; inputType = ER058[9]; inputCode = ER058[10]; break;
          case 11214 : inputName = ER059[1]; inputAir = ER059[2]; inputEnergy = ER059[3]; inputProtein = ER059[4]; inputLemak = ER059[5]; inputKarbo = ER059[6]; inputSerat = ER059[7]; inputCarbing = ER059[8]; inputType = ER059[9]; inputCode = ER059[10]; break;
          case 11215 : inputName = ER060[1]; inputAir = ER060[2]; inputEnergy = ER060[3]; inputProtein = ER060[4]; inputLemak = ER060[5]; inputKarbo = ER060[6]; inputSerat = ER060[7]; inputCarbing = ER060[8]; inputType = ER060[9]; inputCode = ER060[10]; break;
          case 11221 : inputName = ER061[1]; inputAir = ER061[2]; inputEnergy = ER061[3]; inputProtein = ER061[4]; inputLemak = ER061[5]; inputKarbo = ER061[6]; inputSerat = ER061[7]; inputCarbing = ER061[8]; inputType = ER061[9]; inputCode = ER061[10]; break;
          case 11222 : inputName = ER062[1]; inputAir = ER062[2]; inputEnergy = ER062[3]; inputProtein = ER062[4]; inputLemak = ER062[5]; inputKarbo = ER062[6]; inputSerat = ER062[7]; inputCarbing = ER062[8]; inputType = ER062[9]; inputCode = ER062[10]; break;
          case 11223 : inputName = ER063[1]; inputAir = ER063[2]; inputEnergy = ER063[3]; inputProtein = ER063[4]; inputLemak = ER063[5]; inputKarbo = ER063[6]; inputSerat = ER063[7]; inputCarbing = ER063[8]; inputType = ER063[9]; inputCode = ER063[10]; break;
          case 11224 : inputName = ER064[1]; inputAir = ER064[2]; inputEnergy = ER064[3]; inputProtein = ER064[4]; inputLemak = ER064[5]; inputKarbo = ER064[6]; inputSerat = ER064[7]; inputCarbing = ER064[8]; inputType = ER064[9]; inputCode = ER064[10]; break;
          case 11225 : inputName = ER065[1]; inputAir = ER065[2]; inputEnergy = ER065[3]; inputProtein = ER065[4]; inputLemak = ER065[5]; inputKarbo = ER065[6]; inputSerat = ER065[7]; inputCarbing = ER065[8]; inputType = ER065[9]; inputCode = ER065[10]; break;
          case 11231 : inputName = ER066[1]; inputAir = ER066[2]; inputEnergy = ER066[3]; inputProtein = ER066[4]; inputLemak = ER066[5]; inputKarbo = ER066[6]; inputSerat = ER066[7]; inputCarbing = ER066[8]; inputType = ER066[9]; inputCode = ER066[10]; break;
          case 11232 : inputName = ER067[1]; inputAir = ER067[2]; inputEnergy = ER067[3]; inputProtein = ER067[4]; inputLemak = ER067[5]; inputKarbo = ER067[6]; inputSerat = ER067[7]; inputCarbing = ER067[8]; inputType = ER067[9]; inputCode = ER067[10]; break;
          case 11233 : inputName = ER068[1]; inputAir = ER068[2]; inputEnergy = ER068[3]; inputProtein = ER068[4]; inputLemak = ER068[5]; inputKarbo = ER068[6]; inputSerat = ER068[7]; inputCarbing = ER068[8]; inputType = ER068[9]; inputCode = ER068[10]; break;
          case 11234 : inputName = ER069[1]; inputAir = ER069[2]; inputEnergy = ER069[3]; inputProtein = ER069[4]; inputLemak = ER069[5]; inputKarbo = ER069[6]; inputSerat = ER069[7]; inputCarbing = ER069[8]; inputType = ER069[9]; inputCode = ER069[10]; break;
          case 11235 : inputName = ER070[1]; inputAir = ER070[2]; inputEnergy = ER070[3]; inputProtein = ER070[4]; inputLemak = ER070[5]; inputKarbo = ER070[6]; inputSerat = ER070[7]; inputCarbing = ER070[8]; inputType = ER070[9]; inputCode = ER070[10]; break;
          case 11241 : inputName = ER071[1]; inputAir = ER071[2]; inputEnergy = ER071[3]; inputProtein = ER071[4]; inputLemak = ER071[5]; inputKarbo = ER071[6]; inputSerat = ER071[7]; inputCarbing = ER071[8]; inputType = ER071[9]; inputCode = ER071[10]; break;
          case 11242 : inputName = ER072[1]; inputAir = ER072[2]; inputEnergy = ER072[3]; inputProtein = ER072[4]; inputLemak = ER072[5]; inputKarbo = ER072[6]; inputSerat = ER072[7]; inputCarbing = ER072[8]; inputType = ER072[9]; inputCode = ER072[10]; break;
          case 11243 : inputName = ER073[1]; inputAir = ER073[2]; inputEnergy = ER073[3]; inputProtein = ER073[4]; inputLemak = ER073[5]; inputKarbo = ER073[6]; inputSerat = ER073[7]; inputCarbing = ER073[8]; inputType = ER073[9]; inputCode = ER073[10]; break;
          case 11244 : inputName = ER074[1]; inputAir = ER074[2]; inputEnergy = ER074[3]; inputProtein = ER074[4]; inputLemak = ER074[5]; inputKarbo = ER074[6]; inputSerat = ER074[7]; inputCarbing = ER074[8]; inputType = ER074[9]; inputCode = ER074[10]; break;
          case 11245 : inputName = ER075[1]; inputAir = ER075[2]; inputEnergy = ER075[3]; inputProtein = ER075[4]; inputLemak = ER075[5]; inputKarbo = ER075[6]; inputSerat = ER075[7]; inputCarbing = ER075[8]; inputType = ER075[9]; inputCode = ER075[10]; break;
          case 11251 : inputName = ER076[1]; inputAir = ER076[2]; inputEnergy = ER076[3]; inputProtein = ER076[4]; inputLemak = ER076[5]; inputKarbo = ER076[6]; inputSerat = ER076[7]; inputCarbing = ER076[8]; inputType = ER076[9]; inputCode = ER076[10]; break;
          case 11252 : inputName = ER077[1]; inputAir = ER077[2]; inputEnergy = ER077[3]; inputProtein = ER077[4]; inputLemak = ER077[5]; inputKarbo = ER077[6]; inputSerat = ER077[7]; inputCarbing = ER077[8]; inputType = ER077[9]; inputCode = ER077[10]; break;
          case 11253 : inputName = ER078[1]; inputAir = ER078[2]; inputEnergy = ER078[3]; inputProtein = ER078[4]; inputLemak = ER078[5]; inputKarbo = ER078[6]; inputSerat = ER078[7]; inputCarbing = ER078[8]; inputType = ER078[9]; inputCode = ER078[10]; break;
          case 11254 : inputName = ER079[1]; inputAir = ER079[2]; inputEnergy = ER079[3]; inputProtein = ER079[4]; inputLemak = ER079[5]; inputKarbo = ER079[6]; inputSerat = ER079[7]; inputCarbing = ER079[8]; inputType = ER079[9]; inputCode = ER079[10]; break;
          case 11255 : inputName = ER080[1]; inputAir = ER080[2]; inputEnergy = ER080[3]; inputProtein = ER080[4]; inputLemak = ER080[5]; inputKarbo = ER080[6]; inputSerat = ER080[7]; inputCarbing = ER080[8]; inputType = ER080[9]; inputCode = ER080[10]; break;
          case 11311 : inputName = ER081[1]; inputAir = ER081[2]; inputEnergy = ER081[3]; inputProtein = ER081[4]; inputLemak = ER081[5]; inputKarbo = ER081[6]; inputSerat = ER081[7]; inputCarbing = ER081[8]; inputType = ER081[9]; inputCode = ER081[10]; break;
          case 11312 : inputName = ER082[1]; inputAir = ER082[2]; inputEnergy = ER082[3]; inputProtein = ER082[4]; inputLemak = ER082[5]; inputKarbo = ER082[6]; inputSerat = ER082[7]; inputCarbing = ER082[8]; inputType = ER082[9]; inputCode = ER082[10]; break;
          case 11313 : inputName = ER083[1]; inputAir = ER083[2]; inputEnergy = ER083[3]; inputProtein = ER083[4]; inputLemak = ER083[5]; inputKarbo = ER083[6]; inputSerat = ER083[7]; inputCarbing = ER083[8]; inputType = ER083[9]; inputCode = ER083[10]; break;
          case 11314 : inputName = ER084[1]; inputAir = ER084[2]; inputEnergy = ER084[3]; inputProtein = ER084[4]; inputLemak = ER084[5]; inputKarbo = ER084[6]; inputSerat = ER084[7]; inputCarbing = ER084[8]; inputType = ER084[9]; inputCode = ER084[10]; break;
          case 11315 : inputName = ER085[1]; inputAir = ER085[2]; inputEnergy = ER085[3]; inputProtein = ER085[4]; inputLemak = ER085[5]; inputKarbo = ER085[6]; inputSerat = ER085[7]; inputCarbing = ER085[8]; inputType = ER085[9]; inputCode = ER085[10]; break;
          case 11321 : inputName = ER086[1]; inputAir = ER086[2]; inputEnergy = ER086[3]; inputProtein = ER086[4]; inputLemak = ER086[5]; inputKarbo = ER086[6]; inputSerat = ER086[7]; inputCarbing = ER086[8]; inputType = ER086[9]; inputCode = ER086[10]; break;
          case 11322 : inputName = ER087[1]; inputAir = ER087[2]; inputEnergy = ER087[3]; inputProtein = ER087[4]; inputLemak = ER087[5]; inputKarbo = ER087[6]; inputSerat = ER087[7]; inputCarbing = ER087[8]; inputType = ER087[9]; inputCode = ER087[10]; break;
          case 11323 : inputName = ER088[1]; inputAir = ER088[2]; inputEnergy = ER088[3]; inputProtein = ER088[4]; inputLemak = ER088[5]; inputKarbo = ER088[6]; inputSerat = ER088[7]; inputCarbing = ER088[8]; inputType = ER088[9]; inputCode = ER088[10]; break;
          case 11324 : inputName = ER089[1]; inputAir = ER089[2]; inputEnergy = ER089[3]; inputProtein = ER089[4]; inputLemak = ER089[5]; inputKarbo = ER089[6]; inputSerat = ER089[7]; inputCarbing = ER089[8]; inputType = ER089[9]; inputCode = ER089[10]; break;
          case 11325 : inputName = ER090[1]; inputAir = ER090[2]; inputEnergy = ER090[3]; inputProtein = ER090[4]; inputLemak = ER090[5]; inputKarbo = ER090[6]; inputSerat = ER090[7]; inputCarbing = ER090[8]; inputType = ER090[9]; inputCode = ER090[10]; break;
          case 11331 : inputName = ER091[1]; inputAir = ER091[2]; inputEnergy = ER091[3]; inputProtein = ER091[4]; inputLemak = ER091[5]; inputKarbo = ER091[6]; inputSerat = ER091[7]; inputCarbing = ER091[8]; inputType = ER091[9]; inputCode = ER091[10]; break;
          case 11332 : inputName = ER092[1]; inputAir = ER092[2]; inputEnergy = ER092[3]; inputProtein = ER092[4]; inputLemak = ER092[5]; inputKarbo = ER092[6]; inputSerat = ER092[7]; inputCarbing = ER092[8]; inputType = ER092[9]; inputCode = ER092[10]; break;
          case 11333 : inputName = ER093[1]; inputAir = ER093[2]; inputEnergy = ER093[3]; inputProtein = ER093[4]; inputLemak = ER093[5]; inputKarbo = ER093[6]; inputSerat = ER093[7]; inputCarbing = ER093[8]; inputType = ER093[9]; inputCode = ER093[10]; break;
          case 11334 : inputName = ER094[1]; inputAir = ER094[2]; inputEnergy = ER094[3]; inputProtein = ER094[4]; inputLemak = ER094[5]; inputKarbo = ER094[6]; inputSerat = ER094[7]; inputCarbing = ER094[8]; inputType = ER094[9]; inputCode = ER094[10]; break;
          case 11335 : inputName = ER095[1]; inputAir = ER095[2]; inputEnergy = ER095[3]; inputProtein = ER095[4]; inputLemak = ER095[5]; inputKarbo = ER095[6]; inputSerat = ER095[7]; inputCarbing = ER095[8]; inputType = ER095[9]; inputCode = ER095[10]; break;
          case 11341 : inputName = ER096[1]; inputAir = ER096[2]; inputEnergy = ER096[3]; inputProtein = ER096[4]; inputLemak = ER096[5]; inputKarbo = ER096[6]; inputSerat = ER096[7]; inputCarbing = ER096[8]; inputType = ER096[9]; inputCode = ER096[10]; break;
          case 11342 : inputName = ER097[1]; inputAir = ER097[2]; inputEnergy = ER097[3]; inputProtein = ER097[4]; inputLemak = ER097[5]; inputKarbo = ER097[6]; inputSerat = ER097[7]; inputCarbing = ER097[8]; inputType = ER097[9]; inputCode = ER097[10]; break;
          case 11343 : inputName = ER098[1]; inputAir = ER098[2]; inputEnergy = ER098[3]; inputProtein = ER098[4]; inputLemak = ER098[5]; inputKarbo = ER098[6]; inputSerat = ER098[7]; inputCarbing = ER098[8]; inputType = ER098[9]; inputCode = ER098[10]; break;
          case 11344 : inputName = ER099[1]; inputAir = ER099[2]; inputEnergy = ER099[3]; inputProtein = ER099[4]; inputLemak = ER099[5]; inputKarbo = ER099[6]; inputSerat = ER099[7]; inputCarbing = ER099[8]; inputType = ER099[9]; inputCode = ER099[10]; break;
          case 11345 : inputName = ER100[1]; inputAir = ER100[2]; inputEnergy = ER100[3]; inputProtein = ER100[4]; inputLemak = ER100[5]; inputKarbo = ER100[6]; inputSerat = ER100[7]; inputCarbing = ER100[8]; inputType = ER100[9]; inputCode = ER100[10]; break;
          case 11351 : inputName = ER101[1]; inputAir = ER101[2]; inputEnergy = ER101[3]; inputProtein = ER101[4]; inputLemak = ER101[5]; inputKarbo = ER101[6]; inputSerat = ER101[7]; inputCarbing = ER101[8]; inputType = ER101[9]; inputCode = ER101[10]; break;
          case 11352 : inputName = ER102[1]; inputAir = ER102[2]; inputEnergy = ER102[3]; inputProtein = ER102[4]; inputLemak = ER102[5]; inputKarbo = ER102[6]; inputSerat = ER102[7]; inputCarbing = ER102[8]; inputType = ER102[9]; inputCode = ER102[10]; break;
          case 11353 : inputName = ER103[1]; inputAir = ER103[2]; inputEnergy = ER103[3]; inputProtein = ER103[4]; inputLemak = ER103[5]; inputKarbo = ER103[6]; inputSerat = ER103[7]; inputCarbing = ER103[8]; inputType = ER103[9]; inputCode = ER103[10]; break;
          case 11354 : inputName = ER104[1]; inputAir = ER104[2]; inputEnergy = ER104[3]; inputProtein = ER104[4]; inputLemak = ER104[5]; inputKarbo = ER104[6]; inputSerat = ER104[7]; inputCarbing = ER104[8]; inputType = ER104[9]; inputCode = ER104[10]; break;
          case 11355 : inputName = ER105[1]; inputAir = ER105[2]; inputEnergy = ER105[3]; inputProtein = ER105[4]; inputLemak = ER105[5]; inputKarbo = ER105[6]; inputSerat = ER105[7]; inputCarbing = ER105[8]; inputType = ER105[9]; inputCode = ER105[10]; break;
          case 11411 : inputName = ER106[1]; inputAir = ER106[2]; inputEnergy = ER106[3]; inputProtein = ER106[4]; inputLemak = ER106[5]; inputKarbo = ER106[6]; inputSerat = ER106[7]; inputCarbing = ER106[8]; inputType = ER106[9]; inputCode = ER106[10]; break;
          case 11412 : inputName = ER107[1]; inputAir = ER107[2]; inputEnergy = ER107[3]; inputProtein = ER107[4]; inputLemak = ER107[5]; inputKarbo = ER107[6]; inputSerat = ER107[7]; inputCarbing = ER107[8]; inputType = ER107[9]; inputCode = ER107[10]; break;
          case 11413 : inputName = ER108[1]; inputAir = ER108[2]; inputEnergy = ER108[3]; inputProtein = ER108[4]; inputLemak = ER108[5]; inputKarbo = ER108[6]; inputSerat = ER108[7]; inputCarbing = ER108[8]; inputType = ER108[9]; inputCode = ER108[10]; break;
          case 11414 : inputName = ER109[1]; inputAir = ER109[2]; inputEnergy = ER109[3]; inputProtein = ER109[4]; inputLemak = ER109[5]; inputKarbo = ER109[6]; inputSerat = ER109[7]; inputCarbing = ER109[8]; inputType = ER109[9]; inputCode = ER109[10]; break;
          case 11415 : inputName = ER110[1]; inputAir = ER110[2]; inputEnergy = ER110[3]; inputProtein = ER110[4]; inputLemak = ER110[5]; inputKarbo = ER110[6]; inputSerat = ER110[7]; inputCarbing = ER110[8]; inputType = ER110[9]; inputCode = ER110[10]; break;
          case 11421 : inputName = ER111[1]; inputAir = ER111[2]; inputEnergy = ER111[3]; inputProtein = ER111[4]; inputLemak = ER111[5]; inputKarbo = ER111[6]; inputSerat = ER111[7]; inputCarbing = ER111[8]; inputType = ER111[9]; inputCode = ER111[10]; break;
          case 11422 : inputName = ER112[1]; inputAir = ER112[2]; inputEnergy = ER112[3]; inputProtein = ER112[4]; inputLemak = ER112[5]; inputKarbo = ER112[6]; inputSerat = ER112[7]; inputCarbing = ER112[8]; inputType = ER112[9]; inputCode = ER112[10]; break;
          case 11423 : inputName = EP001[1]; inputAir = EP001[2]; inputEnergy = EP001[3]; inputProtein = EP001[4]; inputLemak = EP001[5]; inputKarbo = EP001[6]; inputSerat = EP001[7]; inputCarbing = EP001[8]; inputType = EP001[9]; inputCode = EP001[10]; break;
          case 11424 : inputName = EP002[1]; inputAir = EP002[2]; inputEnergy = EP002[3]; inputProtein = EP002[4]; inputLemak = EP002[5]; inputKarbo = EP002[6]; inputSerat = EP002[7]; inputCarbing = EP002[8]; inputType = EP002[9]; inputCode = EP002[10]; break;
          case 11425 : inputName = EP003[1]; inputAir = EP003[2]; inputEnergy = EP003[3]; inputProtein = EP003[4]; inputLemak = EP003[5]; inputKarbo = EP003[6]; inputSerat = EP003[7]; inputCarbing = EP003[8]; inputType = EP003[9]; inputCode = EP003[10]; break;
          case 11431 : inputName = EP004[1]; inputAir = EP004[2]; inputEnergy = EP004[3]; inputProtein = EP004[4]; inputLemak = EP004[5]; inputKarbo = EP004[6]; inputSerat = EP004[7]; inputCarbing = EP004[8]; inputType = EP004[9]; inputCode = EP004[10]; break;
          case 11432 : inputName = EP005[1]; inputAir = EP005[2]; inputEnergy = EP005[3]; inputProtein = EP005[4]; inputLemak = EP005[5]; inputKarbo = EP005[6]; inputSerat = EP005[7]; inputCarbing = EP005[8]; inputType = EP005[9]; inputCode = EP005[10]; break;
          case 11433 : inputName = EP006[1]; inputAir = EP006[2]; inputEnergy = EP006[3]; inputProtein = EP006[4]; inputLemak = EP006[5]; inputKarbo = EP006[6]; inputSerat = EP006[7]; inputCarbing = EP006[8]; inputType = EP006[9]; inputCode = EP006[10]; break;
          case 11434 : inputName = EP007[1]; inputAir = EP007[2]; inputEnergy = EP007[3]; inputProtein = EP007[4]; inputLemak = EP007[5]; inputKarbo = EP007[6]; inputSerat = EP007[7]; inputCarbing = EP007[8]; inputType = EP007[9]; inputCode = EP007[10]; break;
          case 11435 : inputName = EP008[1]; inputAir = EP008[2]; inputEnergy = EP008[3]; inputProtein = EP008[4]; inputLemak = EP008[5]; inputKarbo = EP008[6]; inputSerat = EP008[7]; inputCarbing = EP008[8]; inputType = EP008[9]; inputCode = EP008[10]; break;
          case 11441 : inputName = EP009[1]; inputAir = EP009[2]; inputEnergy = EP009[3]; inputProtein = EP009[4]; inputLemak = EP009[5]; inputKarbo = EP009[6]; inputSerat = EP009[7]; inputCarbing = EP009[8]; inputType = EP009[9]; inputCode = EP009[10]; break;
          case 11442 : inputName = EP010[1]; inputAir = EP010[2]; inputEnergy = EP010[3]; inputProtein = EP010[4]; inputLemak = EP010[5]; inputKarbo = EP010[6]; inputSerat = EP010[7]; inputCarbing = EP010[8]; inputType = EP010[9]; inputCode = EP010[10]; break;
          case 11443 : inputName = EP011[1]; inputAir = EP011[2]; inputEnergy = EP011[3]; inputProtein = EP011[4]; inputLemak = EP011[5]; inputKarbo = EP011[6]; inputSerat = EP011[7]; inputCarbing = EP011[8]; inputType = EP011[9]; inputCode = EP011[10]; break;
          case 11444 : inputName = EP012[1]; inputAir = EP012[2]; inputEnergy = EP012[3]; inputProtein = EP012[4]; inputLemak = EP012[5]; inputKarbo = EP012[6]; inputSerat = EP012[7]; inputCarbing = EP012[8]; inputType = EP012[9]; inputCode = EP012[10]; break;
          case 11445 : inputName = EP013[1]; inputAir = EP013[2]; inputEnergy = EP013[3]; inputProtein = EP013[4]; inputLemak = EP013[5]; inputKarbo = EP013[6]; inputSerat = EP013[7]; inputCarbing = EP013[8]; inputType = EP013[9]; inputCode = EP013[10]; break;
          case 11451 : inputName = EP014[1]; inputAir = EP014[2]; inputEnergy = EP014[3]; inputProtein = EP014[4]; inputLemak = EP014[5]; inputKarbo = EP014[6]; inputSerat = EP014[7]; inputCarbing = EP014[8]; inputType = EP014[9]; inputCode = EP014[10]; break;
          case 11452 : inputName = EP015[1]; inputAir = EP015[2]; inputEnergy = EP015[3]; inputProtein = EP015[4]; inputLemak = EP015[5]; inputKarbo = EP015[6]; inputSerat = EP015[7]; inputCarbing = EP015[8]; inputType = EP015[9]; inputCode = EP015[10]; break;
          case 311 : inputName = FR001[1]; inputAir = FR001[2]; inputEnergy = FR001[3]; inputProtein = FR001[4]; inputLemak = FR001[5]; inputKarbo = FR001[6]; inputSerat = FR001[7]; inputCarbing = FR001[8]; inputType = FR001[9]; inputCode = FR001[10]; break;
          case 312 : inputName = FR002[1]; inputAir = FR002[2]; inputEnergy = FR002[3]; inputProtein = FR002[4]; inputLemak = FR002[5]; inputKarbo = FR002[6]; inputSerat = FR002[7]; inputCarbing = FR002[8]; inputType = FR002[9]; inputCode = FR002[10]; break;
          case 313 : inputName = FR003[1]; inputAir = FR003[2]; inputEnergy = FR003[3]; inputProtein = FR003[4]; inputLemak = FR003[5]; inputKarbo = FR003[6]; inputSerat = FR003[7]; inputCarbing = FR003[8]; inputType = FR003[9]; inputCode = FR003[10]; break;
          case 314 : inputName = FR004[1]; inputAir = FR004[2]; inputEnergy = FR004[3]; inputProtein = FR004[4]; inputLemak = FR004[5]; inputKarbo = FR004[6]; inputSerat = FR004[7]; inputCarbing = FR004[8]; inputType = FR004[9]; inputCode = FR004[10]; break;
          case 315 : inputName = FR005[1]; inputAir = FR005[2]; inputEnergy = FR005[3]; inputProtein = FR005[4]; inputLemak = FR005[5]; inputKarbo = FR005[6]; inputSerat = FR005[7]; inputCarbing = FR005[8]; inputType = FR005[9]; inputCode = FR005[10]; break;
          case 3111 : inputName = FR006[1]; inputAir = FR006[2]; inputEnergy = FR006[3]; inputProtein = FR006[4]; inputLemak = FR006[5]; inputKarbo = FR006[6]; inputSerat = FR006[7]; inputCarbing = FR006[8]; inputType = FR006[9]; inputCode = FR006[10]; break;
          case 3112 : inputName = FR007[1]; inputAir = FR007[2]; inputEnergy = FR007[3]; inputProtein = FR007[4]; inputLemak = FR007[5]; inputKarbo = FR007[6]; inputSerat = FR007[7]; inputCarbing = FR007[8]; inputType = FR007[9]; inputCode = FR007[10]; break;
          case 3113 : inputName = FR008[1]; inputAir = FR008[2]; inputEnergy = FR008[3]; inputProtein = FR008[4]; inputLemak = FR008[5]; inputKarbo = FR008[6]; inputSerat = FR008[7]; inputCarbing = FR008[8]; inputType = FR008[9]; inputCode = FR008[10]; break;
          case 3114 : inputName = FR009[1]; inputAir = FR009[2]; inputEnergy = FR009[3]; inputProtein = FR009[4]; inputLemak = FR009[5]; inputKarbo = FR009[6]; inputSerat = FR009[7]; inputCarbing = FR009[8]; inputType = FR009[9]; inputCode = FR009[10]; break;
          case 3115 : inputName = FR010[1]; inputAir = FR010[2]; inputEnergy = FR010[3]; inputProtein = FR010[4]; inputLemak = FR010[5]; inputKarbo = FR010[6]; inputSerat = FR010[7]; inputCarbing = FR010[8]; inputType = FR010[9]; inputCode = FR010[10]; break;
          case 3121 : inputName = FR011[1]; inputAir = FR011[2]; inputEnergy = FR011[3]; inputProtein = FR011[4]; inputLemak = FR011[5]; inputKarbo = FR011[6]; inputSerat = FR011[7]; inputCarbing = FR011[8]; inputType = FR011[9]; inputCode = FR011[10]; break;
          case 3122 : inputName = FR012[1]; inputAir = FR012[2]; inputEnergy = FR012[3]; inputProtein = FR012[4]; inputLemak = FR012[5]; inputKarbo = FR012[6]; inputSerat = FR012[7]; inputCarbing = FR012[8]; inputType = FR012[9]; inputCode = FR012[10]; break;
          case 3123 : inputName = FR013[1]; inputAir = FR013[2]; inputEnergy = FR013[3]; inputProtein = FR013[4]; inputLemak = FR013[5]; inputKarbo = FR013[6]; inputSerat = FR013[7]; inputCarbing = FR013[8]; inputType = FR013[9]; inputCode = FR013[10]; break;
          case 3124 : inputName = FR014[1]; inputAir = FR014[2]; inputEnergy = FR014[3]; inputProtein = FR014[4]; inputLemak = FR014[5]; inputKarbo = FR014[6]; inputSerat = FR014[7]; inputCarbing = FR014[8]; inputType = FR014[9]; inputCode = FR014[10]; break;
          case 3125 : inputName = FR015[1]; inputAir = FR015[2]; inputEnergy = FR015[3]; inputProtein = FR015[4]; inputLemak = FR015[5]; inputKarbo = FR015[6]; inputSerat = FR015[7]; inputCarbing = FR015[8]; inputType = FR015[9]; inputCode = FR015[10]; break;
          case 3131 : inputName = FR016[1]; inputAir = FR016[2]; inputEnergy = FR016[3]; inputProtein = FR016[4]; inputLemak = FR016[5]; inputKarbo = FR016[6]; inputSerat = FR016[7]; inputCarbing = FR016[8]; inputType = FR016[9]; inputCode = FR016[10]; break;
          case 3132 : inputName = FR017[1]; inputAir = FR017[2]; inputEnergy = FR017[3]; inputProtein = FR017[4]; inputLemak = FR017[5]; inputKarbo = FR017[6]; inputSerat = FR017[7]; inputCarbing = FR017[8]; inputType = FR017[9]; inputCode = FR017[10]; break;
          case 3133 : inputName = FR018[1]; inputAir = FR018[2]; inputEnergy = FR018[3]; inputProtein = FR018[4]; inputLemak = FR018[5]; inputKarbo = FR018[6]; inputSerat = FR018[7]; inputCarbing = FR018[8]; inputType = FR018[9]; inputCode = FR018[10]; break;
          case 3134 : inputName = FR019[1]; inputAir = FR019[2]; inputEnergy = FR019[3]; inputProtein = FR019[4]; inputLemak = FR019[5]; inputKarbo = FR019[6]; inputSerat = FR019[7]; inputCarbing = FR019[8]; inputType = FR019[9]; inputCode = FR019[10]; break;
          case 3135 : inputName = FR020[1]; inputAir = FR020[2]; inputEnergy = FR020[3]; inputProtein = FR020[4]; inputLemak = FR020[5]; inputKarbo = FR020[6]; inputSerat = FR020[7]; inputCarbing = FR020[8]; inputType = FR020[9]; inputCode = FR020[10]; break;
          case 3141 : inputName = FR021[1]; inputAir = FR021[2]; inputEnergy = FR021[3]; inputProtein = FR021[4]; inputLemak = FR021[5]; inputKarbo = FR021[6]; inputSerat = FR021[7]; inputCarbing = FR021[8]; inputType = FR021[9]; inputCode = FR021[10]; break;
          case 3142 : inputName = FR022[1]; inputAir = FR022[2]; inputEnergy = FR022[3]; inputProtein = FR022[4]; inputLemak = FR022[5]; inputKarbo = FR022[6]; inputSerat = FR022[7]; inputCarbing = FR022[8]; inputType = FR022[9]; inputCode = FR022[10]; break;
          case 3143 : inputName = FR023[1]; inputAir = FR023[2]; inputEnergy = FR023[3]; inputProtein = FR023[4]; inputLemak = FR023[5]; inputKarbo = FR023[6]; inputSerat = FR023[7]; inputCarbing = FR023[8]; inputType = FR023[9]; inputCode = FR023[10]; break;
          case 3144 : inputName = FR024[1]; inputAir = FR024[2]; inputEnergy = FR024[3]; inputProtein = FR024[4]; inputLemak = FR024[5]; inputKarbo = FR024[6]; inputSerat = FR024[7]; inputCarbing = FR024[8]; inputType = FR024[9]; inputCode = FR024[10]; break;
          case 3145 : inputName = FR025[1]; inputAir = FR025[2]; inputEnergy = FR025[3]; inputProtein = FR025[4]; inputLemak = FR025[5]; inputKarbo = FR025[6]; inputSerat = FR025[7]; inputCarbing = FR025[8]; inputType = FR025[9]; inputCode = FR025[10]; break;
          case 3151 : inputName = FR026[1]; inputAir = FR026[2]; inputEnergy = FR026[3]; inputProtein = FR026[4]; inputLemak = FR026[5]; inputKarbo = FR026[6]; inputSerat = FR026[7]; inputCarbing = FR026[8]; inputType = FR026[9]; inputCode = FR026[10]; break;
          case 3152 : inputName = FR027[1]; inputAir = FR027[2]; inputEnergy = FR027[3]; inputProtein = FR027[4]; inputLemak = FR027[5]; inputKarbo = FR027[6]; inputSerat = FR027[7]; inputCarbing = FR027[8]; inputType = FR027[9]; inputCode = FR027[10]; break;
          case 3153 : inputName = FR028[1]; inputAir = FR028[2]; inputEnergy = FR028[3]; inputProtein = FR028[4]; inputLemak = FR028[5]; inputKarbo = FR028[6]; inputSerat = FR028[7]; inputCarbing = FR028[8]; inputType = FR028[9]; inputCode = FR028[10]; break;
          case 3154 : inputName = FR029[1]; inputAir = FR029[2]; inputEnergy = FR029[3]; inputProtein = FR029[4]; inputLemak = FR029[5]; inputKarbo = FR029[6]; inputSerat = FR029[7]; inputCarbing = FR029[8]; inputType = FR029[9]; inputCode = FR029[10]; break;
          case 3155 : inputName = FR030[1]; inputAir = FR030[2]; inputEnergy = FR030[3]; inputProtein = FR030[4]; inputLemak = FR030[5]; inputKarbo = FR030[6]; inputSerat = FR030[7]; inputCarbing = FR030[8]; inputType = FR030[9]; inputCode = FR030[10]; break;
          case 31111 : inputName = FR031[1]; inputAir = FR031[2]; inputEnergy = FR031[3]; inputProtein = FR031[4]; inputLemak = FR031[5]; inputKarbo = FR031[6]; inputSerat = FR031[7]; inputCarbing = FR031[8]; inputType = FR031[9]; inputCode = FR031[10]; break;
          case 31112 : inputName = FR032[1]; inputAir = FR032[2]; inputEnergy = FR032[3]; inputProtein = FR032[4]; inputLemak = FR032[5]; inputKarbo = FR032[6]; inputSerat = FR032[7]; inputCarbing = FR032[8]; inputType = FR032[9]; inputCode = FR032[10]; break;
          case 31113 : inputName = FR033[1]; inputAir = FR033[2]; inputEnergy = FR033[3]; inputProtein = FR033[4]; inputLemak = FR033[5]; inputKarbo = FR033[6]; inputSerat = FR033[7]; inputCarbing = FR033[8]; inputType = FR033[9]; inputCode = FR033[10]; break;
          case 31114 : inputName = FR034[1]; inputAir = FR034[2]; inputEnergy = FR034[3]; inputProtein = FR034[4]; inputLemak = FR034[5]; inputKarbo = FR034[6]; inputSerat = FR034[7]; inputCarbing = FR034[8]; inputType = FR034[9]; inputCode = FR034[10]; break;
          case 31115 : inputName = FP001[1]; inputAir = FP001[2]; inputEnergy = FP001[3]; inputProtein = FP001[4]; inputLemak = FP001[5]; inputKarbo = FP001[6]; inputSerat = FP001[7]; inputCarbing = FP001[8]; inputType = FP001[9]; inputCode = FP001[10]; break;
          case 31121 : inputName = FP002[1]; inputAir = FP002[2]; inputEnergy = FP002[3]; inputProtein = FP002[4]; inputLemak = FP002[5]; inputKarbo = FP002[6]; inputSerat = FP002[7]; inputCarbing = FP002[8]; inputType = FP002[9]; inputCode = FP002[10]; break;
          case 31122 : inputName = FP003[1]; inputAir = FP003[2]; inputEnergy = FP003[3]; inputProtein = FP003[4]; inputLemak = FP003[5]; inputKarbo = FP003[6]; inputSerat = FP003[7]; inputCarbing = FP003[8]; inputType = FP003[9]; inputCode = FP003[10]; break;
          case 31123 : inputName = FP004[1]; inputAir = FP004[2]; inputEnergy = FP004[3]; inputProtein = FP004[4]; inputLemak = FP004[5]; inputKarbo = FP004[6]; inputSerat = FP004[7]; inputCarbing = FP004[8]; inputType = FP004[9]; inputCode = FP004[10]; break;
          case 31124 : inputName = FP005[1]; inputAir = FP005[2]; inputEnergy = FP005[3]; inputProtein = FP005[4]; inputLemak = FP005[5]; inputKarbo = FP005[6]; inputSerat = FP005[7]; inputCarbing = FP005[8]; inputType = FP005[9]; inputCode = FP005[10]; break;
          case 31125 : inputName = FP006[1]; inputAir = FP006[2]; inputEnergy = FP006[3]; inputProtein = FP006[4]; inputLemak = FP006[5]; inputKarbo = FP006[6]; inputSerat = FP006[7]; inputCarbing = FP006[8]; inputType = FP006[9]; inputCode = FP006[10]; break;
          case 31131 : inputName = FP007[1]; inputAir = FP007[2]; inputEnergy = FP007[3]; inputProtein = FP007[4]; inputLemak = FP007[5]; inputKarbo = FP007[6]; inputSerat = FP007[7]; inputCarbing = FP007[8]; inputType = FP007[9]; inputCode = FP007[10]; break;
          case 31132 : inputName = FP008[1]; inputAir = FP008[2]; inputEnergy = FP008[3]; inputProtein = FP008[4]; inputLemak = FP008[5]; inputKarbo = FP008[6]; inputSerat = FP008[7]; inputCarbing = FP008[8]; inputType = FP008[9]; inputCode = FP008[10]; break;
          case 31133 : inputName = FP009[1]; inputAir = FP009[2]; inputEnergy = FP009[3]; inputProtein = FP009[4]; inputLemak = FP009[5]; inputKarbo = FP009[6]; inputSerat = FP009[7]; inputCarbing = FP009[8]; inputType = FP009[9]; inputCode = FP009[10]; break;
          case 31134 : inputName = FP010[1]; inputAir = FP010[2]; inputEnergy = FP010[3]; inputProtein = FP010[4]; inputLemak = FP010[5]; inputKarbo = FP010[6]; inputSerat = FP010[7]; inputCarbing = FP010[8]; inputType = FP010[9]; inputCode = FP010[10]; break;
          case 31135 : inputName = FP011[1]; inputAir = FP011[2]; inputEnergy = FP011[3]; inputProtein = FP011[4]; inputLemak = FP011[5]; inputKarbo = FP011[6]; inputSerat = FP011[7]; inputCarbing = FP011[8]; inputType = FP011[9]; inputCode = FP011[10]; break;
          case 31141 : inputName = FP012[1]; inputAir = FP012[2]; inputEnergy = FP012[3]; inputProtein = FP012[4]; inputLemak = FP012[5]; inputKarbo = FP012[6]; inputSerat = FP012[7]; inputCarbing = FP012[8]; inputType = FP012[9]; inputCode = FP012[10]; break;
          case 31142 : inputName = FP013[1]; inputAir = FP013[2]; inputEnergy = FP013[3]; inputProtein = FP013[4]; inputLemak = FP013[5]; inputKarbo = FP013[6]; inputSerat = FP013[7]; inputCarbing = FP013[8]; inputType = FP013[9]; inputCode = FP013[10]; break;
          case 31143 : inputName = FP014[1]; inputAir = FP014[2]; inputEnergy = FP014[3]; inputProtein = FP014[4]; inputLemak = FP014[5]; inputKarbo = FP014[6]; inputSerat = FP014[7]; inputCarbing = FP014[8]; inputType = FP014[9]; inputCode = FP014[10]; break;
          case 31144 : inputName = FP015[1]; inputAir = FP015[2]; inputEnergy = FP015[3]; inputProtein = FP015[4]; inputLemak = FP015[5]; inputKarbo = FP015[6]; inputSerat = FP015[7]; inputCarbing = FP015[8]; inputType = FP015[9]; inputCode = FP015[10]; break;
          case 31145 : inputName = FP016[1]; inputAir = FP016[2]; inputEnergy = FP016[3]; inputProtein = FP016[4]; inputLemak = FP016[5]; inputKarbo = FP016[6]; inputSerat = FP016[7]; inputCarbing = FP016[8]; inputType = FP016[9]; inputCode = FP016[10]; break;
          case 31151 : inputName = FP017[1]; inputAir = FP017[2]; inputEnergy = FP017[3]; inputProtein = FP017[4]; inputLemak = FP017[5]; inputKarbo = FP017[6]; inputSerat = FP017[7]; inputCarbing = FP017[8]; inputType = FP017[9]; inputCode = FP017[10]; break;
          case 31152 : inputName = FP018[1]; inputAir = FP018[2]; inputEnergy = FP018[3]; inputProtein = FP018[4]; inputLemak = FP018[5]; inputKarbo = FP018[6]; inputSerat = FP018[7]; inputCarbing = FP018[8]; inputType = FP018[9]; inputCode = FP018[10]; break;
          case 31153 : inputName = FP019[1]; inputAir = FP019[2]; inputEnergy = FP019[3]; inputProtein = FP019[4]; inputLemak = FP019[5]; inputKarbo = FP019[6]; inputSerat = FP019[7]; inputCarbing = FP019[8]; inputType = FP019[9]; inputCode = FP019[10]; break;
          case 31154 : inputName = FP020[1]; inputAir = FP020[2]; inputEnergy = FP020[3]; inputProtein = FP020[4]; inputLemak = FP020[5]; inputKarbo = FP020[6]; inputSerat = FP020[7]; inputCarbing = FP020[8]; inputType = FP020[9]; inputCode = FP020[10]; break;
          case 31155 : inputName = FP021[1]; inputAir = FP021[2]; inputEnergy = FP021[3]; inputProtein = FP021[4]; inputLemak = FP021[5]; inputKarbo = FP021[6]; inputSerat = FP021[7]; inputCarbing = FP021[8]; inputType = FP021[9]; inputCode = FP021[10]; break;
          case 31211 : inputName = FP022[1]; inputAir = FP022[2]; inputEnergy = FP022[3]; inputProtein = FP022[4]; inputLemak = FP022[5]; inputKarbo = FP022[6]; inputSerat = FP022[7]; inputCarbing = FP022[8]; inputType = FP022[9]; inputCode = FP022[10]; break;
          case 31212 : inputName = FP023[1]; inputAir = FP023[2]; inputEnergy = FP023[3]; inputProtein = FP023[4]; inputLemak = FP023[5]; inputKarbo = FP023[6]; inputSerat = FP023[7]; inputCarbing = FP023[8]; inputType = FP023[9]; inputCode = FP023[10]; break;
          case 31213 : inputName = FP024[1]; inputAir = FP024[2]; inputEnergy = FP024[3]; inputProtein = FP024[4]; inputLemak = FP024[5]; inputKarbo = FP024[6]; inputSerat = FP024[7]; inputCarbing = FP024[8]; inputType = FP024[9]; inputCode = FP024[10]; break;
          case 31214 : inputName = FP025[1]; inputAir = FP025[2]; inputEnergy = FP025[3]; inputProtein = FP025[4]; inputLemak = FP025[5]; inputKarbo = FP025[6]; inputSerat = FP025[7]; inputCarbing = FP025[8]; inputType = FP025[9]; inputCode = FP025[10]; break;
          case 31215 : inputName = FP026[1]; inputAir = FP026[2]; inputEnergy = FP026[3]; inputProtein = FP026[4]; inputLemak = FP026[5]; inputKarbo = FP026[6]; inputSerat = FP026[7]; inputCarbing = FP026[8]; inputType = FP026[9]; inputCode = FP026[10]; break;
          case 31221 : inputName = FP027[1]; inputAir = FP027[2]; inputEnergy = FP027[3]; inputProtein = FP027[4]; inputLemak = FP027[5]; inputKarbo = FP027[6]; inputSerat = FP027[7]; inputCarbing = FP027[8]; inputType = FP027[9]; inputCode = FP027[10]; break;
          case 31222 : inputName = FP028[1]; inputAir = FP028[2]; inputEnergy = FP028[3]; inputProtein = FP028[4]; inputLemak = FP028[5]; inputKarbo = FP028[6]; inputSerat = FP028[7]; inputCarbing = FP028[8]; inputType = FP028[9]; inputCode = FP028[10]; break;
          case 31223 : inputName = FP029[1]; inputAir = FP029[2]; inputEnergy = FP029[3]; inputProtein = FP029[4]; inputLemak = FP029[5]; inputKarbo = FP029[6]; inputSerat = FP029[7]; inputCarbing = FP029[8]; inputType = FP029[9]; inputCode = FP029[10]; break;
          case 31224 : inputName = FP030[1]; inputAir = FP030[2]; inputEnergy = FP030[3]; inputProtein = FP030[4]; inputLemak = FP030[5]; inputKarbo = FP030[6]; inputSerat = FP030[7]; inputCarbing = FP030[8]; inputType = FP030[9]; inputCode = FP030[10]; break;
          case 31225 : inputName = FP031[1]; inputAir = FP031[2]; inputEnergy = FP031[3]; inputProtein = FP031[4]; inputLemak = FP031[5]; inputKarbo = FP031[6]; inputSerat = FP031[7]; inputCarbing = FP031[8]; inputType = FP031[9]; inputCode = FP031[10]; break;
          case 31231 : inputName = FP032[1]; inputAir = FP032[2]; inputEnergy = FP032[3]; inputProtein = FP032[4]; inputLemak = FP032[5]; inputKarbo = FP032[6]; inputSerat = FP032[7]; inputCarbing = FP032[8]; inputType = FP032[9]; inputCode = FP032[10]; break;
          case 31232 : inputName = FP033[1]; inputAir = FP033[2]; inputEnergy = FP033[3]; inputProtein = FP033[4]; inputLemak = FP033[5]; inputKarbo = FP033[6]; inputSerat = FP033[7]; inputCarbing = FP033[8]; inputType = FP033[9]; inputCode = FP033[10]; break;
          case 31233 : inputName = FP034[1]; inputAir = FP034[2]; inputEnergy = FP034[3]; inputProtein = FP034[4]; inputLemak = FP034[5]; inputKarbo = FP034[6]; inputSerat = FP034[7]; inputCarbing = FP034[8]; inputType = FP034[9]; inputCode = FP034[10]; break;
          case 31234 : inputName = FP035[1]; inputAir = FP035[2]; inputEnergy = FP035[3]; inputProtein = FP035[4]; inputLemak = FP035[5]; inputKarbo = FP035[6]; inputSerat = FP035[7]; inputCarbing = FP035[8]; inputType = FP035[9]; inputCode = FP035[10]; break;
          case 31235 : inputName = FP036[1]; inputAir = FP036[2]; inputEnergy = FP036[3]; inputProtein = FP036[4]; inputLemak = FP036[5]; inputKarbo = FP036[6]; inputSerat = FP036[7]; inputCarbing = FP036[8]; inputType = FP036[9]; inputCode = FP036[10]; break;
          case 31241 : inputName = FP037[1]; inputAir = FP037[2]; inputEnergy = FP037[3]; inputProtein = FP037[4]; inputLemak = FP037[5]; inputKarbo = FP037[6]; inputSerat = FP037[7]; inputCarbing = FP037[8]; inputType = FP037[9]; inputCode = FP037[10]; break;
          case 31242 : inputName = FP038[1]; inputAir = FP038[2]; inputEnergy = FP038[3]; inputProtein = FP038[4]; inputLemak = FP038[5]; inputKarbo = FP038[6]; inputSerat = FP038[7]; inputCarbing = FP038[8]; inputType = FP038[9]; inputCode = FP038[10]; break;
          case 31243 : inputName = FP039[1]; inputAir = FP039[2]; inputEnergy = FP039[3]; inputProtein = FP039[4]; inputLemak = FP039[5]; inputKarbo = FP039[6]; inputSerat = FP039[7]; inputCarbing = FP039[8]; inputType = FP039[9]; inputCode = FP039[10]; break;
          case 31244 : inputName = FP040[1]; inputAir = FP040[2]; inputEnergy = FP040[3]; inputProtein = FP040[4]; inputLemak = FP040[5]; inputKarbo = FP040[6]; inputSerat = FP040[7]; inputCarbing = FP040[8]; inputType = FP040[9]; inputCode = FP040[10]; break;
          case 31245 : inputName = FP041[1]; inputAir = FP041[2]; inputEnergy = FP041[3]; inputProtein = FP041[4]; inputLemak = FP041[5]; inputKarbo = FP041[6]; inputSerat = FP041[7]; inputCarbing = FP041[8]; inputType = FP041[9]; inputCode = FP041[10]; break;
          case 31251 : inputName = FP042[1]; inputAir = FP042[2]; inputEnergy = FP042[3]; inputProtein = FP042[4]; inputLemak = FP042[5]; inputKarbo = FP042[6]; inputSerat = FP042[7]; inputCarbing = FP042[8]; inputType = FP042[9]; inputCode = FP042[10]; break;
          case 31252 : inputName = FP043[1]; inputAir = FP043[2]; inputEnergy = FP043[3]; inputProtein = FP043[4]; inputLemak = FP043[5]; inputKarbo = FP043[6]; inputSerat = FP043[7]; inputCarbing = FP043[8]; inputType = FP043[9]; inputCode = FP043[10]; break;
          case 31253 : inputName = FP044[1]; inputAir = FP044[2]; inputEnergy = FP044[3]; inputProtein = FP044[4]; inputLemak = FP044[5]; inputKarbo = FP044[6]; inputSerat = FP044[7]; inputCarbing = FP044[8]; inputType = FP044[9]; inputCode = FP044[10]; break;
          case 31254 : inputName = FP045[1]; inputAir = FP045[2]; inputEnergy = FP045[3]; inputProtein = FP045[4]; inputLemak = FP045[5]; inputKarbo = FP045[6]; inputSerat = FP045[7]; inputCarbing = FP045[8]; inputType = FP045[9]; inputCode = FP045[10]; break;
          case 31255 : inputName = FP046[1]; inputAir = FP046[2]; inputEnergy = FP046[3]; inputProtein = FP046[4]; inputLemak = FP046[5]; inputKarbo = FP046[6]; inputSerat = FP046[7]; inputCarbing = FP046[8]; inputType = FP046[9]; inputCode = FP046[10]; break;
          case 31311 : inputName = FP047[1]; inputAir = FP047[2]; inputEnergy = FP047[3]; inputProtein = FP047[4]; inputLemak = FP047[5]; inputKarbo = FP047[6]; inputSerat = FP047[7]; inputCarbing = FP047[8]; inputType = FP047[9]; inputCode = FP047[10]; break;
          case 31312 : inputName = FP048[1]; inputAir = FP048[2]; inputEnergy = FP048[3]; inputProtein = FP048[4]; inputLemak = FP048[5]; inputKarbo = FP048[6]; inputSerat = FP048[7]; inputCarbing = FP048[8]; inputType = FP048[9]; inputCode = FP048[10]; break;
          case 31313 : inputName = FP049[1]; inputAir = FP049[2]; inputEnergy = FP049[3]; inputProtein = FP049[4]; inputLemak = FP049[5]; inputKarbo = FP049[6]; inputSerat = FP049[7]; inputCarbing = FP049[8]; inputType = FP049[9]; inputCode = FP049[10]; break;
          case 31314 : inputName = FP050[1]; inputAir = FP050[2]; inputEnergy = FP050[3]; inputProtein = FP050[4]; inputLemak = FP050[5]; inputKarbo = FP050[6]; inputSerat = FP050[7]; inputCarbing = FP050[8]; inputType = FP050[9]; inputCode = FP050[10]; break;
          case 31315 : inputName = FP051[1]; inputAir = FP051[2]; inputEnergy = FP051[3]; inputProtein = FP051[4]; inputLemak = FP051[5]; inputKarbo = FP051[6]; inputSerat = FP051[7]; inputCarbing = FP051[8]; inputType = FP051[9]; inputCode = FP051[10]; break;
          case 31321 : inputName = FP052[1]; inputAir = FP052[2]; inputEnergy = FP052[3]; inputProtein = FP052[4]; inputLemak = FP052[5]; inputKarbo = FP052[6]; inputSerat = FP052[7]; inputCarbing = FP052[8]; inputType = FP052[9]; inputCode = FP052[10]; break;
          case 31322 : inputName = FP053[1]; inputAir = FP053[2]; inputEnergy = FP053[3]; inputProtein = FP053[4]; inputLemak = FP053[5]; inputKarbo = FP053[6]; inputSerat = FP053[7]; inputCarbing = FP053[8]; inputType = FP053[9]; inputCode = FP053[10]; break;
          case 31323 : inputName = FP054[1]; inputAir = FP054[2]; inputEnergy = FP054[3]; inputProtein = FP054[4]; inputLemak = FP054[5]; inputKarbo = FP054[6]; inputSerat = FP054[7]; inputCarbing = FP054[8]; inputType = FP054[9]; inputCode = FP054[10]; break;
          case 31324 : inputName = FP055[1]; inputAir = FP055[2]; inputEnergy = FP055[3]; inputProtein = FP055[4]; inputLemak = FP055[5]; inputKarbo = FP055[6]; inputSerat = FP055[7]; inputCarbing = FP055[8]; inputType = FP055[9]; inputCode = FP055[10]; break;
          case 31325 : inputName = FP056[1]; inputAir = FP056[2]; inputEnergy = FP056[3]; inputProtein = FP056[4]; inputLemak = FP056[5]; inputKarbo = FP056[6]; inputSerat = FP056[7]; inputCarbing = FP056[8]; inputType = FP056[9]; inputCode = FP056[10]; break;
          case 31331 : inputName = FP057[1]; inputAir = FP057[2]; inputEnergy = FP057[3]; inputProtein = FP057[4]; inputLemak = FP057[5]; inputKarbo = FP057[6]; inputSerat = FP057[7]; inputCarbing = FP057[8]; inputType = FP057[9]; inputCode = FP057[10]; break;
          case 31332 : inputName = FP058[1]; inputAir = FP058[2]; inputEnergy = FP058[3]; inputProtein = FP058[4]; inputLemak = FP058[5]; inputKarbo = FP058[6]; inputSerat = FP058[7]; inputCarbing = FP058[8]; inputType = FP058[9]; inputCode = FP058[10]; break;
          case 31333 : inputName = FP059[1]; inputAir = FP059[2]; inputEnergy = FP059[3]; inputProtein = FP059[4]; inputLemak = FP059[5]; inputKarbo = FP059[6]; inputSerat = FP059[7]; inputCarbing = FP059[8]; inputType = FP059[9]; inputCode = FP059[10]; break;
          case 31334 : inputName = FP060[1]; inputAir = FP060[2]; inputEnergy = FP060[3]; inputProtein = FP060[4]; inputLemak = FP060[5]; inputKarbo = FP060[6]; inputSerat = FP060[7]; inputCarbing = FP060[8]; inputType = FP060[9]; inputCode = FP060[10]; break;
          case 31335 : inputName = FP061[1]; inputAir = FP061[2]; inputEnergy = FP061[3]; inputProtein = FP061[4]; inputLemak = FP061[5]; inputKarbo = FP061[6]; inputSerat = FP061[7]; inputCarbing = FP061[8]; inputType = FP061[9]; inputCode = FP061[10]; break;
          case 31341 : inputName = FP062[1]; inputAir = FP062[2]; inputEnergy = FP062[3]; inputProtein = FP062[4]; inputLemak = FP062[5]; inputKarbo = FP062[6]; inputSerat = FP062[7]; inputCarbing = FP062[8]; inputType = FP062[9]; inputCode = FP062[10]; break;
          case 31342 : inputName = FP063[1]; inputAir = FP063[2]; inputEnergy = FP063[3]; inputProtein = FP063[4]; inputLemak = FP063[5]; inputKarbo = FP063[6]; inputSerat = FP063[7]; inputCarbing = FP063[8]; inputType = FP063[9]; inputCode = FP063[10]; break;
          case 31343 : inputName = FP064[1]; inputAir = FP064[2]; inputEnergy = FP064[3]; inputProtein = FP064[4]; inputLemak = FP064[5]; inputKarbo = FP064[6]; inputSerat = FP064[7]; inputCarbing = FP064[8]; inputType = FP064[9]; inputCode = FP064[10]; break;
          case 31344 : inputName = FP065[1]; inputAir = FP065[2]; inputEnergy = FP065[3]; inputProtein = FP065[4]; inputLemak = FP065[5]; inputKarbo = FP065[6]; inputSerat = FP065[7]; inputCarbing = FP065[8]; inputType = FP065[9]; inputCode = FP065[10]; break;
          case 31345 : inputName = FP066[1]; inputAir = FP066[2]; inputEnergy = FP066[3]; inputProtein = FP066[4]; inputLemak = FP066[5]; inputKarbo = FP066[6]; inputSerat = FP066[7]; inputCarbing = FP066[8]; inputType = FP066[9]; inputCode = FP066[10]; break;
          case 31351 : inputName = FP067[1]; inputAir = FP067[2]; inputEnergy = FP067[3]; inputProtein = FP067[4]; inputLemak = FP067[5]; inputKarbo = FP067[6]; inputSerat = FP067[7]; inputCarbing = FP067[8]; inputType = FP067[9]; inputCode = FP067[10]; break;
          case 31352 : inputName = FP068[1]; inputAir = FP068[2]; inputEnergy = FP068[3]; inputProtein = FP068[4]; inputLemak = FP068[5]; inputKarbo = FP068[6]; inputSerat = FP068[7]; inputCarbing = FP068[8]; inputType = FP068[9]; inputCode = FP068[10]; break;
          case 31353 : inputName = FP069[1]; inputAir = FP069[2]; inputEnergy = FP069[3]; inputProtein = FP069[4]; inputLemak = FP069[5]; inputKarbo = FP069[6]; inputSerat = FP069[7]; inputCarbing = FP069[8]; inputType = FP069[9]; inputCode = FP069[10]; break;
          case 31354 : inputName = FP070[1]; inputAir = FP070[2]; inputEnergy = FP070[3]; inputProtein = FP070[4]; inputLemak = FP070[5]; inputKarbo = FP070[6]; inputSerat = FP070[7]; inputCarbing = FP070[8]; inputType = FP070[9]; inputCode = FP070[10]; break;
          case 31355 : inputName = FP071[1]; inputAir = FP071[2]; inputEnergy = FP071[3]; inputProtein = FP071[4]; inputLemak = FP071[5]; inputKarbo = FP071[6]; inputSerat = FP071[7]; inputCarbing = FP071[8]; inputType = FP071[9]; inputCode = FP071[10]; break;
          case 31411 : inputName = FP072[1]; inputAir = FP072[2]; inputEnergy = FP072[3]; inputProtein = FP072[4]; inputLemak = FP072[5]; inputKarbo = FP072[6]; inputSerat = FP072[7]; inputCarbing = FP072[8]; inputType = FP072[9]; inputCode = FP072[10]; break;
          case 31412 : inputName = FP073[1]; inputAir = FP073[2]; inputEnergy = FP073[3]; inputProtein = FP073[4]; inputLemak = FP073[5]; inputKarbo = FP073[6]; inputSerat = FP073[7]; inputCarbing = FP073[8]; inputType = FP073[9]; inputCode = FP073[10]; break;
          case 31413 : inputName = FP074[1]; inputAir = FP074[2]; inputEnergy = FP074[3]; inputProtein = FP074[4]; inputLemak = FP074[5]; inputKarbo = FP074[6]; inputSerat = FP074[7]; inputCarbing = FP074[8]; inputType = FP074[9]; inputCode = FP074[10]; break;
          case 31414 : inputName = FP075[1]; inputAir = FP075[2]; inputEnergy = FP075[3]; inputProtein = FP075[4]; inputLemak = FP075[5]; inputKarbo = FP075[6]; inputSerat = FP075[7]; inputCarbing = FP075[8]; inputType = FP075[9]; inputCode = FP075[10]; break;
          case 31415 : inputName = FP076[1]; inputAir = FP076[2]; inputEnergy = FP076[3]; inputProtein = FP076[4]; inputLemak = FP076[5]; inputKarbo = FP076[6]; inputSerat = FP076[7]; inputCarbing = FP076[8]; inputType = FP076[9]; inputCode = FP076[10]; break;
          case 31421 : inputName = FP077[1]; inputAir = FP077[2]; inputEnergy = FP077[3]; inputProtein = FP077[4]; inputLemak = FP077[5]; inputKarbo = FP077[6]; inputSerat = FP077[7]; inputCarbing = FP077[8]; inputType = FP077[9]; inputCode = FP077[10]; break;
          case 31422 : inputName = FP078[1]; inputAir = FP078[2]; inputEnergy = FP078[3]; inputProtein = FP078[4]; inputLemak = FP078[5]; inputKarbo = FP078[6]; inputSerat = FP078[7]; inputCarbing = FP078[8]; inputType = FP078[9]; inputCode = FP078[10]; break;
          case 31423 : inputName = FP079[1]; inputAir = FP079[2]; inputEnergy = FP079[3]; inputProtein = FP079[4]; inputLemak = FP079[5]; inputKarbo = FP079[6]; inputSerat = FP079[7]; inputCarbing = FP079[8]; inputType = FP079[9]; inputCode = FP079[10]; break;
          case 31424 : inputName = FP080[1]; inputAir = FP080[2]; inputEnergy = FP080[3]; inputProtein = FP080[4]; inputLemak = FP080[5]; inputKarbo = FP080[6]; inputSerat = FP080[7]; inputCarbing = FP080[8]; inputType = FP080[9]; inputCode = FP080[10]; break;
          case 31425 : inputName = FP081[1]; inputAir = FP081[2]; inputEnergy = FP081[3]; inputProtein = FP081[4]; inputLemak = FP081[5]; inputKarbo = FP081[6]; inputSerat = FP081[7]; inputCarbing = FP081[8]; inputType = FP081[9]; inputCode = FP081[10]; break;
          case 31431 : inputName = FP082[1]; inputAir = FP082[2]; inputEnergy = FP082[3]; inputProtein = FP082[4]; inputLemak = FP082[5]; inputKarbo = FP082[6]; inputSerat = FP082[7]; inputCarbing = FP082[8]; inputType = FP082[9]; inputCode = FP082[10]; break;
          case 31432 : inputName = FP083[1]; inputAir = FP083[2]; inputEnergy = FP083[3]; inputProtein = FP083[4]; inputLemak = FP083[5]; inputKarbo = FP083[6]; inputSerat = FP083[7]; inputCarbing = FP083[8]; inputType = FP083[9]; inputCode = FP083[10]; break;
          case 31433 : inputName = FP084[1]; inputAir = FP084[2]; inputEnergy = FP084[3]; inputProtein = FP084[4]; inputLemak = FP084[5]; inputKarbo = FP084[6]; inputSerat = FP084[7]; inputCarbing = FP084[8]; inputType = FP084[9]; inputCode = FP084[10]; break;
          case 31434 : inputName = FP085[1]; inputAir = FP085[2]; inputEnergy = FP085[3]; inputProtein = FP085[4]; inputLemak = FP085[5]; inputKarbo = FP085[6]; inputSerat = FP085[7]; inputCarbing = FP085[8]; inputType = FP085[9]; inputCode = FP085[10]; break;
          case 31435 : inputName = FP086[1]; inputAir = FP086[2]; inputEnergy = FP086[3]; inputProtein = FP086[4]; inputLemak = FP086[5]; inputKarbo = FP086[6]; inputSerat = FP086[7]; inputCarbing = FP086[8]; inputType = FP086[9]; inputCode = FP086[10]; break;
          case 31441 : inputName = FP087[1]; inputAir = FP087[2]; inputEnergy = FP087[3]; inputProtein = FP087[4]; inputLemak = FP087[5]; inputKarbo = FP087[6]; inputSerat = FP087[7]; inputCarbing = FP087[8]; inputType = FP087[9]; inputCode = FP087[10]; break;
          case 31442 : inputName = FP088[1]; inputAir = FP088[2]; inputEnergy = FP088[3]; inputProtein = FP088[4]; inputLemak = FP088[5]; inputKarbo = FP088[6]; inputSerat = FP088[7]; inputCarbing = FP088[8]; inputType = FP088[9]; inputCode = FP088[10]; break;
          case 321 : inputName = GR001[1]; inputAir = GR001[2]; inputEnergy = GR001[3]; inputProtein = GR001[4]; inputLemak = GR001[5]; inputKarbo = GR001[6]; inputSerat = GR001[7]; inputCarbing = GR001[8]; inputType = GR001[9]; inputCode = GR001[10]; break;
          case 322 : inputName = GR002[1]; inputAir = GR002[2]; inputEnergy = GR002[3]; inputProtein = GR002[4]; inputLemak = GR002[5]; inputKarbo = GR002[6]; inputSerat = GR002[7]; inputCarbing = GR002[8]; inputType = GR002[9]; inputCode = GR002[10]; break;
          case 323 : inputName = GR003[1]; inputAir = GR003[2]; inputEnergy = GR003[3]; inputProtein = GR003[4]; inputLemak = GR003[5]; inputKarbo = GR003[6]; inputSerat = GR003[7]; inputCarbing = GR003[8]; inputType = GR003[9]; inputCode = GR003[10]; break;
          case 324 : inputName = GR004[1]; inputAir = GR004[2]; inputEnergy = GR004[3]; inputProtein = GR004[4]; inputLemak = GR004[5]; inputKarbo = GR004[6]; inputSerat = GR004[7]; inputCarbing = GR004[8]; inputType = GR004[9]; inputCode = GR004[10]; break;
          case 325 : inputName = GR005[1]; inputAir = GR005[2]; inputEnergy = GR005[3]; inputProtein = GR005[4]; inputLemak = GR005[5]; inputKarbo = GR005[6]; inputSerat = GR005[7]; inputCarbing = GR005[8]; inputType = GR005[9]; inputCode = GR005[10]; break;
          case 3211 : inputName = GR006[1]; inputAir = GR006[2]; inputEnergy = GR006[3]; inputProtein = GR006[4]; inputLemak = GR006[5]; inputKarbo = GR006[6]; inputSerat = GR006[7]; inputCarbing = GR006[8]; inputType = GR006[9]; inputCode = GR006[10]; break;
          case 3212 : inputName = GR007[1]; inputAir = GR007[2]; inputEnergy = GR007[3]; inputProtein = GR007[4]; inputLemak = GR007[5]; inputKarbo = GR007[6]; inputSerat = GR007[7]; inputCarbing = GR007[8]; inputType = GR007[9]; inputCode = GR007[10]; break;
          case 3213 : inputName = GR008[1]; inputAir = GR008[2]; inputEnergy = GR008[3]; inputProtein = GR008[4]; inputLemak = GR008[5]; inputKarbo = GR008[6]; inputSerat = GR008[7]; inputCarbing = GR008[8]; inputType = GR008[9]; inputCode = GR008[10]; break;
          case 3214 : inputName = GR009[1]; inputAir = GR009[2]; inputEnergy = GR009[3]; inputProtein = GR009[4]; inputLemak = GR009[5]; inputKarbo = GR009[6]; inputSerat = GR009[7]; inputCarbing = GR009[8]; inputType = GR009[9]; inputCode = GR009[10]; break;
          case 3215 : inputName = GR010[1]; inputAir = GR010[2]; inputEnergy = GR010[3]; inputProtein = GR010[4]; inputLemak = GR010[5]; inputKarbo = GR010[6]; inputSerat = GR010[7]; inputCarbing = GR010[8]; inputType = GR010[9]; inputCode = GR010[10]; break;
          case 3221 : inputName = GR011[1]; inputAir = GR011[2]; inputEnergy = GR011[3]; inputProtein = GR011[4]; inputLemak = GR011[5]; inputKarbo = GR011[6]; inputSerat = GR011[7]; inputCarbing = GR011[8]; inputType = GR011[9]; inputCode = GR011[10]; break;
          case 3222 : inputName = GR012[1]; inputAir = GR012[2]; inputEnergy = GR012[3]; inputProtein = GR012[4]; inputLemak = GR012[5]; inputKarbo = GR012[6]; inputSerat = GR012[7]; inputCarbing = GR012[8]; inputType = GR012[9]; inputCode = GR012[10]; break;
          case 3223 : inputName = GR013[1]; inputAir = GR013[2]; inputEnergy = GR013[3]; inputProtein = GR013[4]; inputLemak = GR013[5]; inputKarbo = GR013[6]; inputSerat = GR013[7]; inputCarbing = GR013[8]; inputType = GR013[9]; inputCode = GR013[10]; break;
          case 3224 : inputName = GR014[1]; inputAir = GR014[2]; inputEnergy = GR014[3]; inputProtein = GR014[4]; inputLemak = GR014[5]; inputKarbo = GR014[6]; inputSerat = GR014[7]; inputCarbing = GR014[8]; inputType = GR014[9]; inputCode = GR014[10]; break;
          case 3225 : inputName = GR015[1]; inputAir = GR015[2]; inputEnergy = GR015[3]; inputProtein = GR015[4]; inputLemak = GR015[5]; inputKarbo = GR015[6]; inputSerat = GR015[7]; inputCarbing = GR015[8]; inputType = GR015[9]; inputCode = GR015[10]; break;
          case 3231 : inputName = GR016[1]; inputAir = GR016[2]; inputEnergy = GR016[3]; inputProtein = GR016[4]; inputLemak = GR016[5]; inputKarbo = GR016[6]; inputSerat = GR016[7]; inputCarbing = GR016[8]; inputType = GR016[9]; inputCode = GR016[10]; break;
          case 3232 : inputName = GR017[1]; inputAir = GR017[2]; inputEnergy = GR017[3]; inputProtein = GR017[4]; inputLemak = GR017[5]; inputKarbo = GR017[6]; inputSerat = GR017[7]; inputCarbing = GR017[8]; inputType = GR017[9]; inputCode = GR017[10]; break;
          case 3233 : inputName = GR018[1]; inputAir = GR018[2]; inputEnergy = GR018[3]; inputProtein = GR018[4]; inputLemak = GR018[5]; inputKarbo = GR018[6]; inputSerat = GR018[7]; inputCarbing = GR018[8]; inputType = GR018[9]; inputCode = GR018[10]; break;
          case 3234 : inputName = GR019[1]; inputAir = GR019[2]; inputEnergy = GR019[3]; inputProtein = GR019[4]; inputLemak = GR019[5]; inputKarbo = GR019[6]; inputSerat = GR019[7]; inputCarbing = GR019[8]; inputType = GR019[9]; inputCode = GR019[10]; break;
          case 3235 : inputName = GR020[1]; inputAir = GR020[2]; inputEnergy = GR020[3]; inputProtein = GR020[4]; inputLemak = GR020[5]; inputKarbo = GR020[6]; inputSerat = GR020[7]; inputCarbing = GR020[8]; inputType = GR020[9]; inputCode = GR020[10]; break;
          case 3241 : inputName = GR021[1]; inputAir = GR021[2]; inputEnergy = GR021[3]; inputProtein = GR021[4]; inputLemak = GR021[5]; inputKarbo = GR021[6]; inputSerat = GR021[7]; inputCarbing = GR021[8]; inputType = GR021[9]; inputCode = GR021[10]; break;
          case 3242 : inputName = GR022[1]; inputAir = GR022[2]; inputEnergy = GR022[3]; inputProtein = GR022[4]; inputLemak = GR022[5]; inputKarbo = GR022[6]; inputSerat = GR022[7]; inputCarbing = GR022[8]; inputType = GR022[9]; inputCode = GR022[10]; break;
          case 3243 : inputName = GR023[1]; inputAir = GR023[2]; inputEnergy = GR023[3]; inputProtein = GR023[4]; inputLemak = GR023[5]; inputKarbo = GR023[6]; inputSerat = GR023[7]; inputCarbing = GR023[8]; inputType = GR023[9]; inputCode = GR023[10]; break;
          case 3244 : inputName = GR024[1]; inputAir = GR024[2]; inputEnergy = GR024[3]; inputProtein = GR024[4]; inputLemak = GR024[5]; inputKarbo = GR024[6]; inputSerat = GR024[7]; inputCarbing = GR024[8]; inputType = GR024[9]; inputCode = GR024[10]; break;
          case 3245 : inputName = GR025[1]; inputAir = GR025[2]; inputEnergy = GR025[3]; inputProtein = GR025[4]; inputLemak = GR025[5]; inputKarbo = GR025[6]; inputSerat = GR025[7]; inputCarbing = GR025[8]; inputType = GR025[9]; inputCode = GR025[10]; break;
          case 3251 : inputName = GR026[1]; inputAir = GR026[2]; inputEnergy = GR026[3]; inputProtein = GR026[4]; inputLemak = GR026[5]; inputKarbo = GR026[6]; inputSerat = GR026[7]; inputCarbing = GR026[8]; inputType = GR026[9]; inputCode = GR026[10]; break;
          case 3252 : inputName = GR027[1]; inputAir = GR027[2]; inputEnergy = GR027[3]; inputProtein = GR027[4]; inputLemak = GR027[5]; inputKarbo = GR027[6]; inputSerat = GR027[7]; inputCarbing = GR027[8]; inputType = GR027[9]; inputCode = GR027[10]; break;
          case 3253 : inputName = GR028[1]; inputAir = GR028[2]; inputEnergy = GR028[3]; inputProtein = GR028[4]; inputLemak = GR028[5]; inputKarbo = GR028[6]; inputSerat = GR028[7]; inputCarbing = GR028[8]; inputType = GR028[9]; inputCode = GR028[10]; break;
          case 3254 : inputName = GR029[1]; inputAir = GR029[2]; inputEnergy = GR029[3]; inputProtein = GR029[4]; inputLemak = GR029[5]; inputKarbo = GR029[6]; inputSerat = GR029[7]; inputCarbing = GR029[8]; inputType = GR029[9]; inputCode = GR029[10]; break;
          case 3255 : inputName = GR030[1]; inputAir = GR030[2]; inputEnergy = GR030[3]; inputProtein = GR030[4]; inputLemak = GR030[5]; inputKarbo = GR030[6]; inputSerat = GR030[7]; inputCarbing = GR030[8]; inputType = GR030[9]; inputCode = GR030[10]; break;
          case 32111 : inputName = GR031[1]; inputAir = GR031[2]; inputEnergy = GR031[3]; inputProtein = GR031[4]; inputLemak = GR031[5]; inputKarbo = GR031[6]; inputSerat = GR031[7]; inputCarbing = GR031[8]; inputType = GR031[9]; inputCode = GR031[10]; break;
          case 32112 : inputName = GR032[1]; inputAir = GR032[2]; inputEnergy = GR032[3]; inputProtein = GR032[4]; inputLemak = GR032[5]; inputKarbo = GR032[6]; inputSerat = GR032[7]; inputCarbing = GR032[8]; inputType = GR032[9]; inputCode = GR032[10]; break;
          case 32113 : inputName = GR033[1]; inputAir = GR033[2]; inputEnergy = GR033[3]; inputProtein = GR033[4]; inputLemak = GR033[5]; inputKarbo = GR033[6]; inputSerat = GR033[7]; inputCarbing = GR033[8]; inputType = GR033[9]; inputCode = GR033[10]; break;
          case 32114 : inputName = GR034[1]; inputAir = GR034[2]; inputEnergy = GR034[3]; inputProtein = GR034[4]; inputLemak = GR034[5]; inputKarbo = GR034[6]; inputSerat = GR034[7]; inputCarbing = GR034[8]; inputType = GR034[9]; inputCode = GR034[10]; break;
          case 32115 : inputName = GR035[1]; inputAir = GR035[2]; inputEnergy = GR035[3]; inputProtein = GR035[4]; inputLemak = GR035[5]; inputKarbo = GR035[6]; inputSerat = GR035[7]; inputCarbing = GR035[8]; inputType = GR035[9]; inputCode = GR035[10]; break;
          case 32121 : inputName = GR036[1]; inputAir = GR036[2]; inputEnergy = GR036[3]; inputProtein = GR036[4]; inputLemak = GR036[5]; inputKarbo = GR036[6]; inputSerat = GR036[7]; inputCarbing = GR036[8]; inputType = GR036[9]; inputCode = GR036[10]; break;
          case 32122 : inputName = GR037[1]; inputAir = GR037[2]; inputEnergy = GR037[3]; inputProtein = GR037[4]; inputLemak = GR037[5]; inputKarbo = GR037[6]; inputSerat = GR037[7]; inputCarbing = GR037[8]; inputType = GR037[9]; inputCode = GR037[10]; break;
          case 32123 : inputName = GR038[1]; inputAir = GR038[2]; inputEnergy = GR038[3]; inputProtein = GR038[4]; inputLemak = GR038[5]; inputKarbo = GR038[6]; inputSerat = GR038[7]; inputCarbing = GR038[8]; inputType = GR038[9]; inputCode = GR038[10]; break;
          case 32124 : inputName = GR039[1]; inputAir = GR039[2]; inputEnergy = GR039[3]; inputProtein = GR039[4]; inputLemak = GR039[5]; inputKarbo = GR039[6]; inputSerat = GR039[7]; inputCarbing = GR039[8]; inputType = GR039[9]; inputCode = GR039[10]; break;
          case 32125 : inputName = GR040[1]; inputAir = GR040[2]; inputEnergy = GR040[3]; inputProtein = GR040[4]; inputLemak = GR040[5]; inputKarbo = GR040[6]; inputSerat = GR040[7]; inputCarbing = GR040[8]; inputType = GR040[9]; inputCode = GR040[10]; break;
          case 32131 : inputName = GR041[1]; inputAir = GR041[2]; inputEnergy = GR041[3]; inputProtein = GR041[4]; inputLemak = GR041[5]; inputKarbo = GR041[6]; inputSerat = GR041[7]; inputCarbing = GR041[8]; inputType = GR041[9]; inputCode = GR041[10]; break;
          case 32132 : inputName = GR042[1]; inputAir = GR042[2]; inputEnergy = GR042[3]; inputProtein = GR042[4]; inputLemak = GR042[5]; inputKarbo = GR042[6]; inputSerat = GR042[7]; inputCarbing = GR042[8]; inputType = GR042[9]; inputCode = GR042[10]; break;
          case 32133 : inputName = GR043[1]; inputAir = GR043[2]; inputEnergy = GR043[3]; inputProtein = GR043[4]; inputLemak = GR043[5]; inputKarbo = GR043[6]; inputSerat = GR043[7]; inputCarbing = GR043[8]; inputType = GR043[9]; inputCode = GR043[10]; break;
          case 32134 : inputName = GR044[1]; inputAir = GR044[2]; inputEnergy = GR044[3]; inputProtein = GR044[4]; inputLemak = GR044[5]; inputKarbo = GR044[6]; inputSerat = GR044[7]; inputCarbing = GR044[8]; inputType = GR044[9]; inputCode = GR044[10]; break;
          case 32135 : inputName = GR045[1]; inputAir = GR045[2]; inputEnergy = GR045[3]; inputProtein = GR045[4]; inputLemak = GR045[5]; inputKarbo = GR045[6]; inputSerat = GR045[7]; inputCarbing = GR045[8]; inputType = GR045[9]; inputCode = GR045[10]; break;
          case 32141 : inputName = GR046[1]; inputAir = GR046[2]; inputEnergy = GR046[3]; inputProtein = GR046[4]; inputLemak = GR046[5]; inputKarbo = GR046[6]; inputSerat = GR046[7]; inputCarbing = GR046[8]; inputType = GR046[9]; inputCode = GR046[10]; break;
          case 32142 : inputName = GR047[1]; inputAir = GR047[2]; inputEnergy = GR047[3]; inputProtein = GR047[4]; inputLemak = GR047[5]; inputKarbo = GR047[6]; inputSerat = GR047[7]; inputCarbing = GR047[8]; inputType = GR047[9]; inputCode = GR047[10]; break;
          case 32143 : inputName = GR048[1]; inputAir = GR048[2]; inputEnergy = GR048[3]; inputProtein = GR048[4]; inputLemak = GR048[5]; inputKarbo = GR048[6]; inputSerat = GR048[7]; inputCarbing = GR048[8]; inputType = GR048[9]; inputCode = GR048[10]; break;
          case 32144 : inputName = GR049[1]; inputAir = GR049[2]; inputEnergy = GR049[3]; inputProtein = GR049[4]; inputLemak = GR049[5]; inputKarbo = GR049[6]; inputSerat = GR049[7]; inputCarbing = GR049[8]; inputType = GR049[9]; inputCode = GR049[10]; break;
          case 32145 : inputName = GR050[1]; inputAir = GR050[2]; inputEnergy = GR050[3]; inputProtein = GR050[4]; inputLemak = GR050[5]; inputKarbo = GR050[6]; inputSerat = GR050[7]; inputCarbing = GR050[8]; inputType = GR050[9]; inputCode = GR050[10]; break;
          case 32151 : inputName = GR051[1]; inputAir = GR051[2]; inputEnergy = GR051[3]; inputProtein = GR051[4]; inputLemak = GR051[5]; inputKarbo = GR051[6]; inputSerat = GR051[7]; inputCarbing = GR051[8]; inputType = GR051[9]; inputCode = GR051[10]; break;
          case 32152 : inputName = GR052[1]; inputAir = GR052[2]; inputEnergy = GR052[3]; inputProtein = GR052[4]; inputLemak = GR052[5]; inputKarbo = GR052[6]; inputSerat = GR052[7]; inputCarbing = GR052[8]; inputType = GR052[9]; inputCode = GR052[10]; break;
          case 32153 : inputName = GR053[1]; inputAir = GR053[2]; inputEnergy = GR053[3]; inputProtein = GR053[4]; inputLemak = GR053[5]; inputKarbo = GR053[6]; inputSerat = GR053[7]; inputCarbing = GR053[8]; inputType = GR053[9]; inputCode = GR053[10]; break;
          case 32154 : inputName = GR054[1]; inputAir = GR054[2]; inputEnergy = GR054[3]; inputProtein = GR054[4]; inputLemak = GR054[5]; inputKarbo = GR054[6]; inputSerat = GR054[7]; inputCarbing = GR054[8]; inputType = GR054[9]; inputCode = GR054[10]; break;
          case 32155 : inputName = GR055[1]; inputAir = GR055[2]; inputEnergy = GR055[3]; inputProtein = GR055[4]; inputLemak = GR055[5]; inputKarbo = GR055[6]; inputSerat = GR055[7]; inputCarbing = GR055[8]; inputType = GR055[9]; inputCode = GR055[10]; break;
          case 32211 : inputName = GR056[1]; inputAir = GR056[2]; inputEnergy = GR056[3]; inputProtein = GR056[4]; inputLemak = GR056[5]; inputKarbo = GR056[6]; inputSerat = GR056[7]; inputCarbing = GR056[8]; inputType = GR056[9]; inputCode = GR056[10]; break;
          case 32212 : inputName = GR057[1]; inputAir = GR057[2]; inputEnergy = GR057[3]; inputProtein = GR057[4]; inputLemak = GR057[5]; inputKarbo = GR057[6]; inputSerat = GR057[7]; inputCarbing = GR057[8]; inputType = GR057[9]; inputCode = GR057[10]; break;
          case 32213 : inputName = GR058[1]; inputAir = GR058[2]; inputEnergy = GR058[3]; inputProtein = GR058[4]; inputLemak = GR058[5]; inputKarbo = GR058[6]; inputSerat = GR058[7]; inputCarbing = GR058[8]; inputType = GR058[9]; inputCode = GR058[10]; break;
          case 32214 : inputName = GR059[1]; inputAir = GR059[2]; inputEnergy = GR059[3]; inputProtein = GR059[4]; inputLemak = GR059[5]; inputKarbo = GR059[6]; inputSerat = GR059[7]; inputCarbing = GR059[8]; inputType = GR059[9]; inputCode = GR059[10]; break;
          case 32215 : inputName = GR060[1]; inputAir = GR060[2]; inputEnergy = GR060[3]; inputProtein = GR060[4]; inputLemak = GR060[5]; inputKarbo = GR060[6]; inputSerat = GR060[7]; inputCarbing = GR060[8]; inputType = GR060[9]; inputCode = GR060[10]; break;
          case 32221 : inputName = GR061[1]; inputAir = GR061[2]; inputEnergy = GR061[3]; inputProtein = GR061[4]; inputLemak = GR061[5]; inputKarbo = GR061[6]; inputSerat = GR061[7]; inputCarbing = GR061[8]; inputType = GR061[9]; inputCode = GR061[10]; break;
          case 32222 : inputName = GR062[1]; inputAir = GR062[2]; inputEnergy = GR062[3]; inputProtein = GR062[4]; inputLemak = GR062[5]; inputKarbo = GR062[6]; inputSerat = GR062[7]; inputCarbing = GR062[8]; inputType = GR062[9]; inputCode = GR062[10]; break;
          case 32223 : inputName = GR063[1]; inputAir = GR063[2]; inputEnergy = GR063[3]; inputProtein = GR063[4]; inputLemak = GR063[5]; inputKarbo = GR063[6]; inputSerat = GR063[7]; inputCarbing = GR063[8]; inputType = GR063[9]; inputCode = GR063[10]; break;
          case 32224 : inputName = GR064[1]; inputAir = GR064[2]; inputEnergy = GR064[3]; inputProtein = GR064[4]; inputLemak = GR064[5]; inputKarbo = GR064[6]; inputSerat = GR064[7]; inputCarbing = GR064[8]; inputType = GR064[9]; inputCode = GR064[10]; break;
          case 32225 : inputName = GR065[1]; inputAir = GR065[2]; inputEnergy = GR065[3]; inputProtein = GR065[4]; inputLemak = GR065[5]; inputKarbo = GR065[6]; inputSerat = GR065[7]; inputCarbing = GR065[8]; inputType = GR065[9]; inputCode = GR065[10]; break;
          case 32231 : inputName = GR066[1]; inputAir = GR066[2]; inputEnergy = GR066[3]; inputProtein = GR066[4]; inputLemak = GR066[5]; inputKarbo = GR066[6]; inputSerat = GR066[7]; inputCarbing = GR066[8]; inputType = GR066[9]; inputCode = GR066[10]; break;
          case 32232 : inputName = GR067[1]; inputAir = GR067[2]; inputEnergy = GR067[3]; inputProtein = GR067[4]; inputLemak = GR067[5]; inputKarbo = GR067[6]; inputSerat = GR067[7]; inputCarbing = GR067[8]; inputType = GR067[9]; inputCode = GR067[10]; break;
          case 32233 : inputName = GR068[1]; inputAir = GR068[2]; inputEnergy = GR068[3]; inputProtein = GR068[4]; inputLemak = GR068[5]; inputKarbo = GR068[6]; inputSerat = GR068[7]; inputCarbing = GR068[8]; inputType = GR068[9]; inputCode = GR068[10]; break;
          case 32234 : inputName = GR069[1]; inputAir = GR069[2]; inputEnergy = GR069[3]; inputProtein = GR069[4]; inputLemak = GR069[5]; inputKarbo = GR069[6]; inputSerat = GR069[7]; inputCarbing = GR069[8]; inputType = GR069[9]; inputCode = GR069[10]; break;
          case 32235 : inputName = GR070[1]; inputAir = GR070[2]; inputEnergy = GR070[3]; inputProtein = GR070[4]; inputLemak = GR070[5]; inputKarbo = GR070[6]; inputSerat = GR070[7]; inputCarbing = GR070[8]; inputType = GR070[9]; inputCode = GR070[10]; break;
          case 32241 : inputName = GR071[1]; inputAir = GR071[2]; inputEnergy = GR071[3]; inputProtein = GR071[4]; inputLemak = GR071[5]; inputKarbo = GR071[6]; inputSerat = GR071[7]; inputCarbing = GR071[8]; inputType = GR071[9]; inputCode = GR071[10]; break;
          case 32242 : inputName = GR072[1]; inputAir = GR072[2]; inputEnergy = GR072[3]; inputProtein = GR072[4]; inputLemak = GR072[5]; inputKarbo = GR072[6]; inputSerat = GR072[7]; inputCarbing = GR072[8]; inputType = GR072[9]; inputCode = GR072[10]; break;
          case 32243 : inputName = GR073[1]; inputAir = GR073[2]; inputEnergy = GR073[3]; inputProtein = GR073[4]; inputLemak = GR073[5]; inputKarbo = GR073[6]; inputSerat = GR073[7]; inputCarbing = GR073[8]; inputType = GR073[9]; inputCode = GR073[10]; break;
          case 32244 : inputName = GR074[1]; inputAir = GR074[2]; inputEnergy = GR074[3]; inputProtein = GR074[4]; inputLemak = GR074[5]; inputKarbo = GR074[6]; inputSerat = GR074[7]; inputCarbing = GR074[8]; inputType = GR074[9]; inputCode = GR074[10]; break;
          case 32245 : inputName = GR075[1]; inputAir = GR075[2]; inputEnergy = GR075[3]; inputProtein = GR075[4]; inputLemak = GR075[5]; inputKarbo = GR075[6]; inputSerat = GR075[7]; inputCarbing = GR075[8]; inputType = GR075[9]; inputCode = GR075[10]; break;
          case 32251 : inputName = GR076[1]; inputAir = GR076[2]; inputEnergy = GR076[3]; inputProtein = GR076[4]; inputLemak = GR076[5]; inputKarbo = GR076[6]; inputSerat = GR076[7]; inputCarbing = GR076[8]; inputType = GR076[9]; inputCode = GR076[10]; break;
          case 32252 : inputName = GR077[1]; inputAir = GR077[2]; inputEnergy = GR077[3]; inputProtein = GR077[4]; inputLemak = GR077[5]; inputKarbo = GR077[6]; inputSerat = GR077[7]; inputCarbing = GR077[8]; inputType = GR077[9]; inputCode = GR077[10]; break;
          case 32253 : inputName = GR078[1]; inputAir = GR078[2]; inputEnergy = GR078[3]; inputProtein = GR078[4]; inputLemak = GR078[5]; inputKarbo = GR078[6]; inputSerat = GR078[7]; inputCarbing = GR078[8]; inputType = GR078[9]; inputCode = GR078[10]; break;
          case 32254 : inputName = GR079[1]; inputAir = GR079[2]; inputEnergy = GR079[3]; inputProtein = GR079[4]; inputLemak = GR079[5]; inputKarbo = GR079[6]; inputSerat = GR079[7]; inputCarbing = GR079[8]; inputType = GR079[9]; inputCode = GR079[10]; break;
          case 32255 : inputName = GR080[1]; inputAir = GR080[2]; inputEnergy = GR080[3]; inputProtein = GR080[4]; inputLemak = GR080[5]; inputKarbo = GR080[6]; inputSerat = GR080[7]; inputCarbing = GR080[8]; inputType = GR080[9]; inputCode = GR080[10]; break;
          case 32311 : inputName = GR081[1]; inputAir = GR081[2]; inputEnergy = GR081[3]; inputProtein = GR081[4]; inputLemak = GR081[5]; inputKarbo = GR081[6]; inputSerat = GR081[7]; inputCarbing = GR081[8]; inputType = GR081[9]; inputCode = GR081[10]; break;
          case 32312 : inputName = GR082[1]; inputAir = GR082[2]; inputEnergy = GR082[3]; inputProtein = GR082[4]; inputLemak = GR082[5]; inputKarbo = GR082[6]; inputSerat = GR082[7]; inputCarbing = GR082[8]; inputType = GR082[9]; inputCode = GR082[10]; break;
          case 32313 : inputName = GR083[1]; inputAir = GR083[2]; inputEnergy = GR083[3]; inputProtein = GR083[4]; inputLemak = GR083[5]; inputKarbo = GR083[6]; inputSerat = GR083[7]; inputCarbing = GR083[8]; inputType = GR083[9]; inputCode = GR083[10]; break;
          case 32314 : inputName = GR084[1]; inputAir = GR084[2]; inputEnergy = GR084[3]; inputProtein = GR084[4]; inputLemak = GR084[5]; inputKarbo = GR084[6]; inputSerat = GR084[7]; inputCarbing = GR084[8]; inputType = GR084[9]; inputCode = GR084[10]; break;
          case 32315 : inputName = GP001[1]; inputAir = GP001[2]; inputEnergy = GP001[3]; inputProtein = GP001[4]; inputLemak = GP001[5]; inputKarbo = GP001[6]; inputSerat = GP001[7]; inputCarbing = GP001[8]; inputType = GP001[9]; inputCode = GP001[10]; break;
          case 32321 : inputName = GP002[1]; inputAir = GP002[2]; inputEnergy = GP002[3]; inputProtein = GP002[4]; inputLemak = GP002[5]; inputKarbo = GP002[6]; inputSerat = GP002[7]; inputCarbing = GP002[8]; inputType = GP002[9]; inputCode = GP002[10]; break;
          case 32322 : inputName = GP003[1]; inputAir = GP003[2]; inputEnergy = GP003[3]; inputProtein = GP003[4]; inputLemak = GP003[5]; inputKarbo = GP003[6]; inputSerat = GP003[7]; inputCarbing = GP003[8]; inputType = GP003[9]; inputCode = GP003[10]; break;
          case 32323 : inputName = GP004[1]; inputAir = GP004[2]; inputEnergy = GP004[3]; inputProtein = GP004[4]; inputLemak = GP004[5]; inputKarbo = GP004[6]; inputSerat = GP004[7]; inputCarbing = GP004[8]; inputType = GP004[9]; inputCode = GP004[10]; break;
          case 32324 : inputName = GP005[1]; inputAir = GP005[2]; inputEnergy = GP005[3]; inputProtein = GP005[4]; inputLemak = GP005[5]; inputKarbo = GP005[6]; inputSerat = GP005[7]; inputCarbing = GP005[8]; inputType = GP005[9]; inputCode = GP005[10]; break;
          case 32325 : inputName = GP006[1]; inputAir = GP006[2]; inputEnergy = GP006[3]; inputProtein = GP006[4]; inputLemak = GP006[5]; inputKarbo = GP006[6]; inputSerat = GP006[7]; inputCarbing = GP006[8]; inputType = GP006[9]; inputCode = GP006[10]; break;
          case 32331 : inputName = GP007[1]; inputAir = GP007[2]; inputEnergy = GP007[3]; inputProtein = GP007[4]; inputLemak = GP007[5]; inputKarbo = GP007[6]; inputSerat = GP007[7]; inputCarbing = GP007[8]; inputType = GP007[9]; inputCode = GP007[10]; break;
          case 32332 : inputName = GP008[1]; inputAir = GP008[2]; inputEnergy = GP008[3]; inputProtein = GP008[4]; inputLemak = GP008[5]; inputKarbo = GP008[6]; inputSerat = GP008[7]; inputCarbing = GP008[8]; inputType = GP008[9]; inputCode = GP008[10]; break;
          case 32333 : inputName = GP009[1]; inputAir = GP009[2]; inputEnergy = GP009[3]; inputProtein = GP009[4]; inputLemak = GP009[5]; inputKarbo = GP009[6]; inputSerat = GP009[7]; inputCarbing = GP009[8]; inputType = GP009[9]; inputCode = GP009[10]; break;
          case 32334 : inputName = GP010[1]; inputAir = GP010[2]; inputEnergy = GP010[3]; inputProtein = GP010[4]; inputLemak = GP010[5]; inputKarbo = GP010[6]; inputSerat = GP010[7]; inputCarbing = GP010[8]; inputType = GP010[9]; inputCode = GP010[10]; break;
          case 32335 : inputName = GP011[1]; inputAir = GP011[2]; inputEnergy = GP011[3]; inputProtein = GP011[4]; inputLemak = GP011[5]; inputKarbo = GP011[6]; inputSerat = GP011[7]; inputCarbing = GP011[8]; inputType = GP011[9]; inputCode = GP011[10]; break;
          case 32341 : inputName = GP012[1]; inputAir = GP012[2]; inputEnergy = GP012[3]; inputProtein = GP012[4]; inputLemak = GP012[5]; inputKarbo = GP012[6]; inputSerat = GP012[7]; inputCarbing = GP012[8]; inputType = GP012[9]; inputCode = GP012[10]; break;
          case 32342 : inputName = GP013[1]; inputAir = GP013[2]; inputEnergy = GP013[3]; inputProtein = GP013[4]; inputLemak = GP013[5]; inputKarbo = GP013[6]; inputSerat = GP013[7]; inputCarbing = GP013[8]; inputType = GP013[9]; inputCode = GP013[10]; break;
          case 32343 : inputName = GP014[1]; inputAir = GP014[2]; inputEnergy = GP014[3]; inputProtein = GP014[4]; inputLemak = GP014[5]; inputKarbo = GP014[6]; inputSerat = GP014[7]; inputCarbing = GP014[8]; inputType = GP014[9]; inputCode = GP014[10]; break;
          case 32344 : inputName = GP015[1]; inputAir = GP015[2]; inputEnergy = GP015[3]; inputProtein = GP015[4]; inputLemak = GP015[5]; inputKarbo = GP015[6]; inputSerat = GP015[7]; inputCarbing = GP015[8]; inputType = GP015[9]; inputCode = GP015[10]; break;
          case 32345 : inputName = GP016[1]; inputAir = GP016[2]; inputEnergy = GP016[3]; inputProtein = GP016[4]; inputLemak = GP016[5]; inputKarbo = GP016[6]; inputSerat = GP016[7]; inputCarbing = GP016[8]; inputType = GP016[9]; inputCode = GP016[10]; break;
          case 32351 : inputName = GP017[1]; inputAir = GP017[2]; inputEnergy = GP017[3]; inputProtein = GP017[4]; inputLemak = GP017[5]; inputKarbo = GP017[6]; inputSerat = GP017[7]; inputCarbing = GP017[8]; inputType = GP017[9]; inputCode = GP017[10]; break;
          case 32352 : inputName = GP018[1]; inputAir = GP018[2]; inputEnergy = GP018[3]; inputProtein = GP018[4]; inputLemak = GP018[5]; inputKarbo = GP018[6]; inputSerat = GP018[7]; inputCarbing = GP018[8]; inputType = GP018[9]; inputCode = GP018[10]; break;
          case 32353 : inputName = GP019[1]; inputAir = GP019[2]; inputEnergy = GP019[3]; inputProtein = GP019[4]; inputLemak = GP019[5]; inputKarbo = GP019[6]; inputSerat = GP019[7]; inputCarbing = GP019[8]; inputType = GP019[9]; inputCode = GP019[10]; break;
          case 32354 : inputName = GP020[1]; inputAir = GP020[2]; inputEnergy = GP020[3]; inputProtein = GP020[4]; inputLemak = GP020[5]; inputKarbo = GP020[6]; inputSerat = GP020[7]; inputCarbing = GP020[8]; inputType = GP020[9]; inputCode = GP020[10]; break;
          case 32355 : inputName = GP021[1]; inputAir = GP021[2]; inputEnergy = GP021[3]; inputProtein = GP021[4]; inputLemak = GP021[5]; inputKarbo = GP021[6]; inputSerat = GP021[7]; inputCarbing = GP021[8]; inputType = GP021[9]; inputCode = GP021[10]; break;
          case 32411 : inputName = GP022[1]; inputAir = GP022[2]; inputEnergy = GP022[3]; inputProtein = GP022[4]; inputLemak = GP022[5]; inputKarbo = GP022[6]; inputSerat = GP022[7]; inputCarbing = GP022[8]; inputType = GP022[9]; inputCode = GP022[10]; break;
          case 32412 : inputName = GP023[1]; inputAir = GP023[2]; inputEnergy = GP023[3]; inputProtein = GP023[4]; inputLemak = GP023[5]; inputKarbo = GP023[6]; inputSerat = GP023[7]; inputCarbing = GP023[8]; inputType = GP023[9]; inputCode = GP023[10]; break;
          case 32413 : inputName = GP024[1]; inputAir = GP024[2]; inputEnergy = GP024[3]; inputProtein = GP024[4]; inputLemak = GP024[5]; inputKarbo = GP024[6]; inputSerat = GP024[7]; inputCarbing = GP024[8]; inputType = GP024[9]; inputCode = GP024[10]; break;
          case 32414 : inputName = GP025[1]; inputAir = GP025[2]; inputEnergy = GP025[3]; inputProtein = GP025[4]; inputLemak = GP025[5]; inputKarbo = GP025[6]; inputSerat = GP025[7]; inputCarbing = GP025[8]; inputType = GP025[9]; inputCode = GP025[10]; break;
          case 32415 : inputName = GP026[1]; inputAir = GP026[2]; inputEnergy = GP026[3]; inputProtein = GP026[4]; inputLemak = GP026[5]; inputKarbo = GP026[6]; inputSerat = GP026[7]; inputCarbing = GP026[8]; inputType = GP026[9]; inputCode = GP026[10]; break;
          case 32421 : inputName = GP027[1]; inputAir = GP027[2]; inputEnergy = GP027[3]; inputProtein = GP027[4]; inputLemak = GP027[5]; inputKarbo = GP027[6]; inputSerat = GP027[7]; inputCarbing = GP027[8]; inputType = GP027[9]; inputCode = GP027[10]; break;
          case 32422 : inputName = GP028[1]; inputAir = GP028[2]; inputEnergy = GP028[3]; inputProtein = GP028[4]; inputLemak = GP028[5]; inputKarbo = GP028[6]; inputSerat = GP028[7]; inputCarbing = GP028[8]; inputType = GP028[9]; inputCode = GP028[10]; break;
          case 32423 : inputName = GP029[1]; inputAir = GP029[2]; inputEnergy = GP029[3]; inputProtein = GP029[4]; inputLemak = GP029[5]; inputKarbo = GP029[6]; inputSerat = GP029[7]; inputCarbing = GP029[8]; inputType = GP029[9]; inputCode = GP029[10]; break;
          case 32424 : inputName = GP030[1]; inputAir = GP030[2]; inputEnergy = GP030[3]; inputProtein = GP030[4]; inputLemak = GP030[5]; inputKarbo = GP030[6]; inputSerat = GP030[7]; inputCarbing = GP030[8]; inputType = GP030[9]; inputCode = GP030[10]; break;
          case 32425 : inputName = GP031[1]; inputAir = GP031[2]; inputEnergy = GP031[3]; inputProtein = GP031[4]; inputLemak = GP031[5]; inputKarbo = GP031[6]; inputSerat = GP031[7]; inputCarbing = GP031[8]; inputType = GP031[9]; inputCode = GP031[10]; break;
          case 32431 : inputName = GP032[1]; inputAir = GP032[2]; inputEnergy = GP032[3]; inputProtein = GP032[4]; inputLemak = GP032[5]; inputKarbo = GP032[6]; inputSerat = GP032[7]; inputCarbing = GP032[8]; inputType = GP032[9]; inputCode = GP032[10]; break;
          case 32432 : inputName = GP033[1]; inputAir = GP033[2]; inputEnergy = GP033[3]; inputProtein = GP033[4]; inputLemak = GP033[5]; inputKarbo = GP033[6]; inputSerat = GP033[7]; inputCarbing = GP033[8]; inputType = GP033[9]; inputCode = GP033[10]; break;
          case 32433 : inputName = GP034[1]; inputAir = GP034[2]; inputEnergy = GP034[3]; inputProtein = GP034[4]; inputLemak = GP034[5]; inputKarbo = GP034[6]; inputSerat = GP034[7]; inputCarbing = GP034[8]; inputType = GP034[9]; inputCode = GP034[10]; break;
          case 32434 : inputName = GP035[1]; inputAir = GP035[2]; inputEnergy = GP035[3]; inputProtein = GP035[4]; inputLemak = GP035[5]; inputKarbo = GP035[6]; inputSerat = GP035[7]; inputCarbing = GP035[8]; inputType = GP035[9]; inputCode = GP035[10]; break;
          case 32435 : inputName = GP036[1]; inputAir = GP036[2]; inputEnergy = GP036[3]; inputProtein = GP036[4]; inputLemak = GP036[5]; inputKarbo = GP036[6]; inputSerat = GP036[7]; inputCarbing = GP036[8]; inputType = GP036[9]; inputCode = GP036[10]; break;
          case 32441 : inputName = GP037[1]; inputAir = GP037[2]; inputEnergy = GP037[3]; inputProtein = GP037[4]; inputLemak = GP037[5]; inputKarbo = GP037[6]; inputSerat = GP037[7]; inputCarbing = GP037[8]; inputType = GP037[9]; inputCode = GP037[10]; break;
          case 32442 : inputName = GP038[1]; inputAir = GP038[2]; inputEnergy = GP038[3]; inputProtein = GP038[4]; inputLemak = GP038[5]; inputKarbo = GP038[6]; inputSerat = GP038[7]; inputCarbing = GP038[8]; inputType = GP038[9]; inputCode = GP038[10]; break;
          case 32443 : inputName = GP039[1]; inputAir = GP039[2]; inputEnergy = GP039[3]; inputProtein = GP039[4]; inputLemak = GP039[5]; inputKarbo = GP039[6]; inputSerat = GP039[7]; inputCarbing = GP039[8]; inputType = GP039[9]; inputCode = GP039[10]; break;
          case 32444 : inputName = GP040[1]; inputAir = GP040[2]; inputEnergy = GP040[3]; inputProtein = GP040[4]; inputLemak = GP040[5]; inputKarbo = GP040[6]; inputSerat = GP040[7]; inputCarbing = GP040[8]; inputType = GP040[9]; inputCode = GP040[10]; break;
          case 32445 : inputName = GP041[1]; inputAir = GP041[2]; inputEnergy = GP041[3]; inputProtein = GP041[4]; inputLemak = GP041[5]; inputKarbo = GP041[6]; inputSerat = GP041[7]; inputCarbing = GP041[8]; inputType = GP041[9]; inputCode = GP041[10]; break;
          case 32451 : inputName = GP042[1]; inputAir = GP042[2]; inputEnergy = GP042[3]; inputProtein = GP042[4]; inputLemak = GP042[5]; inputKarbo = GP042[6]; inputSerat = GP042[7]; inputCarbing = GP042[8]; inputType = GP042[9]; inputCode = GP042[10]; break;
          case 32452 : inputName = GP043[1]; inputAir = GP043[2]; inputEnergy = GP043[3]; inputProtein = GP043[4]; inputLemak = GP043[5]; inputKarbo = GP043[6]; inputSerat = GP043[7]; inputCarbing = GP043[8]; inputType = GP043[9]; inputCode = GP043[10]; break;
          case 32453 : inputName = GP044[1]; inputAir = GP044[2]; inputEnergy = GP044[3]; inputProtein = GP044[4]; inputLemak = GP044[5]; inputKarbo = GP044[6]; inputSerat = GP044[7]; inputCarbing = GP044[8]; inputType = GP044[9]; inputCode = GP044[10]; break;
          case 32454 : inputName = GP045[1]; inputAir = GP045[2]; inputEnergy = GP045[3]; inputProtein = GP045[4]; inputLemak = GP045[5]; inputKarbo = GP045[6]; inputSerat = GP045[7]; inputCarbing = GP045[8]; inputType = GP045[9]; inputCode = GP045[10]; break;
          case 32455 : inputName = GP046[1]; inputAir = GP046[2]; inputEnergy = GP046[3]; inputProtein = GP046[4]; inputLemak = GP046[5]; inputKarbo = GP046[6]; inputSerat = GP046[7]; inputCarbing = GP046[8]; inputType = GP046[9]; inputCode = GP046[10]; break;
          case 32511 : inputName = GP047[1]; inputAir = GP047[2]; inputEnergy = GP047[3]; inputProtein = GP047[4]; inputLemak = GP047[5]; inputKarbo = GP047[6]; inputSerat = GP047[7]; inputCarbing = GP047[8]; inputType = GP047[9]; inputCode = GP047[10]; break;
          case 32512 : inputName = GP048[1]; inputAir = GP048[2]; inputEnergy = GP048[3]; inputProtein = GP048[4]; inputLemak = GP048[5]; inputKarbo = GP048[6]; inputSerat = GP048[7]; inputCarbing = GP048[8]; inputType = GP048[9]; inputCode = GP048[10]; break;
          case 32513 : inputName = GP049[1]; inputAir = GP049[2]; inputEnergy = GP049[3]; inputProtein = GP049[4]; inputLemak = GP049[5]; inputKarbo = GP049[6]; inputSerat = GP049[7]; inputCarbing = GP049[8]; inputType = GP049[9]; inputCode = GP049[10]; break;
          case 32514 : inputName = GP050[1]; inputAir = GP050[2]; inputEnergy = GP050[3]; inputProtein = GP050[4]; inputLemak = GP050[5]; inputKarbo = GP050[6]; inputSerat = GP050[7]; inputCarbing = GP050[8]; inputType = GP050[9]; inputCode = GP050[10]; break;
          case 32515 : inputName = GP051[1]; inputAir = GP051[2]; inputEnergy = GP051[3]; inputProtein = GP051[4]; inputLemak = GP051[5]; inputKarbo = GP051[6]; inputSerat = GP051[7]; inputCarbing = GP051[8]; inputType = GP051[9]; inputCode = GP051[10]; break;
          case 32521 : inputName = GP052[1]; inputAir = GP052[2]; inputEnergy = GP052[3]; inputProtein = GP052[4]; inputLemak = GP052[5]; inputKarbo = GP052[6]; inputSerat = GP052[7]; inputCarbing = GP052[8]; inputType = GP052[9]; inputCode = GP052[10]; break;
          case 32522 : inputName = GP053[1]; inputAir = GP053[2]; inputEnergy = GP053[3]; inputProtein = GP053[4]; inputLemak = GP053[5]; inputKarbo = GP053[6]; inputSerat = GP053[7]; inputCarbing = GP053[8]; inputType = GP053[9]; inputCode = GP053[10]; break;
          case 32523 : inputName = GP054[1]; inputAir = GP054[2]; inputEnergy = GP054[3]; inputProtein = GP054[4]; inputLemak = GP054[5]; inputKarbo = GP054[6]; inputSerat = GP054[7]; inputCarbing = GP054[8]; inputType = GP054[9]; inputCode = GP054[10]; break;
          case 32524 : inputName = GP055[1]; inputAir = GP055[2]; inputEnergy = GP055[3]; inputProtein = GP055[4]; inputLemak = GP055[5]; inputKarbo = GP055[6]; inputSerat = GP055[7]; inputCarbing = GP055[8]; inputType = GP055[9]; inputCode = GP055[10]; break;
          case 32525 : inputName = GP056[1]; inputAir = GP056[2]; inputEnergy = GP056[3]; inputProtein = GP056[4]; inputLemak = GP056[5]; inputKarbo = GP056[6]; inputSerat = GP056[7]; inputCarbing = GP056[8]; inputType = GP056[9]; inputCode = GP056[10]; break;
          case 32531 : inputName = GP057[1]; inputAir = GP057[2]; inputEnergy = GP057[3]; inputProtein = GP057[4]; inputLemak = GP057[5]; inputKarbo = GP057[6]; inputSerat = GP057[7]; inputCarbing = GP057[8]; inputType = GP057[9]; inputCode = GP057[10]; break;
          case 32532 : inputName = GP058[1]; inputAir = GP058[2]; inputEnergy = GP058[3]; inputProtein = GP058[4]; inputLemak = GP058[5]; inputKarbo = GP058[6]; inputSerat = GP058[7]; inputCarbing = GP058[8]; inputType = GP058[9]; inputCode = GP058[10]; break;
          case 32533 : inputName = GP059[1]; inputAir = GP059[2]; inputEnergy = GP059[3]; inputProtein = GP059[4]; inputLemak = GP059[5]; inputKarbo = GP059[6]; inputSerat = GP059[7]; inputCarbing = GP059[8]; inputType = GP059[9]; inputCode = GP059[10]; break;
          case 32534 : inputName = GP060[1]; inputAir = GP060[2]; inputEnergy = GP060[3]; inputProtein = GP060[4]; inputLemak = GP060[5]; inputKarbo = GP060[6]; inputSerat = GP060[7]; inputCarbing = GP060[8]; inputType = GP060[9]; inputCode = GP060[10]; break;
          case 32535 : inputName = GP061[1]; inputAir = GP061[2]; inputEnergy = GP061[3]; inputProtein = GP061[4]; inputLemak = GP061[5]; inputKarbo = GP061[6]; inputSerat = GP061[7]; inputCarbing = GP061[8]; inputType = GP061[9]; inputCode = GP061[10]; break;
          case 32541 : inputName = GP062[1]; inputAir = GP062[2]; inputEnergy = GP062[3]; inputProtein = GP062[4]; inputLemak = GP062[5]; inputKarbo = GP062[6]; inputSerat = GP062[7]; inputCarbing = GP062[8]; inputType = GP062[9]; inputCode = GP062[10]; break;
          case 32542 : inputName = GP063[1]; inputAir = GP063[2]; inputEnergy = GP063[3]; inputProtein = GP063[4]; inputLemak = GP063[5]; inputKarbo = GP063[6]; inputSerat = GP063[7]; inputCarbing = GP063[8]; inputType = GP063[9]; inputCode = GP063[10]; break;
          case 32543 : inputName = GP065[1]; inputAir = GP065[2]; inputEnergy = GP065[3]; inputProtein = GP065[4]; inputLemak = GP065[5]; inputKarbo = GP065[6]; inputSerat = GP065[7]; inputCarbing = GP065[8]; inputType = GP065[9]; inputCode = GP065[10]; break;
          case 32544 : inputName = GP064[1]; inputAir = GP064[2]; inputEnergy = GP064[3]; inputProtein = GP064[4]; inputLemak = GP064[5]; inputKarbo = GP064[6]; inputSerat = GP064[7]; inputCarbing = GP064[8]; inputType = GP064[9]; inputCode = GP064[10]; break;
          case 32545 : inputName = GP066[1]; inputAir = GP066[2]; inputEnergy = GP066[3]; inputProtein = GP066[4]; inputLemak = GP066[5]; inputKarbo = GP066[6]; inputSerat = GP066[7]; inputCarbing = GP066[8]; inputType = GP066[9]; inputCode = GP066[10]; break;
          case 32551 : inputName = GP067[1]; inputAir = GP067[2]; inputEnergy = GP067[3]; inputProtein = GP067[4]; inputLemak = GP067[5]; inputKarbo = GP067[6]; inputSerat = GP067[7]; inputCarbing = GP067[8]; inputType = GP067[9]; inputCode = GP067[10]; break;
          case 32552 : inputName = GP068[1]; inputAir = GP068[2]; inputEnergy = GP068[3]; inputProtein = GP068[4]; inputLemak = GP068[5]; inputKarbo = GP068[6]; inputSerat = GP068[7]; inputCarbing = GP068[8]; inputType = GP068[9]; inputCode = GP068[10]; break;
          case 32553 : inputName = GP069[1]; inputAir = GP069[2]; inputEnergy = GP069[3]; inputProtein = GP069[4]; inputLemak = GP069[5]; inputKarbo = GP069[6]; inputSerat = GP069[7]; inputCarbing = GP069[8]; inputType = GP069[9]; inputCode = GP069[10]; break;
          case 32554 : inputName = GP070[1]; inputAir = GP070[2]; inputEnergy = GP070[3]; inputProtein = GP070[4]; inputLemak = GP070[5]; inputKarbo = GP070[6]; inputSerat = GP070[7]; inputCarbing = GP070[8]; inputType = GP070[9]; inputCode = GP070[10]; break;
          case 32555 : inputName = GP071[1]; inputAir = GP071[2]; inputEnergy = GP071[3]; inputProtein = GP071[4]; inputLemak = GP071[5]; inputKarbo = GP071[6]; inputSerat = GP071[7]; inputCarbing = GP071[8]; inputType = GP071[9]; inputCode = GP071[10]; break;
          case 321111 : inputName = GP072[1]; inputAir = GP072[2]; inputEnergy = GP072[3]; inputProtein = GP072[4]; inputLemak = GP072[5]; inputKarbo = GP072[6]; inputSerat = GP072[7]; inputCarbing = GP072[8]; inputType = GP072[9]; inputCode = GP072[10]; break;
          case 321112 : inputName = GP073[1]; inputAir = GP073[2]; inputEnergy = GP073[3]; inputProtein = GP073[4]; inputLemak = GP073[5]; inputKarbo = GP073[6]; inputSerat = GP073[7]; inputCarbing = GP073[8]; inputType = GP073[9]; inputCode = GP073[10]; break;
          case 321113 : inputName = GP074[1]; inputAir = GP074[2]; inputEnergy = GP074[3]; inputProtein = GP074[4]; inputLemak = GP074[5]; inputKarbo = GP074[6]; inputSerat = GP074[7]; inputCarbing = GP074[8]; inputType = GP074[9]; inputCode = GP074[10]; break;
          case 321114 : inputName = GP075[1]; inputAir = GP075[2]; inputEnergy = GP075[3]; inputProtein = GP075[4]; inputLemak = GP075[5]; inputKarbo = GP075[6]; inputSerat = GP075[7]; inputCarbing = GP075[8]; inputType = GP075[9]; inputCode = GP075[10]; break;
          case 321115 : inputName = GP076[1]; inputAir = GP076[2]; inputEnergy = GP076[3]; inputProtein = GP076[4]; inputLemak = GP076[5]; inputKarbo = GP076[6]; inputSerat = GP076[7]; inputCarbing = GP076[8]; inputType = GP076[9]; inputCode = GP076[10]; break;
          case 321121 : inputName = GP077[1]; inputAir = GP077[2]; inputEnergy = GP077[3]; inputProtein = GP077[4]; inputLemak = GP077[5]; inputKarbo = GP077[6]; inputSerat = GP077[7]; inputCarbing = GP077[8]; inputType = GP077[9]; inputCode = GP077[10]; break;
          case 321122 : inputName = GP078[1]; inputAir = GP078[2]; inputEnergy = GP078[3]; inputProtein = GP078[4]; inputLemak = GP078[5]; inputKarbo = GP078[6]; inputSerat = GP078[7]; inputCarbing = GP078[8]; inputType = GP078[9]; inputCode = GP078[10]; break;
          case 321123 : inputName = GP079[1]; inputAir = GP079[2]; inputEnergy = GP079[3]; inputProtein = GP079[4]; inputLemak = GP079[5]; inputKarbo = GP079[6]; inputSerat = GP079[7]; inputCarbing = GP079[8]; inputType = GP079[9]; inputCode = GP079[10]; break;
          case 321124 : inputName = GP080[1]; inputAir = GP080[2]; inputEnergy = GP080[3]; inputProtein = GP080[4]; inputLemak = GP080[5]; inputKarbo = GP080[6]; inputSerat = GP080[7]; inputCarbing = GP080[8]; inputType = GP080[9]; inputCode = GP080[10]; break;
          case 321125 : inputName = GP081[1]; inputAir = GP081[2]; inputEnergy = GP081[3]; inputProtein = GP081[4]; inputLemak = GP081[5]; inputKarbo = GP081[6]; inputSerat = GP081[7]; inputCarbing = GP081[8]; inputType = GP081[9]; inputCode = GP081[10]; break;
          case 321131 : inputName = GP082[1]; inputAir = GP082[2]; inputEnergy = GP082[3]; inputProtein = GP082[4]; inputLemak = GP082[5]; inputKarbo = GP082[6]; inputSerat = GP082[7]; inputCarbing = GP082[8]; inputType = GP082[9]; inputCode = GP082[10]; break;
          case 321132 : inputName = GP083[1]; inputAir = GP083[2]; inputEnergy = GP083[3]; inputProtein = GP083[4]; inputLemak = GP083[5]; inputKarbo = GP083[6]; inputSerat = GP083[7]; inputCarbing = GP083[8]; inputType = GP083[9]; inputCode = GP083[10]; break;
          case 321133 : inputName = GP084[1]; inputAir = GP084[2]; inputEnergy = GP084[3]; inputProtein = GP084[4]; inputLemak = GP084[5]; inputKarbo = GP084[6]; inputSerat = GP084[7]; inputCarbing = GP084[8]; inputType = GP084[9]; inputCode = GP084[10]; break;
          case 321134 : inputName = GP085[1]; inputAir = GP085[2]; inputEnergy = GP085[3]; inputProtein = GP085[4]; inputLemak = GP085[5]; inputKarbo = GP085[6]; inputSerat = GP085[7]; inputCarbing = GP085[8]; inputType = GP085[9]; inputCode = GP085[10]; break;
          case 321135 : inputName = GP086[1]; inputAir = GP086[2]; inputEnergy = GP086[3]; inputProtein = GP086[4]; inputLemak = GP086[5]; inputKarbo = GP086[6]; inputSerat = GP086[7]; inputCarbing = GP086[8]; inputType = GP086[9]; inputCode = GP086[10]; break;
          case 321141 : inputName = GP087[1]; inputAir = GP087[2]; inputEnergy = GP087[3]; inputProtein = GP087[4]; inputLemak = GP087[5]; inputKarbo = GP087[6]; inputSerat = GP087[7]; inputCarbing = GP087[8]; inputType = GP087[9]; inputCode = GP087[10]; break;
          case 321142 : inputName = GP088[1]; inputAir = GP088[2]; inputEnergy = GP088[3]; inputProtein = GP088[4]; inputLemak = GP088[5]; inputKarbo = GP088[6]; inputSerat = GP088[7]; inputCarbing = GP088[8]; inputType = GP088[9]; inputCode = GP088[10]; break;
          case 321143 : inputName = GP089[1]; inputAir = GP089[2]; inputEnergy = GP089[3]; inputProtein = GP089[4]; inputLemak = GP089[5]; inputKarbo = GP089[6]; inputSerat = GP089[7]; inputCarbing = GP089[8]; inputType = GP089[9]; inputCode = GP089[10]; break;
          case 321144 : inputName = GP090[1]; inputAir = GP090[2]; inputEnergy = GP090[3]; inputProtein = GP090[4]; inputLemak = GP090[5]; inputKarbo = GP090[6]; inputSerat = GP090[7]; inputCarbing = GP090[8]; inputType = GP090[9]; inputCode = GP090[10]; break;
          case 321145 : inputName = GP091[1]; inputAir = GP091[2]; inputEnergy = GP091[3]; inputProtein = GP091[4]; inputLemak = GP091[5]; inputKarbo = GP091[6]; inputSerat = GP091[7]; inputCarbing = GP091[8]; inputType = GP091[9]; inputCode = GP091[10]; break;
          case 321151 : inputName = GP092[1]; inputAir = GP092[2]; inputEnergy = GP092[3]; inputProtein = GP092[4]; inputLemak = GP092[5]; inputKarbo = GP092[6]; inputSerat = GP092[7]; inputCarbing = GP092[8]; inputType = GP092[9]; inputCode = GP092[10]; break;
          case 321152 : inputName = GP093[1]; inputAir = GP093[2]; inputEnergy = GP093[3]; inputProtein = GP093[4]; inputLemak = GP093[5]; inputKarbo = GP093[6]; inputSerat = GP093[7]; inputCarbing = GP093[8]; inputType = GP093[9]; inputCode = GP093[10]; break;
          case 321153 : inputName = GP094[1]; inputAir = GP094[2]; inputEnergy = GP094[3]; inputProtein = GP094[4]; inputLemak = GP094[5]; inputKarbo = GP094[6]; inputSerat = GP094[7]; inputCarbing = GP094[8]; inputType = GP094[9]; inputCode = GP094[10]; break;
          case 321154 : inputName = GP095[1]; inputAir = GP095[2]; inputEnergy = GP095[3]; inputProtein = GP095[4]; inputLemak = GP095[5]; inputKarbo = GP095[6]; inputSerat = GP095[7]; inputCarbing = GP095[8]; inputType = GP095[9]; inputCode = GP095[10]; break;
          case 511 : inputName = HR001[1]; inputAir = HR001[2]; inputEnergy = HR001[3]; inputProtein = HR001[4]; inputLemak = HR001[5]; inputKarbo = HR001[6]; inputSerat = HR001[7]; inputCarbing = HR001[8]; inputType = HR001[9]; inputCode = HR001[10]; break;
          case 512 : inputName = HR002[1]; inputAir = HR002[2]; inputEnergy = HR002[3]; inputProtein = HR002[4]; inputLemak = HR002[5]; inputKarbo = HR002[6]; inputSerat = HR002[7]; inputCarbing = HR002[8]; inputType = HR002[9]; inputCode = HR002[10]; break;
          case 513 : inputName = HR003[1]; inputAir = HR003[2]; inputEnergy = HR003[3]; inputProtein = HR003[4]; inputLemak = HR003[5]; inputKarbo = HR003[6]; inputSerat = HR003[7]; inputCarbing = HR003[8]; inputType = HR003[9]; inputCode = HR003[10]; break;
          case 514 : inputName = HR004[1]; inputAir = HR004[2]; inputEnergy = HR004[3]; inputProtein = HR004[4]; inputLemak = HR004[5]; inputKarbo = HR004[6]; inputSerat = HR004[7]; inputCarbing = HR004[8]; inputType = HR004[9]; inputCode = HR004[10]; break;
          case 515 : inputName = HR005[1]; inputAir = HR005[2]; inputEnergy = HR005[3]; inputProtein = HR005[4]; inputLemak = HR005[5]; inputKarbo = HR005[6]; inputSerat = HR005[7]; inputCarbing = HR005[8]; inputType = HR005[9]; inputCode = HR005[10]; break;
          case 5111 : inputName = HR006[1]; inputAir = HR006[2]; inputEnergy = HR006[3]; inputProtein = HR006[4]; inputLemak = HR006[5]; inputKarbo = HR006[6]; inputSerat = HR006[7]; inputCarbing = HR006[8]; inputType = HR006[9]; inputCode = HR006[10]; break;
          case 5112 : inputName = HR007[1]; inputAir = HR007[2]; inputEnergy = HR007[3]; inputProtein = HR007[4]; inputLemak = HR007[5]; inputKarbo = HR007[6]; inputSerat = HR007[7]; inputCarbing = HR007[8]; inputType = HR007[9]; inputCode = HR007[10]; break;
          case 5113 : inputName = HR008[1]; inputAir = HR008[2]; inputEnergy = HR008[3]; inputProtein = HR008[4]; inputLemak = HR008[5]; inputKarbo = HR008[6]; inputSerat = HR008[7]; inputCarbing = HR008[8]; inputType = HR008[9]; inputCode = HR008[10]; break;
          case 5114 : inputName = HR009[1]; inputAir = HR009[2]; inputEnergy = HR009[3]; inputProtein = HR009[4]; inputLemak = HR009[5]; inputKarbo = HR009[6]; inputSerat = HR009[7]; inputCarbing = HR009[8]; inputType = HR009[9]; inputCode = HR009[10]; break;
          case 5115 : inputName = HR010[1]; inputAir = HR010[2]; inputEnergy = HR010[3]; inputProtein = HR010[4]; inputLemak = HR010[5]; inputKarbo = HR010[6]; inputSerat = HR010[7]; inputCarbing = HR010[8]; inputType = HR010[9]; inputCode = HR010[10]; break;
          case 5121 : inputName = HR011[1]; inputAir = HR011[2]; inputEnergy = HR011[3]; inputProtein = HR011[4]; inputLemak = HR011[5]; inputKarbo = HR011[6]; inputSerat = HR011[7]; inputCarbing = HR011[8]; inputType = HR011[9]; inputCode = HR011[10]; break;
          case 5122 : inputName = HP001[1]; inputAir = HP001[2]; inputEnergy = HP001[3]; inputProtein = HP001[4]; inputLemak = HP001[5]; inputKarbo = HP001[6]; inputSerat = HP001[7]; inputCarbing = HP001[8]; inputType = HP001[9]; inputCode = HP001[10]; break;
          case 5123 : inputName = HP002[1]; inputAir = HP002[2]; inputEnergy = HP002[3]; inputProtein = HP002[4]; inputLemak = HP002[5]; inputKarbo = HP002[6]; inputSerat = HP002[7]; inputCarbing = HP002[8]; inputType = HP002[9]; inputCode = HP002[10]; break;
          case 5124 : inputName = HP003[1]; inputAir = HP003[2]; inputEnergy = HP003[3]; inputProtein = HP003[4]; inputLemak = HP003[5]; inputKarbo = HP003[6]; inputSerat = HP003[7]; inputCarbing = HP003[8]; inputType = HP003[9]; inputCode = HP003[10]; break;
          case 5125 : inputName = HP004[1]; inputAir = HP004[2]; inputEnergy = HP004[3]; inputProtein = HP004[4]; inputLemak = HP004[5]; inputKarbo = HP004[6]; inputSerat = HP004[7]; inputCarbing = HP004[8]; inputType = HP004[9]; inputCode = HP004[10]; break;
          case 5131 : inputName = HP005[1]; inputAir = HP005[2]; inputEnergy = HP005[3]; inputProtein = HP005[4]; inputLemak = HP005[5]; inputKarbo = HP005[6]; inputSerat = HP005[7]; inputCarbing = HP005[8]; inputType = HP005[9]; inputCode = HP005[10]; break;
          case 5132 : inputName = HP006[1]; inputAir = HP006[2]; inputEnergy = HP006[3]; inputProtein = HP006[4]; inputLemak = HP006[5]; inputKarbo = HP006[6]; inputSerat = HP006[7]; inputCarbing = HP006[8]; inputType = HP006[9]; inputCode = HP006[10]; break;
          case 5133 : inputName = HP007[1]; inputAir = HP007[2]; inputEnergy = HP007[3]; inputProtein = HP007[4]; inputLemak = HP007[5]; inputKarbo = HP007[6]; inputSerat = HP007[7]; inputCarbing = HP007[8]; inputType = HP007[9]; inputCode = HP007[10]; break;
          case 851 : inputName = JR001[1]; inputAir = JR001[2]; inputEnergy = JR001[3]; inputProtein = JR001[4]; inputLemak = JR001[5]; inputKarbo = JR001[6]; inputSerat = JR001[7]; inputCarbing = JR001[8]; inputType = JR001[9]; inputCode = JR001[10]; break;
          case 852 : inputName = JR002[1]; inputAir = JR002[2]; inputEnergy = JR002[3]; inputProtein = JR002[4]; inputLemak = JR002[5]; inputKarbo = JR002[6]; inputSerat = JR002[7]; inputCarbing = JR002[8]; inputType = JR002[9]; inputCode = JR002[10]; break;
          case 853 : inputName = JR003[1]; inputAir = JR003[2]; inputEnergy = JR003[3]; inputProtein = JR003[4]; inputLemak = JR003[5]; inputKarbo = JR003[6]; inputSerat = JR003[7]; inputCarbing = JR003[8]; inputType = JR003[9]; inputCode = JR003[10]; break;
          case 854 : inputName = JR004[1]; inputAir = JR004[2]; inputEnergy = JR004[3]; inputProtein = JR004[4]; inputLemak = JR004[5]; inputKarbo = JR004[6]; inputSerat = JR004[7]; inputCarbing = JR004[8]; inputType = JR004[9]; inputCode = JR004[10]; break;
          case 855 : inputName = JR005[1]; inputAir = JR005[2]; inputEnergy = JR005[3]; inputProtein = JR005[4]; inputLemak = JR005[5]; inputKarbo = JR005[6]; inputSerat = JR005[7]; inputCarbing = JR005[8]; inputType = JR005[9]; inputCode = JR005[10]; break;
          case 8511 : inputName = JR006[1]; inputAir = JR006[2]; inputEnergy = JR006[3]; inputProtein = JR006[4]; inputLemak = JR006[5]; inputKarbo = JR006[6]; inputSerat = JR006[7]; inputCarbing = JR006[8]; inputType = JR006[9]; inputCode = JR006[10]; break;
          case 8512 : inputName = JP001[1]; inputAir = JP001[2]; inputEnergy = JP001[3]; inputProtein = JP001[4]; inputLemak = JP001[5]; inputKarbo = JP001[6]; inputSerat = JP001[7]; inputCarbing = JP001[8]; inputType = JP001[9]; inputCode = JP001[10]; break;
          case 8513 : inputName = JP002[1]; inputAir = JP002[2]; inputEnergy = JP002[3]; inputProtein = JP002[4]; inputLemak = JP002[5]; inputKarbo = JP002[6]; inputSerat = JP002[7]; inputCarbing = JP002[8]; inputType = JP002[9]; inputCode = JP002[10]; break;
          case 8514 : inputName = JP003[1]; inputAir = JP003[2]; inputEnergy = JP003[3]; inputProtein = JP003[4]; inputLemak = JP003[5]; inputKarbo = JP003[6]; inputSerat = JP003[7]; inputCarbing = JP003[8]; inputType = JP003[9]; inputCode = JP003[10]; break;
          case 8515 : inputName = JP004[1]; inputAir = JP004[2]; inputEnergy = JP004[3]; inputProtein = JP004[4]; inputLemak = JP004[5]; inputKarbo = JP004[6]; inputSerat = JP004[7]; inputCarbing = JP004[8]; inputType = JP004[9]; inputCode = JP004[10]; break;
          case 8521 : inputName = JP005[1]; inputAir = JP005[2]; inputEnergy = JP005[3]; inputProtein = JP005[4]; inputLemak = JP005[5]; inputKarbo = JP005[6]; inputSerat = JP005[7]; inputCarbing = JP005[8]; inputType = JP005[9]; inputCode = JP005[10]; break;
          case 8522 : inputName = JP006[1]; inputAir = JP006[2]; inputEnergy = JP006[3]; inputProtein = JP006[4]; inputLemak = JP006[5]; inputKarbo = JP006[6]; inputSerat = JP006[7]; inputCarbing = JP006[8]; inputType = JP006[9]; inputCode = JP006[10]; break;
          case 8523 : inputName = JP007[1]; inputAir = JP007[2]; inputEnergy = JP007[3]; inputProtein = JP007[4]; inputLemak = JP007[5]; inputKarbo = JP007[6]; inputSerat = JP007[7]; inputCarbing = JP007[8]; inputType = JP007[9]; inputCode = JP007[10]; break;
          case 8524 : inputName = JP008[1]; inputAir = JP008[2]; inputEnergy = JP008[3]; inputProtein = JP008[4]; inputLemak = JP008[5]; inputKarbo = JP008[6]; inputSerat = JP008[7]; inputCarbing = JP008[8]; inputType = JP008[9]; inputCode = JP008[10]; break;
          case 8525 : inputName = JP009[1]; inputAir = JP009[2]; inputEnergy = JP009[3]; inputProtein = JP009[4]; inputLemak = JP009[5]; inputKarbo = JP009[6]; inputSerat = JP009[7]; inputCarbing = JP009[8]; inputType = JP009[9]; inputCode = JP009[10]; break;
          case 8531 : inputName = JP010[1]; inputAir = JP010[2]; inputEnergy = JP010[3]; inputProtein = JP010[4]; inputLemak = JP010[5]; inputKarbo = JP010[6]; inputSerat = JP010[7]; inputCarbing = JP010[8]; inputType = JP010[9]; inputCode = JP010[10]; break;
          case 8532 : inputName = JP011[1]; inputAir = JP011[2]; inputEnergy = JP011[3]; inputProtein = JP011[4]; inputLemak = JP011[5]; inputKarbo = JP011[6]; inputSerat = JP011[7]; inputCarbing = JP011[8]; inputType = JP011[9]; inputCode = JP011[10]; break;
          case 341 : inputName = KR001[1]; inputAir = KR001[2]; inputEnergy = KR001[3]; inputProtein = KR001[4]; inputLemak = KR001[5]; inputKarbo = KR001[6]; inputSerat = KR001[7]; inputCarbing = KR001[8]; inputType = KR001[9]; inputCode = KR001[10]; break;
          case 342 : inputName = KR002[1]; inputAir = KR002[2]; inputEnergy = KR002[3]; inputProtein = KR002[4]; inputLemak = KR002[5]; inputKarbo = KR002[6]; inputSerat = KR002[7]; inputCarbing = KR002[8]; inputType = KR002[9]; inputCode = KR002[10]; break;
          case 343 : inputName = KR003[1]; inputAir = KR003[2]; inputEnergy = KR003[3]; inputProtein = KR003[4]; inputLemak = KR003[5]; inputKarbo = KR003[6]; inputSerat = KR003[7]; inputCarbing = KR003[8]; inputType = KR003[9]; inputCode = KR003[10]; break;
          case 344 : inputName = KR004[1]; inputAir = KR004[2]; inputEnergy = KR004[3]; inputProtein = KR004[4]; inputLemak = KR004[5]; inputKarbo = KR004[6]; inputSerat = KR004[7]; inputCarbing = KR004[8]; inputType = KR004[9]; inputCode = KR004[10]; break;
          case 345 : inputName = KR005[1]; inputAir = KR005[2]; inputEnergy = KR005[3]; inputProtein = KR005[4]; inputLemak = KR005[5]; inputKarbo = KR005[6]; inputSerat = KR005[7]; inputCarbing = KR005[8]; inputType = KR005[9]; inputCode = KR005[10]; break;
          case 3411 : inputName = KR006[1]; inputAir = KR006[2]; inputEnergy = KR006[3]; inputProtein = KR006[4]; inputLemak = KR006[5]; inputKarbo = KR006[6]; inputSerat = KR006[7]; inputCarbing = KR006[8]; inputType = KR006[9]; inputCode = KR006[10]; break;
          case 3412 : inputName = KR007[1]; inputAir = KR007[2]; inputEnergy = KR007[3]; inputProtein = KR007[4]; inputLemak = KR007[5]; inputKarbo = KR007[6]; inputSerat = KR007[7]; inputCarbing = KR007[8]; inputType = KR007[9]; inputCode = KR007[10]; break;
          case 3413 : inputName = KR008[1]; inputAir = KR008[2]; inputEnergy = KR008[3]; inputProtein = KR008[4]; inputLemak = KR008[5]; inputKarbo = KR008[6]; inputSerat = KR008[7]; inputCarbing = KR008[8]; inputType = KR008[9]; inputCode = KR008[10]; break;
          case 3414 : inputName = KR009[1]; inputAir = KR009[2]; inputEnergy = KR009[3]; inputProtein = KR009[4]; inputLemak = KR009[5]; inputKarbo = KR009[6]; inputSerat = KR009[7]; inputCarbing = KR009[8]; inputType = KR009[9]; inputCode = KR009[10]; break;
          case 3415 : inputName = KR010[1]; inputAir = KR010[2]; inputEnergy = KR010[3]; inputProtein = KR010[4]; inputLemak = KR010[5]; inputKarbo = KR010[6]; inputSerat = KR010[7]; inputCarbing = KR010[8]; inputType = KR010[9]; inputCode = KR010[10]; break;
          case 3421 : inputName = KR011[1]; inputAir = KR011[2]; inputEnergy = KR011[3]; inputProtein = KR011[4]; inputLemak = KR011[5]; inputKarbo = KR011[6]; inputSerat = KR011[7]; inputCarbing = KR011[8]; inputType = KR011[9]; inputCode = KR011[10]; break;
          case 3422 : inputName = KR012[1]; inputAir = KR012[2]; inputEnergy = KR012[3]; inputProtein = KR012[4]; inputLemak = KR012[5]; inputKarbo = KR012[6]; inputSerat = KR012[7]; inputCarbing = KR012[8]; inputType = KR012[9]; inputCode = KR012[10]; break;
          case 3423 : inputName = KR013[1]; inputAir = KR013[2]; inputEnergy = KR013[3]; inputProtein = KR013[4]; inputLemak = KR013[5]; inputKarbo = KR013[6]; inputSerat = KR013[7]; inputCarbing = KR013[8]; inputType = KR013[9]; inputCode = KR013[10]; break;
          case 3424 : inputName = KR014[1]; inputAir = KR014[2]; inputEnergy = KR014[3]; inputProtein = KR014[4]; inputLemak = KR014[5]; inputKarbo = KR014[6]; inputSerat = KR014[7]; inputCarbing = KR014[8]; inputType = KR014[9]; inputCode = KR014[10]; break;
          case 3425 : inputName = KP001[1]; inputAir = KP001[2]; inputEnergy = KP001[3]; inputProtein = KP001[4]; inputLemak = KP001[5]; inputKarbo = KP001[6]; inputSerat = KP001[7]; inputCarbing = KP001[8]; inputType = KP001[9]; inputCode = KP001[10]; break;
          case 3431 : inputName = KP002[1]; inputAir = KP002[2]; inputEnergy = KP002[3]; inputProtein = KP002[4]; inputLemak = KP002[5]; inputKarbo = KP002[6]; inputSerat = KP002[7]; inputCarbing = KP002[8]; inputType = KP002[9]; inputCode = KP002[10]; break;
          case 3432 : inputName = KP003[1]; inputAir = KP003[2]; inputEnergy = KP003[3]; inputProtein = KP003[4]; inputLemak = KP003[5]; inputKarbo = KP003[6]; inputSerat = KP003[7]; inputCarbing = KP003[8]; inputType = KP003[9]; inputCode = KP003[10]; break;
          case 3433 : inputName = KP004[1]; inputAir = KP004[2]; inputEnergy = KP004[3]; inputProtein = KP004[4]; inputLemak = KP004[5]; inputKarbo = KP004[6]; inputSerat = KP004[7]; inputCarbing = KP004[8]; inputType = KP004[9]; inputCode = KP004[10]; break;
          case 251 : inputName = MP001[1]; inputAir = MP001[2]; inputEnergy = MP001[3]; inputProtein = MP001[4]; inputLemak = MP001[5]; inputKarbo = MP001[6]; inputSerat = MP001[7]; inputCarbing = MP001[8]; inputType = MP001[9]; inputCode = MP001[10]; break;
          case 252 : inputName = MP002[1]; inputAir = MP002[2]; inputEnergy = MP002[3]; inputProtein = MP002[4]; inputLemak = MP002[5]; inputKarbo = MP002[6]; inputSerat = MP002[7]; inputCarbing = MP002[8]; inputType = MP002[9]; inputCode = MP002[10]; break;
          case 253 : inputName = MP003[1]; inputAir = MP003[2]; inputEnergy = MP003[3]; inputProtein = MP003[4]; inputLemak = MP003[5]; inputKarbo = MP003[6]; inputSerat = MP003[7]; inputCarbing = MP003[8]; inputType = MP003[9]; inputCode = MP003[10]; break;
          case 254 : inputName = MP004[1]; inputAir = MP004[2]; inputEnergy = MP004[3]; inputProtein = MP004[4]; inputLemak = MP004[5]; inputKarbo = MP004[6]; inputSerat = MP004[7]; inputCarbing = MP004[8]; inputType = MP004[9]; inputCode = MP004[10]; break;
          case 255 : inputName = MP005[1]; inputAir = MP005[2]; inputEnergy = MP005[3]; inputProtein = MP005[4]; inputLemak = MP005[5]; inputKarbo = MP005[6]; inputSerat = MP005[7]; inputCarbing = MP005[8]; inputType = MP005[9]; inputCode = MP005[10]; break;
          case 2511 : inputName = MP006[1]; inputAir = MP006[2]; inputEnergy = MP006[3]; inputProtein = MP006[4]; inputLemak = MP006[5]; inputKarbo = MP006[6]; inputSerat = MP006[7]; inputCarbing = MP006[8]; inputType = MP006[9]; inputCode = MP006[10]; break;
          case 2512 : inputName = MP007[1]; inputAir = MP007[2]; inputEnergy = MP007[3]; inputProtein = MP007[4]; inputLemak = MP007[5]; inputKarbo = MP007[6]; inputSerat = MP007[7]; inputCarbing = MP007[8]; inputType = MP007[9]; inputCode = MP007[10]; break;
          case 2513 : inputName = MP008[1]; inputAir = MP008[2]; inputEnergy = MP008[3]; inputProtein = MP008[4]; inputLemak = MP008[5]; inputKarbo = MP008[6]; inputSerat = MP008[7]; inputCarbing = MP008[8]; inputType = MP008[9]; inputCode = MP008[10]; break;
          case 2514 : inputName = MP009[1]; inputAir = MP009[2]; inputEnergy = MP009[3]; inputProtein = MP009[4]; inputLemak = MP009[5]; inputKarbo = MP009[6]; inputSerat = MP009[7]; inputCarbing = MP009[8]; inputType = MP009[9]; inputCode = MP009[10]; break;
          case 2515 : inputName = MP010[1]; inputAir = MP010[2]; inputEnergy = MP010[3]; inputProtein = MP010[4]; inputLemak = MP010[5]; inputKarbo = MP010[6]; inputSerat = MP010[7]; inputCarbing = MP010[8]; inputType = MP010[9]; inputCode = MP010[10]; break;
          case 2521 : inputName = MP011[1]; inputAir = MP011[2]; inputEnergy = MP011[3]; inputProtein = MP011[4]; inputLemak = MP011[5]; inputKarbo = MP011[6]; inputSerat = MP011[7]; inputCarbing = MP011[8]; inputType = MP011[9]; inputCode = MP011[10]; break;
          case 2522 : inputName = MP012[1]; inputAir = MP012[2]; inputEnergy = MP012[3]; inputProtein = MP012[4]; inputLemak = MP012[5]; inputKarbo = MP012[6]; inputSerat = MP012[7]; inputCarbing = MP012[8]; inputType = MP012[9]; inputCode = MP012[10]; break;
          case 2523 : inputName = MP013[1]; inputAir = MP013[2]; inputEnergy = MP013[3]; inputProtein = MP013[4]; inputLemak = MP013[5]; inputKarbo = MP013[6]; inputSerat = MP013[7]; inputCarbing = MP013[8]; inputType = MP013[9]; inputCode = MP013[10]; break;
          case 2524 : inputName = MP014[1]; inputAir = MP014[2]; inputEnergy = MP014[3]; inputProtein = MP014[4]; inputLemak = MP014[5]; inputKarbo = MP014[6]; inputSerat = MP014[7]; inputCarbing = MP014[8]; inputType = MP014[9]; inputCode = MP014[10]; break;
          case 2525 : inputName = MP015[1]; inputAir = MP015[2]; inputEnergy = MP015[3]; inputProtein = MP015[4]; inputLemak = MP015[5]; inputKarbo = MP015[6]; inputSerat = MP015[7]; inputCarbing = MP015[8]; inputType = MP015[9]; inputCode = MP015[10]; break;
          case 2531 : inputName = MP016[1]; inputAir = MP016[2]; inputEnergy = MP016[3]; inputProtein = MP016[4]; inputLemak = MP016[5]; inputKarbo = MP016[6]; inputSerat = MP016[7]; inputCarbing = MP016[8]; inputType = MP016[9]; inputCode = MP016[10]; break;
          case 2532 : inputName = MP017[1]; inputAir = MP017[2]; inputEnergy = MP017[3]; inputProtein = MP017[4]; inputLemak = MP017[5]; inputKarbo = MP017[6]; inputSerat = MP017[7]; inputCarbing = MP017[8]; inputType = MP017[9]; inputCode = MP017[10]; break;
          case 2533 : inputName = MP018[1]; inputAir = MP018[2]; inputEnergy = MP018[3]; inputProtein = MP018[4]; inputLemak = MP018[5]; inputKarbo = MP018[6]; inputSerat = MP018[7]; inputCarbing = MP018[8]; inputType = MP018[9]; inputCode = MP018[10]; break;
          case 221 : inputName = NR001[1]; inputAir = NR001[2]; inputEnergy = NR001[3]; inputProtein = NR001[4]; inputLemak = NR001[5]; inputKarbo = NR001[6]; inputSerat = NR001[7]; inputCarbing = NR001[8]; inputType = NR001[9]; inputCode = NR001[10]; break;
          case 222 : inputName = NR002[1]; inputAir = NR002[2]; inputEnergy = NR002[3]; inputProtein = NR002[4]; inputLemak = NR002[5]; inputKarbo = NR002[6]; inputSerat = NR002[7]; inputCarbing = NR002[8]; inputType = NR002[9]; inputCode = NR002[10]; break;
          case 223 : inputName = NR003[1]; inputAir = NR003[2]; inputEnergy = NR003[3]; inputProtein = NR003[4]; inputLemak = NR003[5]; inputKarbo = NR003[6]; inputSerat = NR003[7]; inputCarbing = NR003[8]; inputType = NR003[9]; inputCode = NR003[10]; break;
          case 224 : inputName = NR004[1]; inputAir = NR004[2]; inputEnergy = NR004[3]; inputProtein = NR004[4]; inputLemak = NR004[5]; inputKarbo = NR004[6]; inputSerat = NR004[7]; inputCarbing = NR004[8]; inputType = NR004[9]; inputCode = NR004[10]; break;
          case 225 : inputName = NR005[1]; inputAir = NR005[2]; inputEnergy = NR005[3]; inputProtein = NR005[4]; inputLemak = NR005[5]; inputKarbo = NR005[6]; inputSerat = NR005[7]; inputCarbing = NR005[8]; inputType = NR005[9]; inputCode = NR005[10]; break;
          case 2211 : inputName = NR006[1]; inputAir = NR006[2]; inputEnergy = NR006[3]; inputProtein = NR006[4]; inputLemak = NR006[5]; inputKarbo = NR006[6]; inputSerat = NR006[7]; inputCarbing = NR006[8]; inputType = NR006[9]; inputCode = NR006[10]; break;
          case 2212 : inputName = NR007[1]; inputAir = NR007[2]; inputEnergy = NR007[3]; inputProtein = NR007[4]; inputLemak = NR007[5]; inputKarbo = NR007[6]; inputSerat = NR007[7]; inputCarbing = NR007[8]; inputType = NR007[9]; inputCode = NR007[10]; break;
          case 2213 : inputName = NR008[1]; inputAir = NR008[2]; inputEnergy = NR008[3]; inputProtein = NR008[4]; inputLemak = NR008[5]; inputKarbo = NR008[6]; inputSerat = NR008[7]; inputCarbing = NR008[8]; inputType = NR008[9]; inputCode = NR008[10]; break;
          case 2214 : inputName = NR009[1]; inputAir = NR009[2]; inputEnergy = NR009[3]; inputProtein = NR009[4]; inputLemak = NR009[5]; inputKarbo = NR009[6]; inputSerat = NR009[7]; inputCarbing = NR009[8]; inputType = NR009[9]; inputCode = NR009[10]; break;
          case 2215 : inputName = NR010[1]; inputAir = NR010[2]; inputEnergy = NR010[3]; inputProtein = NR010[4]; inputLemak = NR010[5]; inputKarbo = NR010[6]; inputSerat = NR010[7]; inputCarbing = NR010[8]; inputType = NR010[9]; inputCode = NR010[10]; break;
          case 2221 : inputName = NR011[1]; inputAir = NR011[2]; inputEnergy = NR011[3]; inputProtein = NR011[4]; inputLemak = NR011[5]; inputKarbo = NR011[6]; inputSerat = NR011[7]; inputCarbing = NR011[8]; inputType = NR011[9]; inputCode = NR011[10]; break;
          case 2222 : inputName = NR012[1]; inputAir = NR012[2]; inputEnergy = NR012[3]; inputProtein = NR012[4]; inputLemak = NR012[5]; inputKarbo = NR012[6]; inputSerat = NR012[7]; inputCarbing = NR012[8]; inputType = NR012[9]; inputCode = NR012[10]; break;
          case 2223 : inputName = NR013[1]; inputAir = NR013[2]; inputEnergy = NR013[3]; inputProtein = NR013[4]; inputLemak = NR013[5]; inputKarbo = NR013[6]; inputSerat = NR013[7]; inputCarbing = NR013[8]; inputType = NR013[9]; inputCode = NR013[10]; break;
          case 2224 : inputName = NR014[1]; inputAir = NR014[2]; inputEnergy = NR014[3]; inputProtein = NR014[4]; inputLemak = NR014[5]; inputKarbo = NR014[6]; inputSerat = NR014[7]; inputCarbing = NR014[8]; inputType = NR014[9]; inputCode = NR014[10]; break;
          case 2225 : inputName = NR015[1]; inputAir = NR015[2]; inputEnergy = NR015[3]; inputProtein = NR015[4]; inputLemak = NR015[5]; inputKarbo = NR015[6]; inputSerat = NR015[7]; inputCarbing = NR015[8]; inputType = NR015[9]; inputCode = NR015[10]; break;
          case 2231 : inputName = NR016[1]; inputAir = NR016[2]; inputEnergy = NR016[3]; inputProtein = NR016[4]; inputLemak = NR016[5]; inputKarbo = NR016[6]; inputSerat = NR016[7]; inputCarbing = NR016[8]; inputType = NR016[9]; inputCode = NR016[10]; break;
          case 2232 : inputName = NR017[1]; inputAir = NR017[2]; inputEnergy = NR017[3]; inputProtein = NR017[4]; inputLemak = NR017[5]; inputKarbo = NR017[6]; inputSerat = NR017[7]; inputCarbing = NR017[8]; inputType = NR017[9]; inputCode = NR017[10]; break;
          case 2233 : inputName = NR018[1]; inputAir = NR018[2]; inputEnergy = NR018[3]; inputProtein = NR018[4]; inputLemak = NR018[5]; inputKarbo = NR018[6]; inputSerat = NR018[7]; inputCarbing = NR018[8]; inputType = NR018[9]; inputCode = NR018[10]; break;
          case 2234 : inputName = NR019[1]; inputAir = NR019[2]; inputEnergy = NR019[3]; inputProtein = NR019[4]; inputLemak = NR019[5]; inputKarbo = NR019[6]; inputSerat = NR019[7]; inputCarbing = NR019[8]; inputType = NR019[9]; inputCode = NR019[10]; break;
          case 2235 : inputName = NR020[1]; inputAir = NR020[2]; inputEnergy = NR020[3]; inputProtein = NR020[4]; inputLemak = NR020[5]; inputKarbo = NR020[6]; inputSerat = NR020[7]; inputCarbing = NR020[8]; inputType = NR020[9]; inputCode = NR020[10]; break;
          case 2241 : inputName = NR021[1]; inputAir = NR021[2]; inputEnergy = NR021[3]; inputProtein = NR021[4]; inputLemak = NR021[5]; inputKarbo = NR021[6]; inputSerat = NR021[7]; inputCarbing = NR021[8]; inputType = NR021[9]; inputCode = NR021[10]; break;
          case 2242 : inputName = NR022[1]; inputAir = NR022[2]; inputEnergy = NR022[3]; inputProtein = NR022[4]; inputLemak = NR022[5]; inputKarbo = NR022[6]; inputSerat = NR022[7]; inputCarbing = NR022[8]; inputType = NR022[9]; inputCode = NR022[10]; break;
          case 2243 : inputName = NR023[1]; inputAir = NR023[2]; inputEnergy = NR023[3]; inputProtein = NR023[4]; inputLemak = NR023[5]; inputKarbo = NR023[6]; inputSerat = NR023[7]; inputCarbing = NR023[8]; inputType = NR023[9]; inputCode = NR023[10]; break;
          case 2244 : inputName = NR024[1]; inputAir = NR024[2]; inputEnergy = NR024[3]; inputProtein = NR024[4]; inputLemak = NR024[5]; inputKarbo = NR024[6]; inputSerat = NR024[7]; inputCarbing = NR024[8]; inputType = NR024[9]; inputCode = NR024[10]; break;
          case 2245 : inputName = NP001[1]; inputAir = NP001[2]; inputEnergy = NP001[3]; inputProtein = NP001[4]; inputLemak = NP001[5]; inputKarbo = NP001[6]; inputSerat = NP001[7]; inputCarbing = NP001[8]; inputType = NP001[9]; inputCode = NP001[10]; break;
          case 2251 : inputName = NP002[1]; inputAir = NP002[2]; inputEnergy = NP002[3]; inputProtein = NP002[4]; inputLemak = NP002[5]; inputKarbo = NP002[6]; inputSerat = NP002[7]; inputCarbing = NP002[8]; inputType = NP002[9]; inputCode = NP002[10]; break;
          case 2252 : inputName = NP003[1]; inputAir = NP003[2]; inputEnergy = NP003[3]; inputProtein = NP003[4]; inputLemak = NP003[5]; inputKarbo = NP003[6]; inputSerat = NP003[7]; inputCarbing = NP003[8]; inputType = NP003[9]; inputCode = NP003[10]; break;
          case 2253 : inputName = NP004[1]; inputAir = NP004[2]; inputEnergy = NP004[3]; inputProtein = NP004[4]; inputLemak = NP004[5]; inputKarbo = NP004[6]; inputSerat = NP004[7]; inputCarbing = NP004[8]; inputType = NP004[9]; inputCode = NP004[10]; break;
          case 2254 : inputName = NP005[1]; inputAir = NP005[2]; inputEnergy = NP005[3]; inputProtein = NP005[4]; inputLemak = NP005[5]; inputKarbo = NP005[6]; inputSerat = NP005[7]; inputCarbing = NP005[8]; inputType = NP005[9]; inputCode = NP005[10]; break;
          case 2255 : inputName = NP006[1]; inputAir = NP006[2]; inputEnergy = NP006[3]; inputProtein = NP006[4]; inputLemak = NP006[5]; inputKarbo = NP006[6]; inputSerat = NP006[7]; inputCarbing = NP006[8]; inputType = NP006[9]; inputCode = NP006[10]; break;
          case 22111 : inputName = NP007[1]; inputAir = NP007[2]; inputEnergy = NP007[3]; inputProtein = NP007[4]; inputLemak = NP007[5]; inputKarbo = NP007[6]; inputSerat = NP007[7]; inputCarbing = NP007[8]; inputType = NP007[9]; inputCode = NP007[10]; break;
          case 22112 : inputName = NP008[1]; inputAir = NP008[2]; inputEnergy = NP008[3]; inputProtein = NP008[4]; inputLemak = NP008[5]; inputKarbo = NP008[6]; inputSerat = NP008[7]; inputCarbing = NP008[8]; inputType = NP008[9]; inputCode = NP008[10]; break;
          case 22113 : inputName = NP009[1]; inputAir = NP009[2]; inputEnergy = NP009[3]; inputProtein = NP009[4]; inputLemak = NP009[5]; inputKarbo = NP009[6]; inputSerat = NP009[7]; inputCarbing = NP009[8]; inputType = NP009[9]; inputCode = NP009[10]; break;
          case 22114 : inputName = NP010[1]; inputAir = NP010[2]; inputEnergy = NP010[3]; inputProtein = NP010[4]; inputLemak = NP010[5]; inputKarbo = NP010[6]; inputSerat = NP010[7]; inputCarbing = NP010[8]; inputType = NP010[9]; inputCode = NP010[10]; break;
          case 22115 : inputName = NP011[1]; inputAir = NP011[2]; inputEnergy = NP011[3]; inputProtein = NP011[4]; inputLemak = NP011[5]; inputKarbo = NP011[6]; inputSerat = NP011[7]; inputCarbing = NP011[8]; inputType = NP011[9]; inputCode = NP011[10]; break;
          case 22121 : inputName = NP012[1]; inputAir = NP012[2]; inputEnergy = NP012[3]; inputProtein = NP012[4]; inputLemak = NP012[5]; inputKarbo = NP012[6]; inputSerat = NP012[7]; inputCarbing = NP012[8]; inputType = NP012[9]; inputCode = NP012[10]; break;
          case 22122 : inputName = NP013[1]; inputAir = NP013[2]; inputEnergy = NP013[3]; inputProtein = NP013[4]; inputLemak = NP013[5]; inputKarbo = NP013[6]; inputSerat = NP013[7]; inputCarbing = NP013[8]; inputType = NP013[9]; inputCode = NP013[10]; break;
          case 911 : inputName = QR001[1]; inputAir = QR001[2]; inputEnergy = QR001[3]; inputProtein = QR001[4]; inputLemak = QR001[5]; inputKarbo = QR001[6]; inputSerat = QR001[7]; inputCarbing = QR001[8]; inputType = QR001[9]; inputCode = QR001[10]; break;

          default :
            lcd.show(NOT_FOUND_SCREEN);
            delay(500);
            break;
        }
        
        // float protein = inputProtein.toFloat() * 100.0;
        // Serial.println(inputName);
        // Serial.println(inputName.length());
        // Serial.println(protein);

        delay(50);
        if(lcd.getScreen() != NOT_FOUND_SCREEN){
          if(inputType != ""){ 
            lcd.show(NUTRITION_SCALER_SCREEN);

            if(inputType == "MINUMAN") inputFood = DRINK;
            else if(inputType == "BUMBU") inputFood = SPICE;
            else if(inputType == "GULA, SIRUP DAN KONFEKSIONERI") inputFood = SUGAR;
            else if(inputType == "LEMAK DAN MINYAK") inputFood = OIL;
            else if(inputType == "SUSU DAN HASIL OLAHANNYA") inputFood = MILK;
            else if(inputType == "TELUR DAN HASIL OLAHANNYA") inputFood = EGG;
            else if(inputType == "IKAN, KERANG, UDANG DAN HASIL OLAHANNYA") inputFood = FISH;
            else if(inputType == "DAGING, UNGGAS DAN HASIL OLAHANNYA") inputFood = MEAT;
            else if(inputType == "BUAH DAN HASIL OLAHANNYA") inputFood = FRUITS;
            else if(inputType == "SAYURAN DAN HASIL OLAHANNYA") inputFood = VEGETABLE;
            else if(inputType == "KACANG, BIJI, BEAN DAN HASIL OLAHANNYA") inputFood = NUTS;
            else if(inputType == "UMBI BERPATI DAN HASIL OLAHANNYA") inputFood = ROOTS;
            else if(inputType == "SEREALIA DAN HASIL OLAHANNYA") inputFood = CEREALIA;
            else inputFood = UNKNOWN;

            if(inputFood != UNKNOWN){
              lcd.showFood(inputFood);
              float new_weight = rawWeight;

              rawAir = inputAir.toFloat() * (new_weight / 100.0);
              rawEnergy = inputEnergy.toFloat() * (new_weight / 100.0);
              rawProtein = inputProtein.toFloat() * (new_weight / 100.0);
              rawLemak = inputLemak.toFloat() * (new_weight / 100.0);
              rawKarbo = inputKarbo.toFloat() * (new_weight / 100.0);
              rawSerat = inputSerat.toFloat() * (new_weight / 100.0);
              rawCarbing = inputCarbing.toFloat() * (new_weight / 100.0);

              debug("Total Weight", new_weight);
              debug("Air", rawAir);
              debug("Energy", rawEnergy);
              debug("Protein", rawProtein);
              debug("Lemak", rawLemak);
              debug("Karbo", rawKarbo);
              debug("Serat", rawSerat);
              debug("Carbing", rawCarbing);
              Serial.println("");
              lcd.updateValue(new_weight);
              lcd.showValue(String(rawWeight, 1)+" gr", String(rawEnergy, 1)+" kkal", String(rawLemak, 1)+" gr", String(rawKarbo, 1)+" gr", String(rawCarbing, 1)+" gr", String(rawProtein, 1)+" gr");
            }
          }
        } else{
          code = "";
          lcd.show(MENU_1_SCREEN);
        }

        
      }

      if(lcd.getDelete()){
        code = code.substring(0, code.length()-1);
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        lcd.updateCode(String(code));
      }

      if(lcd.getBack()){
        lastWeight = 0;
        lcd.show(MENU_1_SCREEN);
      }

    } else if(lcd.getScreen() == SETTING_INFO_SCREEN){
      if(lcd.getBack()){
        lastWeight = 0;
        if(deviceConnected) lcd.show(SETTING_BTON_SCREEN);
        else lcd.show(SETTING_BTOFF_SCREEN);
        delay(50);
      }
    
    } else if(lcd.getScreen() == SETTING_BTON_SCREEN || lcd.getScreen() == SETTING_BTOFF_SCREEN){
      if(lcd.getSetMode() == 2){
        lcd.show(SETTING_INFO_SCREEN);
      } else if(lcd.getSetMode() == 1){
        lcd.show(CALIBRATION_SCREEN);
      }
      
      if(lcd.getBack()){
        lastWeight = 0;
        lcd.show(DASHBOARD_SCREEN);
        delay(50);
      }
    } else if(lcd.getScreen() == NUTRITION_SCALER_SCREEN){
      if(lcd.getBack()){
        code = "";
        
        lcd.show(MENU_1_SCREEN);
      } else if(lcd.getTotal()){
        totalWeight += rawWeight;
        totalEnergy += rawEnergy;
        totalLemak += rawLemak;
        totalKarbo += rawKarbo;
        totalCarbing += rawCarbing;
        totalProtein += rawProtein;

        lcd.show(TOTAL_NUTRITION_SCREEN);
        delay(50);
        lcd.showTotal(String(totalWeight, 1)+" gr", String(totalEnergy,2)+" kkal", String(totalLemak, 1)+" gr", String(totalKarbo, 1)+" gr", String(totalCarbing, 1)+" gr", String(totalProtein, 1)+" gr");
      } else if(lcd.getReset()){
        code = "";

        lastWeight = 0;

        totalWeight = 0;
        totalAir = 0;
        totalCarbing = 0;
        totalEnergy = 0;
        totalKarbo = 0;
        totalLemak = 0;
        totalProtein = 0;
        totalSerat = 0;
        
        lcd.show(NUTRITION_WEIGHT_SCREEN);
        lcd.showFood(UNKNOWN);
      } else if(lcd.getNext()){

        lastWeight = rawWeight;
        if(lastWeight != 0){
          totalWeight += rawWeight;
          totalEnergy += rawEnergy;
          totalLemak += rawLemak;
          totalKarbo += rawKarbo;
          totalCarbing += rawCarbing;
          totalProtein += rawProtein;
        } 
        // else{
        //   totalWeight = rawWeight;
        //   totalEnergy = rawEnergy;
        //   totalLemak = rawLemak;
        //   totalKarbo = rawKarbo;
        //   totalCarbing = rawCarbing;
        //   totalProtein = rawProtein;
        // }
        
        debug("Total Weight", totalWeight);
        debug("Total Energy", totalEnergy);
        debug("Total Lemak", totalLemak);
        debug("Total Karbo", totalKarbo);
        debug("Total Carbing", totalCarbing);
        debug("Total Protein", totalProtein);

        code = "";
        
        lcd.show(MENU_1_SCREEN);
        delay(50);
      }
    } else if(lcd.getScreen() == UNIVERSAL_WEIGHT_SCREEN){
      if(lcd.getBack()){
        lcd.show(DASHBOARD_SCREEN);
        delay(50);
      }
    } else if(lcd.getScreen() == SETTING_SCREEN){
      if(lcd.getBack()){
        lcd.show(DASHBOARD_SCREEN);
        delay(50);
      }

      //button wifi

      //button calibrate
    } else if(lcd.getScreen() == CALIBRATION_SCREEN){
      if(lcd.getPlus()){
        _calibration+=0.01;
        Serial.println(_calibration);
        lcd.updateCal(String(_calibration));
      }

      if(lcd.getMinus()){
        _calibration-=0.01;
        Serial.println(_calibration);
        lcd.updateCal(String(_calibration));
      }

      if(lcd.getBack()){
        lastWeight = 0;
        if(deviceConnected) lcd.show(SETTING_BTON_SCREEN);
        else lcd.show(SETTING_BTOFF_SCREEN);
        delay(50);
      }

      if(lcd.getHome()){
        data.save("cal", String(_calibration, 1));
        lcd.show(SAVED_SCREEN);
        lcd.show(DASHBOARD_SCREEN);
      }
    } else if(lcd.getScreen() == TOTAL_NUTRITION_SCREEN){
      if(lcd.getBack()){
        lcd.show(NUTRITION_SCALER_SCREEN);
        delay(50);
        
        if(inputFood != UNKNOWN) lcd.showFood(inputFood);
        else {
          code = "";
          lcd.show(NUTRITION_WEIGHT_SCREEN);
          delay(50);
          lcd.showFood(UNKNOWN);
        }

        float new_weight = rawWeight;

        rawAir = inputAir.toFloat() * (new_weight / 100.0);
        rawEnergy = inputEnergy.toFloat() * (new_weight / 100.0);
        rawProtein = inputProtein.toFloat() * (new_weight / 100.0);
        rawLemak = inputLemak.toFloat() * (new_weight / 100.0);
        rawKarbo = inputKarbo.toFloat() * (new_weight / 100.0);
        rawSerat = inputSerat.toFloat() * (new_weight / 100.0);
        rawCarbing = inputCarbing.toFloat() * (new_weight / 100.0);

        totalWeight -= rawWeight;
        totalEnergy -= rawEnergy;
        totalLemak -= rawLemak;
        totalKarbo -= rawKarbo;
        totalCarbing -= rawCarbing;
        totalProtein -= rawProtein;

        debug("Total Weight", new_weight);
        debug("Air", rawAir);
        debug("Energy", rawEnergy);
        debug("Protein", rawProtein);
        debug("Lemak", rawLemak);
        debug("Karbo", rawKarbo);
        debug("Serat", rawSerat);
        debug("Carbing", rawCarbing);
        Serial.println("");
        lcd.updateValue(new_weight);
        lcd.showValue(String(rawWeight, 1)+" gr", String(rawEnergy, 1)+" kkal", String(rawLemak, 1)+" gr", String(rawKarbo, 1)+" gr", String(rawCarbing, 1)+" gr", String(rawProtein, 1)+" gr");

      } 
      
      if(lcd.getHome()){
        lastWeight = 0;
        lcd.show(DASHBOARD_SCREEN);
        delay(50);
      }
    }

    delay(250);
    ledcWriteTone(0, 0);
  }

  if(millis() - lastRead > sensor_speed){
      if(lcd.getScreen() == NUTRITION_WEIGHT_SCREEN){
        if(lastWeight == 0) {
          rawWeight = updateWeight() - dumpPlate + _calibration;
        } else{
          rawWeight = updateWeight() - lastWeight;
        }

        float new_weight = rawWeight;

        rawAir = inputAir.toFloat() * (new_weight / 100.0);
        rawEnergy = inputEnergy.toFloat() * (new_weight / 100.0);
        rawProtein = inputProtein.toFloat() * (new_weight / 100.0);
        rawLemak = inputLemak.toFloat() * (new_weight / 100.0);
        rawKarbo = inputKarbo.toFloat() * (new_weight / 100.0);
        rawSerat = inputSerat.toFloat() * (new_weight / 100.0);
        rawCarbing = inputCarbing.toFloat() * (new_weight / 100.0);

        debug("Total Weight", new_weight);
        debug("Air", rawAir);
        debug("Energy", rawEnergy);
        debug("Protein", rawProtein);
        debug("Lemak", rawLemak);
        debug("Karbo", rawKarbo);
        debug("Serat", rawSerat);
        debug("Carbing", rawCarbing);
        Serial.println("");
        lcd.updateValue(new_weight);
        // lcd.showValue(String(rawWeight, 1)+" gr", String(rawEnergy, 1)+" kkal", String(rawLemak, 1)+" gr", String(rawKarbo, 1)+" gr", String(rawCarbing, 1)+" gr", String(rawProtein, 1)+" gr");
      }

    if(lcd.getScreen() == UNIVERSAL_WEIGHT_SCREEN){
      rawWeight = updateWeight() + _calibration;

      // debug("Raw Weight + Calibration", rawWeight);
      // debug("Calibration", _calibration);
      // Serial.println("");

      lcd.updateValue(rawWeight);
    }

    if(lcd.getScreen() == CALIBRATION_SCREEN){
      rawWeight = updateWeight() + _calibration;
      
      // debug("Raw Weight + Calibration", rawWeight);
      // debug("Calibration", _calibration);
      // Serial.println("");
      
      lcd.updateValue(rawWeight);
    }

    if(lcd.getScreen() == DASHBOARD_SCREEN){
      bat = map(analogReadMilliVolts(7), 0, 3300, 0, 100);
      if(bat > 70) lcd.updateBattery(BATTERY_FULL);
      else if(bat > 30 && bat <= 70) lcd.updateBattery(BATTERY_HALF);
      else lcd.updateBattery(BATTERY_LOW);

      rawWeight = updateWeight() + _calibration;
      dumpPlate = rawWeight;
      
      // debug("Raw Weight + Calibration", rawWeight);
      // debug("Calibration", _calibration);
      // debug("Plate", dumpPlate);
    }

    lastRead = millis();
  }
}
