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
  DASHBOARD_SCREEN,
  UNIVERSAL_WEIGHT_SCREEN,
  NUTRITION_WEIGHT_SCREEN,
  TOTAL_NUTRITION_SCREEN
} Screen_position_t;

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
    Screen_position_t _screen;
    
    int _key;
    bool _enter = false;
    bool _back = false;
    bool _next = false;
    bool _delete = false;
    bool _total = false;
    int _mode = 0;

  public:
    LCDClass();
    void init();
    void showValue(String value1, String value2, String value3, String value4, String value5);
    void loading(int percentage);
    void reset();
    void showBattery(Battery_Level_t batt_level);
    void show(Screen_position_t screen);
    void updateValue(float weight_value);
    void updateCode(String code);

    // Touch Function
    void calibration();
    bool touchUpdate();

    bool getEnter();
    bool getBack();
    bool getNext();
    bool getDelete();
    bool getTotal();
    int getKey();
};

extern LCDClass lcd;

#endif // LCD_H_