/**
 * Improv WiFi Implementation
 * Based on Improv Wi-Fi specification: https://www.improv-wifi.com/
 */

#include "improv_wifi.h"
#include <Preferences.h>

// Improv serial protocol constants
#define IMPROV_SERIAL_VERSION 1

Preferences preferences;

ImprovWiFi::ImprovWiFi() {
    isProvisioning = false;
    lastStatusUpdate = 0;
}

void ImprovWiFi::begin() {
    Serial.println("Initializing Improv WiFi...");
    
    preferences.begin("wifi", false);
    
    // Try to load saved WiFi credentials
    String savedSSID = preferences.getString("ssid", "");
    String savedPassword = preferences.getString("password", "");
    
    if (savedSSID.length() > 0) {
        Serial.println("Attempting to connect to saved WiFi: " + savedSSID);
        connectWiFi(savedSSID, savedPassword);
    } else {
        Serial.println("No saved WiFi credentials found");
        Serial.println("Waiting for Improv WiFi provisioning...");
    }
    
    // Send device info for Improv
    sendDeviceInfo();
}

void ImprovWiFi::loop() {
    // Handle serial commands for Improv WiFi
    handleSerial();
    
    // Send periodic WiFi status updates
    if (millis() - lastStatusUpdate > 5000) {
        sendWiFiStatus();
        lastStatusUpdate = millis();
    }
}

void ImprovWiFi::handleSerial() {
    if (!Serial.available()) {
        return;
    }
    
    // Read serial data for Improv protocol
    // This is a simplified implementation
    // Full implementation would parse Improv serial protocol messages
    
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    // Simple command handling for testing
    if (input.startsWith("WIFI:")) {
        // Format: WIFI:ssid,password
        int commaPos = input.indexOf(',', 5);
        if (commaPos > 0) {
            String ssid = input.substring(5, commaPos);
            String password = input.substring(commaPos + 1);
            
            Serial.println("Attempting to connect to: " + ssid);
            connectWiFi(ssid, password);
        }
    } else if (input == "STATUS") {
        sendWiFiStatus();
    } else if (input == "INFO") {
        sendDeviceInfo();
    } else if (input == "SCAN") {
        Serial.println("Scanning for WiFi networks...");
        int n = WiFi.scanNetworks();
        Serial.println("Networks found: " + String(n));
        for (int i = 0; i < n; i++) {
            Serial.printf("  %d: %s (%d dBm) %s\n", 
                         i + 1, 
                         WiFi.SSID(i).c_str(), 
                         WiFi.RSSI(i),
                         (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Encrypted");
        }
    }
}

void ImprovWiFi::sendDeviceInfo() {
    Serial.println("=== Device Info ===");
    Serial.println("Device: ESP32-S3 LVGL Display");
    Serial.println("Chip: ESP32-S3");
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    Serial.println("Firmware: 1.0.0");
    Serial.println("Improv Version: 1");
    Serial.println("==================");
}

void ImprovWiFi::sendWiFiStatus() {
    Serial.print("WiFi Status: ");
    
    switch (WiFi.status()) {
        case WL_CONNECTED:
            Serial.println("Connected");
            Serial.print("  SSID: ");
            Serial.println(WiFi.SSID());
            Serial.print("  IP: ");
            Serial.println(WiFi.localIP());
            Serial.print("  RSSI: ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm");
            break;
        case WL_NO_SSID_AVAIL:
            Serial.println("No SSID Available");
            break;
        case WL_CONNECT_FAILED:
            Serial.println("Connection Failed");
            break;
        case WL_DISCONNECTED:
            Serial.println("Disconnected");
            break;
        default:
            Serial.println("Unknown");
            break;
    }
}

void ImprovWiFi::connectWiFi(const String& ssid, const String& password) {
    Serial.println("Connecting to WiFi...");
    Serial.println("SSID: " + ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        
        // Save credentials
        preferences.putString("ssid", ssid);
        preferences.putString("password", password);
        
        sendWiFiStatus();
    } else {
        Serial.println("WiFi Connection Failed!");
        sendWiFiStatus();
    }
}
