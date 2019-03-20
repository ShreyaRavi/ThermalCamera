# Wireless Thermal Camera

## How to Set Up Each Project
1. Run `make` in the the corresponding directory
2. Move `main.bin` from corresponding directory to Raspberry Pi SD card on corresponding Pi
3. Remove `kernel.img` on current Raspberry Pi SD card
4. Rename `main.bin` as `kernel.img` on current Raspberry Pi SD card
5. Put SD card bag into slot in corresponding Pi and power up Pi!

# ThermalCamPi Project
Uses thermal camera to read temperature data from the environment and calculates corresponding pixel data that it sends to an ESP32 serverd

# DisplayPi Project
Reads thermal image pixel data from an ESP32 client and displays image to an HDMI monitor
