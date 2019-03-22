# Thermal Camera with Photo Gallery

# About
Uses thermal camera to read temperature data from the environment and calculates corresponding pixel data that it puts in a frame buffer, to be displayed through an HDMI cable.
The project also implements a photo gallery wherein you can take a picture and scroll through previously captured pictures.


## How to Set Up Project
1. Connect a SparkFun MLX90640 (Qwicc) IR Array to Raspberry Pi. Use the standard pins for I2C communication on the RPi.
2. Connect a PS/2 Keyboard to the Raspberry Pi; use GPIO 4 for the DATA line, and GPIO 17 for the CLOCK line.
3. Run `make install` in the ThermalCamPi folder, sending the data to the RPi through a USB to UART device and bootloader. Alternatively, run `make` in the ThermalCamPi folder, copy `main.bin` into the RPi SD card, and rename it `kernel.img`.

# Using the Application
The application has two modes: **stream** and **gallery**.

In **stream** mode, the camera transmits a live stream of the thermal temperature readings. For the default iron gradient, colors closer to blue/purple indicate colder temperatures, and colors closer to orange/yellow indicate warmer temperatures. The default color range is 20ºC to 40ºC.

In **gallery** mode, the user can move between different saved images that were taken during stream mode. The maximum allowed number of images is 24.

## Commands
- `spacebar` — capture image (use in **stream** mode)
- `e` — toggle **gallery** mode
- `j` — go to previous image
- `l` — go to next image

### References
- [MLX90640 Data Sheet from Melexsis](https://www.melexis.com/-/media/files/documents/datasheets/mlx90640-datasheet-melexis.pdf)
- [Melexsis GitHub Repository for MLX90640 for Arduino API](https://github.com/melexis/mlx90640-library)
- [CS 107e library modules](https://github.com/cs107e/cs107e.github.io)
