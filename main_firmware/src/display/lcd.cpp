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
  pinMode(21, INPUT);

  calibration();

  tft.fillScreen(BG_COLOR);
  tft.setSwapBytes(true);
  tft.setFreeFont(&FreeSans12pt7b);
  tft.setTextColor(TFT_BLACK, BG_COLOR);
  
  textWeightNutri.createSprite(170, 35);
  textCode.createSprite(100, 30);
  textWeightUniv.createSprite(280, 70);

  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
  tft.pushImage(0, 0, 320, 240, Boot);
}

void LCDClass::showValue(String value1, String value2, String value3, String value4, String value5, String value6){
  tft.setFreeFont(&FreeSans9pt7b);
  tft.setTextDatum(BR_DATUM);

  tft.drawString(value1, 256, 73, 1);
  tft.drawString(value2, 256, 104, 1);
  tft.drawString(value3, 256, 129, 1);
  tft.drawString(value4, 256, 154, 1);
  tft.drawString(value5, 256, 179, 1);
  tft.drawString(value6, 256, 204, 1);
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
      tft.pushImage(0, 0, 320, 240, Code_Input);
      break;
    case TOTAL_NUTRITION_SCREEN :
      tft.pushImage(0, 0, 320, 240, Output);
      break;
    case SETTING_SCREEN :
      tft.pushImage(0, 0, 320, 240, Setting);
      break;
    
    default :
      tft.pushImage(0, 0, 320, 240, Boot);
      break;
  }
  
  _screen = screen;
}

void LCDClass::updateBattery(Battery_Level_t batt_level){
  switch (batt_level){
    case BATTERY_LOW :
      tft.pushImage(183, 173, 35, 35, Battery_Low, BG_COLOR);
      break;
    case BATTERY_HALF :
      tft.pushImage(183, 173, 35, 35, Battery_Half, BG_COLOR);
      break;
    case BATTERY_FULL :
      tft.pushImage(183, 173, 35, 35, Battery_Full, BG_COLOR);
      break;
    default :
      tft.pushImage(183, 173, 35, 35, Battery_Low, BG_COLOR);
      break;
  }
}

bool LCDClass::touchUpdate(){
  if(digitalRead(21)) return false;

  uint16_t t_x, t_y;
  tft.getTouchRaw(&t_x, &t_y);
  tft.convertRawXY(&t_x, &t_y);

  if(t_x >= 191 && t_x <= 221 && t_y >= 55 && t_y <=86){
    _key = 7;
    Serial.println("7");
  } else if(t_x >= 235 && t_x <= 265 && t_y >= 55 && t_y <=86){
    _key = 8;
    Serial.println("8");
  } else if(t_x >= 279 && t_x <= 309 && t_y >= 55 && t_y <=86){
    _key = 9;
    Serial.println("9");
  } else if(t_x >= 191 && t_x <= 221 && t_y >= 101 && t_y <=131){
    _key = 4;
    Serial.println("4");
  } else if(t_x >= 235 && t_x <= 265 && t_y >= 101 && t_y <=131){
    _key = 5;
    Serial.println("5");
  } else if(t_x >= 279 && t_x <= 309 && t_y >= 101 && t_y <=131){
    _key = 6;
    Serial.println("6");
  } else if(t_x >= 191 && t_x <= 221 && t_y >= 147 && t_y <=177){
    _key = 1;
    Serial.println("1");
  } else if(t_x >= 235 && t_x <= 265 && t_y >= 147 && t_y <=177){
    _key = 2;
    Serial.println("2");
  } else if(t_x >= 279 && t_x <= 309 && t_y >= 147 && t_y <=177){
    _key = 3;
    Serial.println("3");
  } else if(t_x >= 191 && t_x <= 221 && t_y >= 193 && t_y <=223){
    _key = 0;
    Serial.println("0");
  } else{
    _key = 99;
  }
  
  if(t_x >= 235 && t_x <= 309 && t_y >= 193 && t_y <=223){
    _enter = true;
    Serial.println("Enter");
  } else{
    _enter = false;
  }

  if(t_x >= 17 && t_x <= 42 && t_y >= 17 && t_y <=42){
    _back = true;
    Serial.println("Back");
  } else{
    _back = false;
  }
  
  if(t_x >= 303 && t_x <= 328 && t_y >= 17 && t_y <=42){
    _next = true;
    Serial.println("Next");
  } else{
    _next = false;
  }

  if(t_x >= 289 && t_x <= 309 && t_y >= 15 && t_y <=36){
    _delete = true;
    Serial.println("Delete");
  } else{
    _delete = false;
  }
  
  if(t_x >= 169 && t_x <= 194 && t_y >= 17 && t_y <=42){
    _total = true;
    Serial.println("Total");
  } else{
    _total = false;
  }

  if(t_x >= 281 && t_x <= 301 && t_y >= 19 && t_y <=39){
    _home = true;
    Serial.println("Home");
  } else{
    _home = false;
  }

  if(t_x >= 20 && t_x <= 150 && t_y >= 20 && t_y <=220){
    _mode = 1;
  } else if(t_x >= 170 && t_x <= 300 && t_y >= 20 && t_y <=149){
    _mode = 2;
  } else if(t_x >= 240 && t_x <= 300 && t_y >= 160 && t_y <=220){
    _mode = 3;
  } else{
    _mode = 0;
  }

  return true;
}

void LCDClass::updateValue(float weight_value){
  String text_value = String(weight_value, 2);
  if(_screen = NUTRITION_WEIGHT_SCREEN){
    textWeightNutri.fillSprite(BG_COLOR);
    textWeightNutri.setTextDatum(CC_DATUM);
    textWeightNutri.setFreeFont(&FreeSans12pt7b);
    textWeightNutri.drawString(text_value, 85, 32, 6);  //koordinat berat yg 2 variabel tengah
    textWeightNutri.pushSprite(8, 60);
  } else if(_screen = UNIVERSAL_WEIGHT_SCREEN){ 
    textWeightUniv.fillSprite(BG_COLOR); 
    textWeightUniv.setTextDatum(CC_DATUM);
    textWeightUniv.setFreeFont(&FreeSans9pt7b);
    textWeightUniv.drawString(text_value, 140, 35, 6);
    textWeightUniv.pushSprite(30, 60);
  }
}

void LCDClass::updateCode(String code){
  tft.setFreeFont(&FreeSans9pt7b);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(code, 192, 16, 1);
}

void LCDClass::showFood(Food_type_t food){
  switch (food){
    case BANANA :
      tft.pushImage(57, 111, 70, 70, Icon_Makanan_Pisang, BG_COLOR);
      break;
    case UNKNOWN :
      tft.pushImage(57, 111, 70, 70, Icon_Makanan_Unknown, BG_COLOR);
      break;

    default :
      tft.pushImage(57, 111, 70, 70, Icon_Makanan_Unknown, BG_COLOR);
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

int LCDClass::getKey(){
  return _key;
}

int LCDClass::getMode(){
  return _mode;
}

Screen_position_t LCDClass::getScreen(){
  return _screen;
}

LCDClass lcd;
