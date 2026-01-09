This project uses ESPHome as a base to deploy a Tesla Powerwall home monitoring display, built on lvgl and pypowerwall.

It acquires values over MQTT from pypowerwall (which, itself, is able to grab them as they get used elsewhere).

## ESPHome Setup

To use the ESPHome-based firmware:

1. **Configure WiFi credentials**: Copy `secrets.yaml.example` to `secrets.yaml` and update with your WiFi credentials:
   ```bash
   cp secrets.yaml.example secrets.yaml
   # Edit secrets.yaml with your actual WiFi SSID and password
   ```

2. **Install ESPHome**: Follow the [ESPHome installation guide](https://esphome.io/guides/getting_started_command_line.html)

3. **Compile and upload**: 
   ```bash
   esphome run bedside-display.yml
   ```

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
