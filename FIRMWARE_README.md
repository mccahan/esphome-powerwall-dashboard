# ESP32-S3 LVGL Display Firmware

This firmware provides a native ESP32-S3 implementation (without ESPHome) for the Guition ESP32-S3-4848S040 board with a 480x480 display.

## Features

- **LVGL Graphics**: Beautiful UI rendering with LVGL v8.3
- **480x480 Display**: Full support for ST7701S RGB display
- **Improv WiFi**: Easy WiFi configuration via serial interface
- **Web Installer**: Flash directly from your browser using ESP Web Tools
- **Display Test**: Colored squares demo to verify display functionality

## Hardware

This firmware is designed for:
- **Board**: Guition ESP32-S3-4848S040
- **Display**: 480x480 ST7701S RGB interface
- **Chip**: ESP32-S3 with 16MB flash and PSRAM

## Quick Start

### Option 1: Web Installer (Easiest)

1. Visit the [web installer](https://mccahan.github.io/esphome-powerwall-dashboard/)
2. Connect your ESP32-S3 via USB
3. Click "Install to ESP32-S3"
4. Follow the on-screen instructions

### Option 2: Build from Source

#### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- ESP32-S3 board connected via USB

#### Build and Upload

```bash
# Clone the repository
git clone https://github.com/mccahan/esphome-powerwall-dashboard.git
cd esphome-powerwall-dashboard

# Build and upload
pio run --target upload

# Monitor serial output
pio device monitor
```

## WiFi Configuration

After flashing, configure WiFi using the serial monitor:

```
# Scan for networks
SCAN

# Connect to WiFi
WIFI:your-ssid,your-password

# Check status
STATUS

# Get device info
INFO
```

## Project Structure

```
├── src/
│   ├── main.cpp              # Main application
│   ├── display_driver.cpp    # ST7701S display driver
│   └── improv_wifi.cpp       # Improv WiFi implementation
├── include/
│   ├── display_driver.h      # Display driver header
│   ├── improv_wifi.h         # Improv WiFi header
│   └── lv_conf.h            # LVGL configuration
├── docs/
│   ├── index.html           # Web installer page
│   └── manifest.json        # ESP Web Tools manifest
└── platformio.ini           # PlatformIO configuration
```

## Display Test

The firmware displays a 3x3 grid of colored squares:
- Red, Green, Blue (top row)
- Yellow, Magenta, Cyan (middle row)
- Orange, Purple, White (bottom row)

This verifies that the display is working correctly.

## Technical Details

### Display Interface

The ST7701S display uses an RGB parallel interface with the following pins:

- RGB Data: 16 pins for RGB565 color
- Control: HSYNC, VSYNC, DE, PCLK
- Backlight: PWM-controlled on GPIO38

### LVGL Configuration

- Color depth: 16-bit (RGB565)
- Resolution: 480x480
- Double buffering with 40-line buffers
- Hardware acceleration where available

### Improv WiFi

Based on the [Improv WiFi specification](https://www.improv-wifi.com/), allowing:
- WiFi credential provisioning
- Network scanning
- Status reporting
- Persistent credential storage

## Development

### Adding Custom UI

Edit `src/main.cpp` and modify the `create_test_display()` function to create your own LVGL UI elements.

### Display Driver

The display driver (`src/display_driver.cpp`) handles initialization and pixel data transfer. Modify this file to adjust display parameters or add features.

### WiFi Features

Extend `src/improv_wifi.cpp` to add custom WiFi features, such as:
- Web server for configuration
- mDNS/DNS-SD service discovery
- MQTT connectivity
- Home Assistant integration

## Troubleshooting

### Display not working

1. Check all connections are secure
2. Verify backlight is on (GPIO38)
3. Check serial output for error messages

### WiFi not connecting

1. Use `SCAN` to verify network is visible
2. Check SSID and password are correct
3. Ensure 2.4GHz WiFi is available (5GHz not supported)

### Build errors

1. Update PlatformIO: `pio upgrade`
2. Clean build: `pio run --target clean`
3. Check all dependencies are installed

### SHA-256 comparison failed / Boot loop

If you see errors like:
```
SHA-256 comparison failed:
Calculated: cf780b40783e49f19c34ce87853c75dccf8548dc116c9d41cfb6194a2f332177
Expected: 13a1a0aa54262123c6d1b1ae881977f54f41eddd693e1918c55e35a98727c4b5
```

This was caused by incorrect flash offsets in the web installer. **This has been fixed** - the web installer now flashes:
- Bootloader at offset 0x0
- Partition table at offset 0x8000
- Application firmware at offset 0x10000

To fix an already-flashed device:
1. Visit the [web installer](https://mccahan.github.io/esphome-powerwall-dashboard/)
2. Flash the firmware again using the updated installer
3. Select "Erase all flash before installing" when prompted

## License

This project is provided as-is for educational and development purposes.

## Acknowledgments

- Hardware configuration based on [esphome-modular-lvgl-buttons](https://github.com/agillis/esphome-modular-lvgl-buttons)
- [LVGL](https://lvgl.io/) for the graphics library
- [Improv WiFi](https://www.improv-wifi.com/) for the WiFi provisioning protocol
- [ESP Web Tools](https://esphome.github.io/esp-web-tools/) for web-based flashing
