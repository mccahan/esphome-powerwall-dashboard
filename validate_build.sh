#!/bin/bash
# Build validation script

set -euo pipefail

echo "=== ESP32-S3 LVGL Firmware Build Validation ==="
echo ""

# Check required files exist
echo "Checking project structure..."
required_files=(
    "platformio.ini"
    "src/main.cpp"
    "src/display_driver.cpp"
    "src/improv_wifi.cpp"
    "include/display_driver.h"
    "include/improv_wifi.h"
    "include/lv_conf.h"
    "docs/index.html"
    "docs/manifest.json"
)

for file in "${required_files[@]}"; do
    if [ -f "$file" ]; then
        echo "  ✓ $file"
    else
        echo "  ✗ $file (MISSING)"
        exit 1
    fi
done

echo ""
echo "Checking source code structure..."

# Check main.cpp has required functions
if grep -q "void setup()" src/main.cpp && \
   grep -q "void loop()" src/main.cpp && \
   grep -q "create_test_display" src/main.cpp; then
    echo "  ✓ main.cpp structure valid"
else
    echo "  ✗ main.cpp missing required functions"
    exit 1
fi

# Check display driver
if grep -q "class DisplayDriver" include/display_driver.h && \
   grep -q "void init()" include/display_driver.h && \
   grep -q "void flush(" include/display_driver.h; then
    echo "  ✓ display_driver.h structure valid"
else
    echo "  ✗ display_driver.h missing required methods"
    exit 1
fi

# Check Improv WiFi
if grep -q "class ImprovWiFi" include/improv_wifi.h && \
   grep -q "void begin()" include/improv_wifi.h && \
   grep -q "void loop()" include/improv_wifi.h; then
    echo "  ✓ improv_wifi.h structure valid"
else
    echo "  ✗ improv_wifi.h missing required methods"
    exit 1
fi

# Check LVGL config
if grep -q "LV_FONT_MONTSERRAT_16" include/lv_conf.h && \
   grep -q "LV_FONT_MONTSERRAT_24" include/lv_conf.h && \
   grep -q "LV_COLOR_DEPTH" include/lv_conf.h; then
    echo "  ✓ lv_conf.h has required font definitions"
else
    echo "  ✗ lv_conf.h missing required LVGL configuration"
    exit 1
fi

# Check web installer
if grep -q "esp-web-install-button" docs/index.html && \
   grep -q "manifest.json" docs/index.html; then
    echo "  ✓ Web installer configured correctly"
else
    echo "  ✗ Web installer missing required components"
    exit 1
fi

# Check manifest
if grep -q "ESP32-S3" docs/manifest.json && \
   grep -q "firmware.bin" docs/manifest.json && \
   grep -q "bootloader.bin" docs/manifest.json && \
   grep -q "partitions.bin" docs/manifest.json && \
   grep -q '"offset": 0' docs/manifest.json && \
   grep -q '"offset": 32768' docs/manifest.json && \
   grep -q '"offset": 65536' docs/manifest.json; then
    echo "  ✓ Manifest configured for ESP32-S3 with correct flash offsets"
else
    echo "  ✗ Manifest missing required configuration or incorrect flash offsets"
    exit 1
fi

echo ""
echo "Checking PlatformIO configuration..."

# Check platformio.ini
if grep -q "platform = espressif32" platformio.ini && \
   grep -q "board = esp32-s3-devkitc-1" platformio.ini && \
   grep -q "framework = arduino" platformio.ini && \
   grep -q "lvgl/lvgl" platformio.ini; then
    echo "  ✓ platformio.ini configured correctly"
else
    echo "  ✗ platformio.ini missing required configuration"
    exit 1
fi

echo ""
echo "=== All validation checks passed! ==="
echo ""
echo "Note: This validation checks project structure only."
echo "The actual build requires network access to download dependencies."
echo "GitHub Actions CI will perform the full build with network access."
