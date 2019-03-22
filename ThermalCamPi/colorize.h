#ifndef COLORIZE_H
#define COLORIZE_H


/*
 * Reads an entire frame from the thermal camera and displays
 * whatever image the user requests according to state machine
 *
 * This function is to be used without wireless integration.
 */
void get_and_display_image(void);

/*
 * Completes all initialization, including thermal_lib, I2C, and UART libraries
 * Loads thermal camera parameter data and stores them.
 * Speeds up FPS of stream with camera and I2C lib
 */
void setup_data_read(float low_temp, float high_temp);

/*
 * Initializes for reading data from the thermal camera,
 * and transmits pixel data through UART-WiFi bridge (ESP32).
 *
 * This function is to be used only with wireless integration.
 */
void data_transmit(void);

#endif