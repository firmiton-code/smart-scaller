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
  
  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
  tft.pushImage(0, 0, 320, 240, Boot);
}

void LCDClass::showValue(String value1, String value2, String value3, String value4, String value5){
  tft.fillScreen(BG_COLOR);
  tft.pushImage(0, 0, 320, 240, Dashboard);

  tft.setTextDatum(TR_DATUM);
  tft.drawString(value1, 108, 45, 6);
  
  tft.setTextDatum(TR_DATUM);
  tft.drawString(value2, 270, 45, 6);
  
  tft.setTextDatum(TR_DATUM);
  tft.drawString(value3, 123, 165, 6);
  
  tft.setFreeFont(&FreeSans18pt7b);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(value4, 270, 160, 1);
  tft.drawString(value5, 270, 190, 1);
  
  tft.setFreeFont(&FreeSans12pt7b);
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
  if(screen == DASHBOARD_SCREEN){
    tft.pushImage(0, 0, 320, 240, Dashboard);
  } else if(screen == UNIVERSAL_WEIGHT_SCREEN){
    tft.pushImage(0, 0, 320, 240, Weight_Scalling);
  } else if(screen == NUTRITION_WEIGHT_SCREEN){
    tft.pushImage(0, 0, 320, 240, Code_Input);
  } else if(screen == TOTAL_NUTRITION_SCREEN){
    tft.pushImage(0, 0, 320, 240, Output);
  }

  _screen = screen;
}

void LCDClass::showBattery(Battery_Level_t batt_level){
  switch (batt_level){
    case BATTERY_LOW :
      tft.pushImage(280, 104, 26, 26, Output, BG_COLOR);
      // tft.fillRect(305, 19, 15, 2, BG_COLOR);
      break;
    case BATTERY_HALF :
      tft.pushImage(280, 104, 26, 26, Loading, BG_COLOR);
      // tft.fillRect(305, 19, 15, 2, BG_COLOR);
      break;
    case BATTERY_FULL :
      tft.pushImage(280, 104, 26, 26, Weight_Scalling, BG_COLOR);
      // tft.fillRect(305, 19, 15, 2, BG_COLOR);
      break;
    default :
      tft.pushImage(280, 104, 26, 26, Error, BG_COLOR);
      // tft.fillRect(305, 19, 15, 2, BG_COLOR);
      break;
  }
}

bool LCDClass::touchUpdate(){
  if(digitalRead(21)) return false;

  uint16_t t_x, t_y;
  tft.getTouchRaw(&t_x, &t_y);
  tft.convertRawXY(&t_x, &t_y);

  if(t_x >= 186 && t_x <= 226 && t_y >= 50 && t_y <=91){
    _key = 7;
    Serial.println("7");
  } else if(t_x >= 230 && t_x <= 270 && t_y >= 50 && t_y <=91){
    _key = 8;
    Serial.println("8");
  } else if(t_x >= 274 && t_x <= 314 && t_y >= 50 && t_y <=91){
    _key = 9;
    Serial.println("9");
  } else if(t_x >= 186 && t_x <= 226 && t_y >= 96 && t_y <=136){
    _key = 4;
    Serial.println("4");
  } else if(t_x >= 230 && t_x <= 270 && t_y >= 96 && t_y <=136){
    _key = 5;
    Serial.println("5");
  } else if(t_x >= 274 && t_x <= 314 && t_y >= 96 && t_y <=136){
    _key = 6;
    Serial.println("6");
  } else if(t_x >= 186 && t_x <= 226 && t_y >= 142 && t_y <=182){
    _key = 1;
    Serial.println("1");
  } else if(t_x >= 230 && t_x <= 270 && t_y >= 142 && t_y <=182){
    _key = 2;
    Serial.println("2");
  } else if(t_x >= 274 && t_x <= 314 && t_y >= 142 && t_y <=182){
    _key = 3;
    Serial.println("3");
  } else if(t_x >= 186 && t_x <= 226 && t_y >= 188 && t_y <=228){
    _key = 0;
    Serial.println("0");
  } else{
    _key = 99;
  }
  
  if(t_x >= 230 && t_x <= 314 && t_y >= 188 && t_y <=228){
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

  if(t_x >= 169 && t_x <= 194 && t_y >= 17 && t_y <=42){
    _total = true;
    Serial.println("Total");
  } else{
    _total = false;
  }

  return true;
}

void LCDClass::updateValue(float weight_value){
  String text_value = String(weight_value, 2);
  tft.setFreeFont(&FreeSans18pt7b);
  tft.setTextDatum(CC_DATUM);
  tft.drawString(text_value, 93, 77, 6);  //koordinat berat yg 2 variabel tengah
}

void LCDClass::updateCode(String code){
  tft.setFreeFont(&FreeSans18pt7b);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(code, 192, 16, 1);
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

int LCDClass::getKey(){
  return _key;
}

LCDClass lcd;
