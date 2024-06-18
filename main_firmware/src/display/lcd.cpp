#include "lcd.h"

LCDClass *lcd_instances;

LCDClass::LCDClass(){
  lcd_instances = this;
}

void LCDClass::calibration(){
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists 
  if (!SPIFFS.begin()) {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

void LCDClass::init(){
  tft.init();
  tft.setRotation(3);
  pinMode(TOUCH_PIN, INPUT);

  calibration();

  tft.fillScreen(BG_COLOR);
  tft.setSwapBytes(true);
  tft.setFreeFont(&FreeSans12pt7b);
  tft.setTextColor(TFT_BLACK, BG_COLOR);
  
  textWeightNutri.createSprite(150, 30);
  textCode.createSprite(100, 30);
  textWeightUniv.createSprite(280, 70);

  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
  tft.pushImage(0, 0, 320, 240, Boot);
}

void LCDClass::showValue(String value1, String value2, String value3, String value4, String value5, String value6){
  tft.setFreeFont(&FreeSans9pt7b);
  tft.setTextDatum(BR_DATUM);

  tft.drawString(value1, 300, 60, 1);
  tft.drawString(value2, 300, 95, 1);
  tft.drawString(value3, 300, 117, 1);
  tft.drawString(value4, 300, 140, 1);
  tft.drawString(value5, 300, 162, 1);
  tft.drawString(value6, 300, 184, 1);
}

void LCDClass::showTotal(String value1, String value2, String value3, String value4, String value5, String value6){
  tft.setFreeFont(&FreeSans9pt7b);
  tft.setTextDatum(BR_DATUM);

  tft.drawString(value1, 265, 85, 1);
  tft.drawString(value2, 265, 115, 1);
  tft.drawString(value3, 265, 140, 1);
  tft.drawString(value4, 265, 165, 1);
  tft.drawString(value5, 265, 190, 1);
  tft.drawString(value6, 265, 215, 1);
}

void LCDClass::loading(int percentage){
  tft.fillSmoothRoundRect(85, 190, 150, 15, 15, TFT_DARKGREY, BG_COLOR);
  tft.fillSmoothRoundRect(85, 190, percentage*15, 15, 15, TFT_BLUE, BG_COLOR);
  // tft.drawSmoothArc(160, 120, 10, 12, 0, percentage*36, TFT_GREEN, BG_COLOR);
  // tft.fillSmoothCircle(160, 120, 50, TFT_GREEN, BG_COLOR);
}

void LCDClass::reset(){
  tft.fillScreen(BG_COLOR);
}

void LCDClass::show(Screen_position_t screen){
  switch (screen) {
    case DASHBOARD_SCREEN :
      tft.pushImage(0, 0, 320, 240, Dashboard);
      break;
    case UNIVERSAL_WEIGHT_SCREEN :
      tft.pushImage(0, 0, 320, 240, Weight_Scalling);
      break;
    case NUTRITION_WEIGHT_SCREEN :
      tft.pushImage(0, 0, 320, 240, Code_Input_Rev);
      break;
    case NUTRITION_SCALER_SCREEN :
      tft.pushImage(0, 0, 320, 240, Code_Success);
      break;
    case TOTAL_NUTRITION_SCREEN :
      tft.pushImage(0, 0, 320, 240, Output);
      break;
    case SETTING_SCREEN :
      tft.pushImage(0, 0, 320, 240, Setting);
      break;
    case CALIBRATION_SCREEN :
      tft.pushImage(0, 0, 320, 240, Calibration);
      break;
    case SAVED_SCREEN :
      tft.pushImage(0, 0, 320, 240, Save_Calibration);
      break;
    case COMING_SOON_SCREEN :
      tft.pushImage(0, 0, 320, 240, soon);
      break;
    case MENU_1_SCREEN :
      tft.pushImage(0, 0, 320, 240, Choose_Menu_1);
      break;
    case MENU_2_SCREEN :
      tft.pushImage(0, 0, 320, 240, Choose_Menu_2);
      break;
    case MENU_3_SCREEN :
      tft.pushImage(0, 0, 320, 240, Choose_Menu_3);
      break;
    case MENU_4_SCREEN :
      tft.pushImage(0, 0, 320, 240, Choose_Menu_4);
      break;
    case SETTING_BTOFF_SCREEN :
      tft.pushImage(0, 0, 320, 240, Setting_BTOFF);
      break;
    case SETTING_BTON_SCREEN :
      tft.pushImage(0, 0, 320, 240, Setting_BTON);
      break;
    case SETTING_INFO_SCREEN :
      tft.pushImage(0, 0, 320, 240, Setting_Info);
      break;
    case NOT_FOUND_SCREEN :
      tft.pushImage(0, 0, 320, 240, NotFound);
      break;

   default :
      tft.pushImage(0, 0, 320, 240, Boot);
      break;
  }
  Serial.println(screen);

  Serial.println("Screen Changed");
  _screen = screen;
}

void LCDClass::updateBattery(Battery_Level_t batt_level){
  switch (batt_level){
    case BATTERY_LOW :
      tft.pushImage(183, 173, 35, 35, Battery_Low, BG_COLOR);
      tft.fillRect(200, 205, 18, 5, BG_COLOR);
      break;
    case BATTERY_HALF :
      tft.pushImage(183, 173, 35, 35, Battery_Half, BG_COLOR);
      tft.fillRect(200, 205, 18, 5, BG_COLOR);
      break;
    case BATTERY_FULL :
      tft.pushImage(183, 173, 35, 35, Battery_Full, BG_COLOR);
      tft.fillRect(200, 205, 18, 5, BG_COLOR);
      break;
    default :
      tft.pushImage(183, 173, 35, 35, Battery_Low, BG_COLOR);
      tft.fillRect(200, 205, 18, 5, BG_COLOR);
      break;
  }
}

bool LCDClass::touchUpdate(){
  if(digitalRead(TOUCH_PIN)) {
    return false;
  }
  uint16_t t_x, t_y;
  tft.getTouchRaw(&t_x, &t_y);
  tft.convertRawXY(&t_x, &t_y);

  if(_screen == UNIVERSAL_WEIGHT_SCREEN){
    if(t_x >= 17 && t_x <= 52 && t_y >= 17 && t_y <= 52){
      _back = true;
      Serial.println("Back");
    } else{
      _back = false;
    }
  }

  if(_screen == MENU_1_SCREEN || _screen == MENU_2_SCREEN || _screen == MENU_3_SCREEN || _screen == MENU_4_SCREEN){
    if(t_x >= 17 && t_x <= 52 && t_y >= 17 && t_y <= 52){
      _back = true;
      Serial.println("Back");
    } else{
      _back = false;
    }

    if(t_x >= 32 && t_x <= 252 && t_y >= 56 && t_y <= 96){
      _choose =  1;
      Serial.println("1");
    } else if(t_x >= 32 && t_x <= 252 && t_y >= 104 && t_y <= 144){
      _choose = 2;
      Serial.println("2");
    } else if(t_x >= 32 && t_x <= 252 && t_y >= 152 && t_y <= 192){
      _choose = 3;
      Serial.println("3");
    } else if(t_x >= 32 && t_x <= 252 && t_y >= 200 && t_y <= 240){
      _choose = 4;
      Serial.println("4");
    } else{
      _choose = 0;
    }

    if(t_x >= 270 && t_x <= 300 && t_y >= 50 && t_y <= 80){
      _menu = 1;
      Serial.println("Menu Up");
    } else if(t_x >= 270 && t_x <= 300 && t_y >= 180 && t_y <= 230){
      _menu = 2;
      Serial.println("Menu Down");
    } else{
      _menu = 0;
    }
  }

  if(_screen == NUTRITION_WEIGHT_SCREEN){
    if(t_x >= 186 && t_x <= 246 && t_y >= 45 && t_y <= 100){
      _key =  1;
      Serial.println("1");
    } else if(t_x >= 254 && t_x <= 314 && t_y >= 45 && t_y <= 100){
      _key = 2;
      Serial.println("2");
    } else if(t_x >= 186 && t_x <= 246 && t_y >= 110 && t_y <= 165){
      _key = 3;
      Serial.println("3");
    } else if(t_x >= 254 && t_x <= 314 && t_y >= 110 && t_y <= 165){
      _key = 4;
      Serial.println("4");
    } else if(t_x >= 186 && t_x <= 264 && t_y >= 175 && t_y <= 230){
      _key = 5;
      Serial.println("5");
    } else {
      _key = 99;
    }

    if(t_x >= 220 && t_x <= 314 && t_y >= 175 && t_y <= 230){
      _enter = true;
      Serial.println("Enter");
    } else{
      _enter = false;
    }
    
    if(t_x >= 17 && t_x <= 52 && t_y >= 17 && t_y <= 52){
      _back = true;
      Serial.println("Back");
    } else{
      _back = false;
    }
    
    if(t_x >= 285 && t_x <= 310 && t_y >= 15 && t_y <= 35){
      _delete = true;
      Serial.println("Delete");
    } else{
      _delete = false;
    }
  }

  if(_screen == NUTRITION_SCALER_SCREEN){
    if(t_x >= 241 && t_x <= 307 && t_y >= 194 && t_y <= 224){
      _total = true;
      Serial.println("Total");
    } else if(t_x >= 160 && t_x <= 226 && t_y >= 194 && t_y <= 224){
      _reset = true;
      Serial.println("Reset");
    } else{
      _reset = false;
    }

    if(t_x >= 64 && t_x <= 99 && t_y >= 17 && t_y <= 52){
      _next = true;
      Serial.println("Next");
    } else{
      _next = false;
    }

    if(t_x >= 17 && t_x <= 52 && t_y >= 17 && t_y <= 52){
      _back = true;
      Serial.println("Back");
    } else{
      _back = false;
    }
  }

  if(_screen == CALIBRATION_SCREEN || _screen == TOTAL_NUTRITION_SCREEN){
    if(t_x >= 283 && t_x <= 338 && t_y >= 7 && t_y <= 52){
      _home = true;
      Serial.println("Home");
    } else{
      _home = false;
    }

    if(t_x >= 17 && t_x <= 52 && t_y >= 17 && t_y <= 52){
      _back = true;
      Serial.println("Back");
    } else{
      _back = false;
    }
  }
  
  if(_screen == CALIBRATION_SCREEN){
    if(t_x >= 49 && t_x <= 104 && t_y >= 163 && t_y <= 212){
      _minus = true;
      Serial.println("Minus");
    } else{
      _minus = false;
    }

    if(t_x >= 215 && t_x <= 270 && t_y >= 163 && t_y <= 212){
      _plus = true;
      Serial.println("Plus");
    } else{
      _plus = false;
    }
    
    if(t_x >= 17 && t_x <= 52 && t_y >= 17 && t_y <= 52){
      _back = true;
      Serial.println("Back");
    } else{
      _back = false;
    }
  }
  
  if(_screen == SETTING_INFO_SCREEN){
    if(t_x >= 17 && t_x <= 52 && t_y >= 17 && t_y <= 52){
      _back = true;
      Serial.println("Back");
    } else{
      _back = false;
    }
  }

  if(_screen == SETTING_BTON_SCREEN || _screen == SETTING_BTOFF_SCREEN){
    if(t_x >= 17 && t_x <= 52 && t_y >= 17 && t_y <= 52){
      _back = true;
      Serial.println("Back");
    } else{
      _back = false;
    }

    if(t_x >= 145 && t_x <= 190 && t_y >= 43 && t_y <= 220){
      _set_mode = 2;
    } else if(t_x >= 95 && t_x <= 140 && t_y >= 43 && t_y <= 220){
      _set_mode = 1;
    } else{
      _set_mode = 0;
    }
  }

  if(_screen == DASHBOARD_SCREEN){
    if(t_x >= 20 && t_x <= 150 && t_y >= 20 && t_y <=220){
      _mode = 1;
      Serial.println("Mode");
    } else if(t_x >= 170 && t_x <= 300 && t_y >= 20 && t_y <=149){
      _mode = 2;
      Serial.println("Mode");
    } else if(t_x >= 240 && t_x <= 300 && t_y >= 160 && t_y <=220){
      _mode = 3;
      Serial.println("Mode");
    } else{
      _mode = 0;
    }
  }

  Serial.println("Screen Tapped");
  return true;
}

void LCDClass::updateValue(float weight_value){
  String text_value =  weight_value < 800.00 ? String(weight_value, 1) + " gr" : String((weight_value/1000.00), 1) + " kg";
  if(_screen == NUTRITION_WEIGHT_SCREEN/* || _screen == NUTRITION_SCALER_SCREEN*/){
    textWeightNutri.fillSprite(BG_COLOR);
    textWeightNutri.setTextDatum(CC_DATUM);
    textWeightNutri.setFreeFont(&FreeSans12pt7b);
    textWeightNutri.setTextColor(TFT_BLACK, TFT_WHITE);
    textWeightNutri.drawString(text_value, 76, 10, 1);  //koordinat berat yg 2 variabel tengah
    textWeightNutri.pushSprite(20, 179);
  } else if(_screen == UNIVERSAL_WEIGHT_SCREEN || _screen == CALIBRATION_SCREEN){ 
    textWeightUniv.fillSprite(BG_COLOR); 
    textWeightUniv.setTextDatum(CC_DATUM);
    textWeightUniv.setFreeFont(&FreeSans24pt7b);
    textWeightUniv.setTextColor(TFT_BLACK, TFT_WHITE);
    textWeightUniv.drawString(text_value, 140, 35, 1);
    textWeightUniv.pushSprite(30, 55);
  }
}

void LCDClass::updateCode(String code){
  tft.setFreeFont(&FreeSans9pt7b);
  // tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(code, 192, 18, 1);
}

void LCDClass::updateCal(String cal){
  tft.setFreeFont(&FreeSans9pt7b);
  // tft.setTextColor(TFT_BLACK, TFT_WHITE);
  String prnt = " "+cal+" ";
  tft.setTextDatum(CC_DATUM);
  tft.drawString(prnt, 159, 187, 1);
}

void LCDClass::showFood(Food_type_t food){
  switch (food){
    case CEREALIA :
      tft.pushImage(53, 70, 80, 80, Cerealia, BG_COLOR);
      break;
    case DRINK :
      tft.pushImage(53, 70, 80, 80, Drink, BG_COLOR);
      break;
    case EGG :
      tft.pushImage(53, 70, 80, 80, Egg, BG_COLOR);
      break;
    case FISH :
      tft.pushImage(53, 70, 80, 80, Fish, BG_COLOR);
      break;
    case FRUITS :
      tft.pushImage(53, 70, 80, 80, Fruits, BG_COLOR);
      break;
    case MEAT :
      tft.pushImage(53, 70, 80, 80, Milk, BG_COLOR);
      break;
    case MILK :
      tft.pushImage(53, 70, 80, 80, Milk, BG_COLOR);
      break;
    case NUTS :
      tft.pushImage(53, 70, 80, 80, Nuts, BG_COLOR);
      break;
    case OIL :
      tft.pushImage(53, 70, 80, 80, Oil, BG_COLOR);
      break;
    case ROOTS :
      tft.pushImage(53, 70, 80, 80, Roots, BG_COLOR);
      break;
    case SPICE :
      tft.pushImage(53, 70, 80, 80, Spice, BG_COLOR);
      break;
    case SUGAR :
      tft.pushImage(53, 70, 80, 80, Sugar, BG_COLOR);
      break;
    case VEGETABLE :
      tft.pushImage(53, 70, 80, 80, Vegetable, BG_COLOR);
      break;
    case UNKNOWN :
      tft.pushImage(53, 70, 80, 80, Unknown, BG_COLOR);
      break;

    default :
      tft.pushImage(53, 70, 80, 80, Unknown, BG_COLOR);
      break;
  }
}

bool LCDClass::getEnter(){
  return _enter;
}

bool LCDClass::getBack(){
  return _back;
}

bool LCDClass::getNext(){
  return _next;
}

bool LCDClass::getDelete(){
  return _delete;
}

bool LCDClass::getTotal(){
  return _total;
}

bool LCDClass::getHome(){
  return _home;
}

bool LCDClass::getReset(){
  return _reset;
}

bool LCDClass::getPlus(){
  return _plus;
}

bool LCDClass::getMinus(){
  return _minus;
}

int LCDClass::getKey(){
  return _key;
}

int LCDClass::getMode(){
  return _mode;
}

int LCDClass::getChoose(){
  return _choose;
}

int LCDClass::getMenu(){
  return _menu;
}

int LCDClass::getSetMode(){
  return _set_mode;
}

Screen_position_t LCDClass::getScreen(){
  return _screen;
}

LCDClass lcd;
