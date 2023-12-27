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
  pinMode(14, INPUT);

  calibration();

  tft.fillScreen(BG_COLOR);
  tft.setSwapBytes(true);
  tft.setFreeFont(&FreeSans12pt7b);
  tft.setTextColor(TFT_BLACK, BG_COLOR);
  
  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
  // max 240*230=55200
  // if (NULL != bgSprite.createSprite(290, 190)) {
  //   Serial.println("Sprite created");
  //   Serial.print("Free Heap: ");
  //   Serial.println(ESP.getFreeHeap());
  //   bgSprite.setSwapBytes(true);
  // } else {
  //   Serial.println("Sprite creation failed");
  // }

  // tft.pushImage(90, 40, 140, 120, telsys_logo);
  tft.pushImage(0, 0, 320, 240, Boot);
}

void LCDClass::boot(){
  delay(2000);
  tft.fillScreen(BG_COLOR);
  // tft.pushImage(90, 60, 140, 120, telsys_logo);
  tft.pushImage(0, 0, 320, 240, Boot);
}

void LCDClass::show(String value1, String value2, String value3, String value4, String value5){
  tft.fillScreen(BG_COLOR);
  tft.pushImage(0, 0, 320, 240, Dashboard);

  // tft.drawSmoothRoundRect(8, 10, 10, 12, 142, 100, TFT_DARKGREY, BG_COLOR);
  // tft.drawSmoothRoundRect(170, 10, 10, 12, 142, 100, TFT_DARKGREY, BG_COLOR);
  // tft.drawSmoothRoundRect(8, 130, 10, 12, 142, 100, TFT_DARKGREY, BG_COLOR);
  // tft.drawSmoothRoundRect(170, 130, 10, 12, 142, 100, TFT_DARKGREY, BG_COLOR);
  
  // bgSprite.fillSprite(BG_COLOR);
  // bgSprite.pushImage(75, 35, 140, 120, dashboard_logo);
  // bgSprite.pushSprite(15, 25, BG_COLOR);

  // tft.setTextDatum(TL_DATUM);
  // tft.drawString("Heart Rate", 50, 15, 2);
  // tft.drawString("Oxygen", 230, 15, 2);
  // tft.drawString("Temperature", 37, 135, 2);
  // tft.drawString("Blood Pressure", 210, 135, 2);

  tft.setTextDatum(TR_DATUM);
  tft.drawString(value1, 108, 45, 6);
  // tft.setTextDatum(TL_DATUM);
  // tft.drawString("bpm", 112, 45, 2);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(value2, 270, 45, 6);
  // tft.setTextDatum(TL_DATUM);
  // tft.drawString("%", 273, 45, 1);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(value3, 123, 165, 6);
  // tft.setTextDatum(BL_DATUM);
  // tft.drawString("*C", 125, 205, 2);
  tft.setFreeFont(&FreeSans18pt7b);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(value4, 270, 160, 1);
  tft.drawString(value5, 270, 190, 1);
  // tft.setTextDatum(TL_DATUM);
  // tft.drawString("mmhg", 273, 180, 2);
  tft.setFreeFont(&FreeSans12pt7b);
}

void LCDClass::notice(String value, String note){
  if(value == "BPM"){
    tft.fillScreen(BG_COLOR);
    tft.pushImage(90, 30, 100, 100, Choose_NutriScale);
    tft.drawCentreString(note, 160, 150, 1);
  } else if(value == "TENSI"){
    tft.fillScreen(BG_COLOR);
    tft.pushImage(90, 30, 100, 100, Choose_Universal);
    tft.drawCentreString(note, 160, 150, 1);
  }
}

void LCDClass::load(int percentage){
  tft.fillSmoothRoundRect(85, 190, 150, 15, 15, TFT_DARKGREY, BG_COLOR);
  tft.fillSmoothRoundRect(85, 190, percentage*15, 15, 15, TFT_BLUE, BG_COLOR);
  // tft.drawSmoothArc(160, 120, 10, 12, 0, percentage*36, TFT_GREEN, BG_COLOR);
  // tft.fillSmoothCircle(160, 120, 50, TFT_GREEN, BG_COLOR);
}

void LCDClass::reset(){
  tft.fillScreen(BG_COLOR);
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
  if(digitalRead(14)) return false;

  uint16_t t_x, t_y;
  tft.getTouchRaw(&t_x, &t_y);
  tft.convertRawXY(&t_x, &t_y);
  
  if(t_x >= 174 && t_x <= 202 && t_y >= 102 && t_y <=130){
    _play = true;
  }

  if(t_x >= 20 && t_x <= 46 && t_y >= 104 && t_y <=130){
    // debug("Touch", "Button Power Press");
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 0);
    // vTaskDelay(500 / portTICK_PERIOD_MS);
    // ESP.restart();
    // esp_deep_sleep_start();
    _power = true;
  }

  if(t_x >= 93 && t_x <= 226 && t_y >= 123 && t_y <=149){
    _connection = true;
  } else if(t_x >= 93 && t_x <= 226 && t_y >= 157 && t_y <=163){
    _connection = false;
  }

  return true;
}

void LCDClass::show_choose(){
  tft.pushImage(0, 0, 320, 240, Dashboard);
}

bool LCDClass::getConnection(){
  return _connection;
}

bool LCDClass::getPlay(){
  return _play;
}

bool LCDClass::getPower(){
  return _power;
}

void LCDClass::setPlay(bool play){
  _play = play;
}

LCDClass lcd;
