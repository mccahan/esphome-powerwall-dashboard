/**
 * Improv WiFi Integration
 * Allows WiFi configuration via serial/Bluetooth
 * Based on Improv Wi-Fi Serial specification
 */

#ifndef IMPROV_WIFI_H
#define IMPROV_WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <vector>

// Improv Serial Protocol Constants
#define IMPROV_SERIAL_VERSION 1

enum ImprovSerialMessageType {
    IMPROV_CURRENT_STATE = 0x01,
    IMPROV_ERROR_STATE = 0x02,
    IMPROV_RPC = 0x03,
    IMPROV_RPC_RESULT = 0x04
};

enum ImprovSerialCurrentState {
    IMPROV_STATE_READY = 0x02,        // Authorized, ready for provisioning
    IMPROV_STATE_PROVISIONING = 0x03, // Attempting to connect
    IMPROV_STATE_PROVISIONED = 0x04   // Connected to WiFi
};

enum ImprovSerialErrorState {
    IMPROV_ERROR_NONE = 0x00,
    IMPROV_ERROR_INVALID_RPC = 0x01,
    IMPROV_ERROR_UNKNOWN_RPC = 0x02,
    IMPROV_ERROR_UNABLE_TO_CONNECT = 0x03,
    IMPROV_ERROR_UNKNOWN = 0xFF
};

enum ImprovSerialRPCCommand {
    IMPROV_RPC_SEND_WIFI = 0x01,
    IMPROV_RPC_REQUEST_STATE = 0x02,
    IMPROV_RPC_REQUEST_INFO = 0x03,
    IMPROV_RPC_REQUEST_SCAN = 0x04
};

class ImprovWiFi {
public:
    ImprovWiFi();
    void begin();
    void loop();
    
private:
    void handleSerial();
    void sendPacket(uint8_t type, const std::vector<uint8_t>& data);
    void sendCurrentState();
    void sendErrorState(uint8_t error);
    void sendRPCResult(uint8_t command, const std::vector<String>& strings);
    void sendDeviceInfo();
    void connectWiFi(const String& ssid, const String& password);
    void handleRPCCommand(uint8_t command, const std::vector<uint8_t>& data);
    
    std::vector<uint8_t> buildStringList(const std::vector<String>& strings);
    uint8_t calculateChecksum(const std::vector<uint8_t>& data);
    
    ImprovSerialCurrentState currentState;
    ImprovSerialErrorState errorState;
    unsigned long lastStatusUpdate;
    std::vector<uint8_t> inputBuffer;
};

#endif // IMPROV_WIFI_H
