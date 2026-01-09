/**
 * ESP32-S3 LVGL Display with Improv Wi-Fi
 * Hardware: Guition ESP32-S3-4848S040 (480x480 display)
 */

#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include "display_driver.h"
#include "improv_wifi.h"

// LVGL display buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[DISPLAY_WIDTH * 40];
static lv_color_t buf2[DISPLAY_WIDTH * 40];
static lv_disp_drv_t disp_drv;

// Display driver instance
DisplayDriver displayDriver;

// Improv WiFi instance
ImprovWiFi improvWiFi;

// LVGL display flush callback
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    displayDriver.flush(area->x1, area->y1, area->x2, area->y2, color_p);
    lv_disp_flush_ready(disp);
}

// Create colored squares for display test
void create_test_display() {
    // Clear screen with black background
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    
    // Create a grid of colored squares
    const int square_size = 80;
    const int spacing = 20;
    const int start_x = 40;
    const int start_y = 40;
    
    // Define colors for the squares
    lv_color_t colors[] = {
        lv_color_make(255, 0, 0),     // Red
        lv_color_make(0, 255, 0),     // Green
        lv_color_make(0, 0, 255),     // Blue
        lv_color_make(255, 255, 0),   // Yellow
        lv_color_make(255, 0, 255),   // Magenta
        lv_color_make(0, 255, 255),   // Cyan
        lv_color_make(255, 128, 0),   // Orange
        lv_color_make(128, 0, 255),   // Purple
        lv_color_make(255, 255, 255), // White
    };
    
    int color_idx = 0;
    
    // Create 3x3 grid of colored squares
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            lv_obj_t *square = lv_obj_create(scr);
            lv_obj_set_size(square, square_size, square_size);
            lv_obj_set_pos(square, start_x + col * (square_size + spacing), 
                          start_y + row * (square_size + spacing));
            lv_obj_set_style_bg_color(square, colors[color_idx], 0);
            lv_obj_set_style_border_width(square, 0, 0);
            lv_obj_set_style_radius(square, 10, 0);
            
            color_idx++;
        }
    }
    
    // Add a title label
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "ESP32-S3 LVGL Display Test");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -20);
    
    // Add WiFi status label
    lv_obj_t *wifi_label = lv_label_create(scr);
    if (WiFi.status() == WL_CONNECTED) {
        String status = "WiFi: " + WiFi.SSID() + " (" + WiFi.localIP().toString() + ")";
        lv_label_set_text(wifi_label, status.c_str());
    } else {
        lv_label_set_text(wifi_label, "WiFi: Not Connected");
    }
    lv_obj_set_style_text_color(wifi_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(wifi_label, &lv_font_montserrat_16, 0);
    lv_obj_align(wifi_label, LV_ALIGN_TOP_MID, 0, 10);
}

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32-S3 LVGL Display Starting...");
    
    // Initialize display driver
    displayDriver.init();
    Serial.println("Display initialized");
    
    // Initialize LVGL
    lv_init();
    Serial.println("LVGL initialized");
    
    // Initialize display buffer
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, DISPLAY_WIDTH * 40);
    
    // Initialize display driver for LVGL
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = DISPLAY_WIDTH;
    disp_drv.ver_res = DISPLAY_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    
    Serial.println("LVGL display driver registered");
    
    // Initialize Improv WiFi
    improvWiFi.begin();
    Serial.println("Improv WiFi initialized");
    
    // Create test display
    create_test_display();
    Serial.println("Test display created");
    
    Serial.println("Setup complete!");
}

void loop() {
    // Handle LVGL tasks
    lv_timer_handler();
    
    // Handle Improv WiFi
    improvWiFi.loop();
    
    delay(5);
}
