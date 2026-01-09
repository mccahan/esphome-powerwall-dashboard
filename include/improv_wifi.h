/**
 * Improv WiFi Integration
 * Allows WiFi configuration via serial/Bluetooth
 */

#ifndef IMPROV_WIFI_H
#define IMPROV_WIFI_H

#include <Arduino.h>
#include <WiFi.h>

class ImprovWiFi {
public:
    ImprovWiFi();
    void begin();
    void loop();
    
private:
    void handleSerial();
    void sendDeviceInfo();
    void sendWiFiStatus();
    void connectWiFi(const String& ssid, const String& password);
    
    bool isProvisioning;
    unsigned long lastStatusUpdate;
};

#endif // IMPROV_WIFI_H
