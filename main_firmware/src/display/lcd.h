#ifndef LCD_H_
#define LCD_H_

#include <Arduino.h>
#include "lcd_assets.h"
#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h>

// This is the file name used to store the calibration data
// You can change this to create new calibration files.
// The SPIFFS file name must start with "/".
#define CALIBRATION_FILE "/TouchCalData1"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

#define BG_COLOR TFT_WHITE
#define LARGE_TEXT 6
#define NORMAL_TEXT 1
#define SMALL_TEXT 2
#define NUMBER_TEXT 7

typedef enum {
  BATTERY_LOW,
  BATTERY_HALF,
  BATTERY_FULL
} Battery_Level_t;

class LCDClass{
  private:
    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite bgSprite = TFT_eSprite(&tft);
    TFT_eSprite textSprite = TFT_eSprite(&tft);
    TFT_eSprite logoSprite = TFT_eSprite(&tft);
    Battery_Level_t batt;
    bool _connection = false;
    bool _play = false;
    bool _power = false;

  public:
    LCDClass();
    void init();
    void boot();
    void show(String value1, String value2, String value3, String value4, String value5);
    void notice(String value, String note);
    void load(int percentage);
    void reset();
    void showBattery(Battery_Level_t batt_level);
    void show_choose();

    void calibration();

    bool touchUpdate();
    bool getConnection();
    bool getPlay();
    bool getPower();

    void setPlay(bool play);
    void update(int batt);
};

extern LCDClass lcd;

#endif // LCD_H_