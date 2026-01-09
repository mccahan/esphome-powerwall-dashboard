/**
 * Improv WiFi Implementation
 * Based on Improv Wi-Fi Serial specification: https://www.improv-wifi.com/
 */

#include "improv_wifi.h"
#include <Preferences.h>

// Improv serial protocol constants
#define MAX_WIFI_ATTEMPTS 20

Preferences preferences;

ImprovWiFi::ImprovWiFi() {
    currentState = IMPROV_STATE_READY;
    errorState = IMPROV_ERROR_NONE;
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
    
    // Send initial state packet for Improv detection
    delay(100);  // Small delay to ensure serial is ready
    sendCurrentState();
}

void ImprovWiFi::loop() {
    // Handle serial commands for Improv WiFi
    handleSerial();
    
    // Send periodic state updates
    if (millis() - lastStatusUpdate > 5000) {
        sendCurrentState();
        lastStatusUpdate = millis();
    }
}

void ImprovWiFi::handleSerial() {
    while (Serial.available()) {
        uint8_t byte = Serial.read();
        inputBuffer.push_back(byte);
        
        // Check if we have enough bytes to determine if it's an Improv packet
        if (inputBuffer.size() >= 9) {
            // Check for "IMPROV" header
            if (inputBuffer[0] == 'I' && inputBuffer[1] == 'M' && 
                inputBuffer[2] == 'P' && inputBuffer[3] == 'R' && 
                inputBuffer[4] == 'O' && inputBuffer[5] == 'V') {
                
                uint8_t version = inputBuffer[6];
                uint8_t type = inputBuffer[7];
                uint8_t length = inputBuffer[8];
                
                // Validate length to prevent buffer issues
                if (length > 200) {  // Reasonable max length for Improv packets
                    Serial.println("Improv: Packet length too large");
                    inputBuffer.clear();
                    continue;
                }
                
                // Wait for complete packet: header(6) + version(1) + type(1) + length(1) + data(length) + checksum(1)
                uint16_t expectedSize = 9 + length + 1;
                
                if (inputBuffer.size() >= expectedSize) {
                    // Verify checksum
                    uint8_t receivedChecksum = inputBuffer[expectedSize - 1];
                    uint8_t calculatedChecksum = 0;
                    for (size_t i = 0; i < expectedSize - 1; i++) {
                        calculatedChecksum += inputBuffer[i];
                    }
                    
                    if (receivedChecksum == calculatedChecksum) {
                        // Valid packet - handle it
                        if (type == IMPROV_RPC && version == IMPROV_SERIAL_VERSION) {
                            if (length > 0) {
                                uint8_t command = inputBuffer[9];
                                std::vector<uint8_t> data(inputBuffer.begin() + 10, inputBuffer.begin() + 9 + length);
                                handleRPCCommand(command, data);
                            }
                        }
                    } else {
                        Serial.println("Improv: Checksum mismatch");
                    }
                    
                    // Clear buffer
                    inputBuffer.clear();
                }
            } else {
                // Not an Improv packet - clear the first byte
                inputBuffer.erase(inputBuffer.begin());
            }
        }
    }
}

void ImprovWiFi::handleRPCCommand(uint8_t command, const std::vector<uint8_t>& data) {
    Serial.printf("Improv RPC Command: 0x%02X\n", command);
    
    switch (command) {
        case IMPROV_RPC_REQUEST_STATE:
            Serial.println("Request: Current State");
            sendCurrentState();
            // If already provisioned, send the redirect URL
            if (currentState == IMPROV_STATE_PROVISIONED) {
                std::vector<String> urls;
                urls.push_back("http://" + WiFi.localIP().toString());
                sendRPCResult(command, urls);
            }
            break;
            
        case IMPROV_RPC_REQUEST_INFO:
            Serial.println("Request: Device Info");
            sendDeviceInfo();
            break;
            
        case IMPROV_RPC_REQUEST_SCAN:
            Serial.println("Request: WiFi Scan");
            {
                int n = WiFi.scanNetworks();
                Serial.printf("Found %d networks\n", n);
                
                for (int i = 0; i < n; i++) {
                    std::vector<String> network;
                    network.push_back(WiFi.SSID(i));
                    network.push_back(String(WiFi.RSSI(i)));
                    network.push_back((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "NO" : "YES");
                    sendRPCResult(command, network);
                }
                
                // Send empty result to signal end of scan
                std::vector<String> empty;
                sendRPCResult(command, empty);
            }
            break;
            
        case IMPROV_RPC_SEND_WIFI:
            Serial.println("Request: Send WiFi Settings");
            {
                // Parse WiFi credentials from data
                // Format: <ssid_length><ssid><password_length><password>
                if (data.size() >= 2) {
                    uint8_t ssidLength = data[0];
                    if (data.size() >= 1 + ssidLength + 1) {
                        String ssid;
                        for (int i = 0; i < ssidLength; i++) {
                            ssid += (char)data[1 + i];
                        }
                        
                        uint8_t passwordLength = data[1 + ssidLength];
                        String password;
                        for (int i = 0; i < passwordLength; i++) {
                            password += (char)data[1 + ssidLength + 1 + i];
                        }
                        
                        Serial.printf("Connecting to: %s\n", ssid.c_str());
                        connectWiFi(ssid, password);
                        
                        if (currentState == IMPROV_STATE_PROVISIONED) {
                            // Send success with redirect URL
                            std::vector<String> result;
                            result.push_back("http://" + WiFi.localIP().toString());
                            sendRPCResult(command, result);
                        }
                    }
                }
            }
            break;
            
        default:
            Serial.printf("Unknown RPC command: 0x%02X\n", command);
            sendErrorState(IMPROV_ERROR_UNKNOWN_RPC);
            break;
    }
}

void ImprovWiFi::sendDeviceInfo() {
    std::vector<String> info;
    info.push_back("ESP32-LVGL-Display");  // Firmware name
    info.push_back("1");                    // Firmware version
    info.push_back("ESP32-S3");            // Chip family
    info.push_back("ESP32-S3 Display");    // Device name
    
    sendRPCResult(IMPROV_RPC_REQUEST_INFO, info);
    
    // Also log to serial for debugging
    Serial.println("=== Device Info ===");
    Serial.println("Device: ESP32-S3 LVGL Display");
    Serial.println("Chip: ESP32-S3");
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    Serial.println("Firmware: 1.0.0");
    Serial.println("Improv Version: 1");
    Serial.println("==================");
}

void ImprovWiFi::sendCurrentState() {
    // Update state based on WiFi connection
    if (WiFi.status() == WL_CONNECTED) {
        currentState = IMPROV_STATE_PROVISIONED;
    } else {
        currentState = IMPROV_STATE_READY;
    }
    
    std::vector<uint8_t> data;
    data.push_back(currentState);
    sendPacket(IMPROV_CURRENT_STATE, data);
    
    // Debug output
    Serial.print("Improv State: ");
    switch (currentState) {
        case IMPROV_STATE_READY:
            Serial.println("READY");
            break;
        case IMPROV_STATE_PROVISIONING:
            Serial.println("PROVISIONING");
            break;
        case IMPROV_STATE_PROVISIONED:
            Serial.print("PROVISIONED - IP: ");
            Serial.println(WiFi.localIP());
            break;
    }
}

void ImprovWiFi::sendErrorState(uint8_t error) {
    errorState = (ImprovSerialErrorState)error;
    std::vector<uint8_t> data;
    data.push_back(error);
    sendPacket(IMPROV_ERROR_STATE, data);
    
    Serial.printf("Improv Error: 0x%02X\n", error);
}

void ImprovWiFi::sendRPCResult(uint8_t command, const std::vector<String>& strings) {
    std::vector<uint8_t> data;
    data.push_back(command);
    
    // Build string list
    std::vector<uint8_t> stringData = buildStringList(strings);
    
    // Check if total length fits in uint8_t
    if (stringData.size() > 255) {
        Serial.println("Improv: String data too large for packet");
        return;
    }
    
    data.push_back(stringData.size());  // Total length of string data
    data.insert(data.end(), stringData.begin(), stringData.end());
    
    sendPacket(IMPROV_RPC_RESULT, data);
}

std::vector<uint8_t> ImprovWiFi::buildStringList(const std::vector<String>& strings) {
    std::vector<uint8_t> result;
    
    for (const String& str : strings) {
        // Truncate strings that are too long
        size_t len = str.length();
        if (len > 255) {
            len = 255;
            Serial.println("Improv: String truncated to 255 bytes");
        }
        
        result.push_back(len);
        for (size_t i = 0; i < len; i++) {
            result.push_back(str[i]);
        }
    }
    
    return result;
}

void ImprovWiFi::sendPacket(uint8_t type, const std::vector<uint8_t>& data) {
    std::vector<uint8_t> packet;
    
    // Add "IMPROV" header
    packet.push_back('I');
    packet.push_back('M');
    packet.push_back('P');
    packet.push_back('R');
    packet.push_back('O');
    packet.push_back('V');
    
    // Add version
    packet.push_back(IMPROV_SERIAL_VERSION);
    
    // Add type
    packet.push_back(type);
    
    // Add length
    packet.push_back(data.size());
    
    // Add data
    packet.insert(packet.end(), data.begin(), data.end());
    
    // Calculate and add checksum
    uint8_t checksum = calculateChecksum(packet);
    packet.push_back(checksum);
    
    // Send packet
    Serial.write(packet.data(), packet.size());
    
    // Send newline
    Serial.write('\n');
}

uint8_t ImprovWiFi::calculateChecksum(const std::vector<uint8_t>& data) {
    uint8_t checksum = 0;
    for (uint8_t byte : data) {
        checksum += byte;
    }
    return checksum;
}

void ImprovWiFi::connectWiFi(const String& ssid, const String& password) {
    Serial.println("Connecting to WiFi...");
    Serial.println("SSID: " + ssid);
    
    // Set state to provisioning
    currentState = IMPROV_STATE_PROVISIONING;
    sendCurrentState();
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < MAX_WIFI_ATTEMPTS) {
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
        
        // Update state to provisioned
        currentState = IMPROV_STATE_PROVISIONED;
        errorState = IMPROV_ERROR_NONE;
        sendCurrentState();
    } else {
        Serial.println("WiFi Connection Failed!");
        
        // Set error state
        currentState = IMPROV_STATE_READY;
        sendCurrentState();
        sendErrorState(IMPROV_ERROR_UNABLE_TO_CONNECT);
    }
}
