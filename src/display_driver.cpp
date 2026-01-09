/**
 * Display Driver Implementation for ST7701S
 */

#include "display_driver.h"
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_ops.h>
#include <driver/spi_master.h>

// SPI configuration
#define SPI_SPEED_HZ 2000000

static esp_lcd_panel_handle_t panel_handle = NULL;
static uint16_t *frame_buffer = NULL;

DisplayDriver::DisplayDriver() {
    spi = nullptr;
}

void DisplayDriver::init() {
    Serial.println("Initializing display driver...");
    
    // Set backlight pin
    pinMode(LCD_BL, OUTPUT);
    setBacklight(255);  // Full brightness
    
    // For ESP32-S3 with RGB interface, we need to use the ESP LCD component
    // This is a simplified implementation - in production, you'd use esp_lcd_panel_rgb
    
    // Allocate frame buffer
    frame_buffer = (uint16_t*)heap_caps_malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t), 
                                               MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    
    if (frame_buffer == NULL) {
        Serial.println("Failed to allocate frame buffer!");
        // Try without SPIRAM
        frame_buffer = (uint16_t*)malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t));
    }
    
    if (frame_buffer != NULL) {
        Serial.println("Frame buffer allocated successfully");
        // Clear frame buffer
        memset(frame_buffer, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t));
    } else {
        Serial.println("Failed to allocate frame buffer!");
    }
    
    // Initialize SPI for command interface
    spi = new SPIClass(HSPI);
    spi->begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);
    
    // Initialize display
    initDisplay();
    
    Serial.println("Display driver initialized");
}

void DisplayDriver::initDisplay() {
    // ST7701S initialization sequence
    // Based on the hardware configuration
    
    delay(120);
    
    // Send initialization commands via SPI
    digitalWrite(TFT_CS, LOW);
    
    // Command 2 BK0 Select
    sendCommand(0xFF);
    sendData(0x77);
    sendData(0x01);
    sendData(0x00);
    sendData(0x00);
    sendData(0x10);
    
    // Disable MDT flag
    sendCommand(0xCD);
    sendData(0x00);
    
    digitalWrite(TFT_CS, HIGH);
    
    delay(120);
}

void DisplayDriver::sendCommand(uint8_t cmd) {
    spi->beginTransaction(SPISettings(SPI_SPEED_HZ, MSBFIRST, SPI_MODE3));
    spi->transfer(cmd);
    spi->endTransaction();
}

void DisplayDriver::sendData(uint8_t data) {
    spi->beginTransaction(SPISettings(SPI_SPEED_HZ, MSBFIRST, SPI_MODE3));
    spi->transfer(data);
    spi->endTransaction();
}

void DisplayDriver::flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t *color_p) {
    // Copy LVGL buffer to frame buffer
    if (frame_buffer == NULL) return;
    
    int32_t w = x2 - x1 + 1;
    int32_t h = y2 - y1 + 1;
    
    for (int32_t y = y1; y <= y2; y++) {
        for (int32_t x = x1; x <= x2; x++) {
            int32_t idx = (y - y1) * w + (x - x1);
            int32_t fb_idx = y * DISPLAY_WIDTH + x;
            
            if (fb_idx < DISPLAY_WIDTH * DISPLAY_HEIGHT) {
                frame_buffer[fb_idx] = color_p[idx].full;
            }
        }
    }
    
    // In a full implementation, this would trigger a DMA transfer to the display
    // For now, we're using a software approach with the frame buffer
}

void DisplayDriver::setBacklight(uint8_t brightness) {
    // Use PWM for backlight control
    ledcSetup(0, 5000, 8);  // Channel 0, 5kHz, 8-bit resolution
    ledcAttachPin(LCD_BL, 0);
    ledcWrite(0, brightness);
}
