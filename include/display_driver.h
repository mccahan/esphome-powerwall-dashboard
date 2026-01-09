/**
 * Display Driver for ST7701S on ESP32-S3
 * Based on Guition ESP32-S3-4848S040 hardware configuration
 */

#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <Arduino.h>
#include <SPI.h>
#include <lvgl.h>

#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 480

// Pin definitions from hardware configuration
#define TFT_CS    39
#define TFT_MOSI  47
#define TFT_SCLK  48
#define TFT_DC    -1  // Not used in RGB mode
#define TFT_RST   -1  // Not used

// RGB interface pins
#define LCD_DE    18
#define LCD_VSYNC 17
#define LCD_HSYNC 16
#define LCD_PCLK  21

// Backlight
#define LCD_BL    38

// RGB data pins
#define LCD_R0    11
#define LCD_R1    12
#define LCD_R2    13
#define LCD_R3    14
#define LCD_R4    0

#define LCD_G0    8
#define LCD_G1    20
#define LCD_G2    3
#define LCD_G3    46
#define LCD_G4    9
#define LCD_G5    10

#define LCD_B0    4
#define LCD_B1    5
#define LCD_B2    6
#define LCD_B3    7
#define LCD_B4    15

class DisplayDriver {
public:
    DisplayDriver();
    void init();
    void flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t *color_p);
    void setBacklight(uint8_t brightness);
    
private:
    void initPins();
    void initDisplay();
    void sendCommand(uint8_t cmd);
    void sendData(uint8_t data);
    
    SPIClass *spi;
};

#endif // DISPLAY_DRIVER_H
