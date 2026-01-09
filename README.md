This project uses ESPHome as a base to deploy a Tesla Powerwall home monitoring display, built on lvgl and pypowerwall.

It acquires values over MQTT from pypowerwall (which, itself, is able to grab them as they get used elsewhere).

---

## Native ESP32-S3 LVGL Firmware

A new native ESP32-S3 firmware (without ESPHome) is now available with the following features:

- **480x480 RGB Display** with LVGL graphics library
- **Improv WiFi** for easy WiFi configuration
- **Web-based flashing** via ESP Web Tools
- **Display test** with colored squares

### Quick Start

Visit the [web installer](https://mccahan.github.io/esphome-powerwall-dashboard/) to flash the firmware directly from your browser.

For more information, see [FIRMWARE_README.md](FIRMWARE_README.md).
