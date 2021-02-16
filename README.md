# Open NSynth Super SH1106

![SH1106](sh1106.jpg)

This is slightly modified sources of Open NSynth Super for work with SH1106 OLED screen instead of SSD1306.
If you use your OLED with pin DC=0 (not connected or tied to GND), the default address of your SH1106 will be 0x3c,
Otherwise (pin DC=1 or connected to 3.3V), SH1106 address will be 0x3d and you don't need to replace or change original src/ofApp.h

### Notes
The files which was changed:
1. ![app/open-nsynth/src/OledScreenDriver.cpp](OledScreenDriver.cpp)
2. ![app/open-nsynth/src/ofApp.h](ofApp.h)

### Original source
The original project ![https://github.com/googlecreativelab/open-nsynth-super](https://github.com/googlecreativelab/open-nsynth-super)
