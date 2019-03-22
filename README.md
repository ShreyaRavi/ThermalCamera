# Thermal Camera with Photo Gallery

## How to Set Up Project
1. Connect a SparkFun MLX90640 (Qwicc) IR Array to Raspberry Pi. Use the standard pins for I2C communication on the RPi.
2. Connect a PS/2 Keyboard to the Raspberry Pi; use GPIO 4 for the DATA line, and GPIO 17 for the CLOCK line.
3. Run `make install` in the ThermalCamPi folder, sending the data to the RPi through a USB to UART device and bootloader. Alternatively, run `make` in the ThermalCamPi folder, copy `main.bin` into the RPi SD card, and rename it `kernel.img`.

# ThermalCamPi Project
Uses thermal camera to read temperature data from the environment and calculates corresponding pixel data that it puts in a frame buffer, to be displayed through an HDMI cable.
The project also implements a photo gallery wherein you can take a picture and scroll through previously captured pictures.





### References
- [MLX90640 Data Sheet from Melexsis](https://www.melexis.com/-/media/files/documents/datasheets/mlx90640-datasheet-melexis.pdf)
- dsfd
