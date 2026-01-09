# Display Layout Visualization

## 480x480 Display Test Pattern

```
┌────────────────────────────────────────────────────────┐
│                                                        │
│    WiFi: [Status] (IP Address if connected)           │
│                                                        │
│                                                        │
│                                                        │
│      ┌──────┐    ┌──────┐    ┌──────┐                │
│      │      │    │      │    │      │                │
│      │ RED  │    │GREEN │    │ BLUE │                │
│      │      │    │      │    │      │                │
│      └──────┘    └──────┘    └──────┘                │
│                                                        │
│      ┌──────┐    ┌──────┐    ┌──────┐                │
│      │      │    │      │    │      │                │
│      │YELLOW│    │MAGNT │    │ CYAN │                │
│      │      │    │      │    │      │                │
│      └──────┘    └──────┘    └──────┘                │
│                                                        │
│      ┌──────┐    ┌──────┐    ┌──────┐                │
│      │      │    │      │    │      │                │
│      │ORNG  │    │PURPLE│    │WHITE │                │
│      │      │    │      │    │      │                │
│      └──────┘    └──────┘    └──────┘                │
│                                                        │
│                                                        │
│                                                        │
│                                                        │
│        ESP32-S3 LVGL Display Test                     │
│                                                        │
└────────────────────────────────────────────────────────┘
```

## Specifications

### Screen
- **Resolution**: 480x480 pixels
- **Color Depth**: RGB565 (16-bit)
- **Background**: Black (#000000)

### Colored Squares
- **Size**: 80x80 pixels each
- **Layout**: 3x3 grid
- **Spacing**: 20 pixels between squares
- **Border Radius**: 10 pixels (rounded corners)
- **Border**: None
- **Starting Position**: (40, 40) from top-left

### Colors
Row 1:
- **Red**: RGB(255, 0, 0)
- **Green**: RGB(0, 255, 0)
- **Blue**: RGB(0, 0, 255)

Row 2:
- **Yellow**: RGB(255, 255, 0)
- **Magenta**: RGB(255, 0, 255)
- **Cyan**: RGB(0, 255, 255)

Row 3:
- **Orange**: RGB(255, 128, 0)
- **Purple**: RGB(128, 0, 255)
- **White**: RGB(255, 255, 255)

### Text Elements

#### Title (Bottom)
- **Font**: Montserrat 24
- **Color**: White
- **Text**: "ESP32-S3 LVGL Display Test"
- **Position**: Bottom center, 20px from bottom
- **Alignment**: Center

#### WiFi Status (Top)
- **Font**: Montserrat 16
- **Color**: White
- **Text**: "WiFi: [Status]" or "WiFi: [SSID] ([IP])"
- **Position**: Top center, 10px from top
- **Alignment**: Center

## Display Test Purpose

This test pattern serves multiple purposes:

1. **Color Verification**: Ensures all color channels (R, G, B) are functioning
2. **Pixel Accuracy**: Verifies precise rendering at specified coordinates
3. **LVGL Functionality**: Confirms LVGL is properly initialized and rendering
4. **Display Orientation**: Validates correct display rotation setting
5. **WiFi Status**: Shows network connectivity information

## Expected Behavior

On boot, the display should:
1. Show black background immediately
2. Display 9 colored squares in a grid pattern
3. Show title text at bottom
4. Display WiFi status at top (initially "Not Connected")
5. Update WiFi status when connection is established

## Troubleshooting

If display shows:
- **All white/black**: Check display initialization sequence
- **Wrong colors**: Verify RGB pin mappings
- **Rotated/mirrored**: Adjust rotation setting in display driver
- **Flickering**: Check PCLK frequency and timing parameters
- **No display**: Verify backlight is enabled (GPIO38)
