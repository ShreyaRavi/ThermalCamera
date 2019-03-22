#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "math.h"
#include "save.h"
#include "MLX90640_I2C_Driver.h"
#include "MLX90640_API.h"
#include "thermal_lib.h"
#include "gl.h"
#include "keyboard_ext.h"

const char MLX90640_address = 0x33; //Default 7-bit unshifted address of the MLX90640

#define TA_SHIFT 8 //Default shift for MLX90640 in open air

static float mlx90640To[768]; //Array of pixels
paramsMLX90640 mlx90640; //Data structure containing pixel data parameters


/*
 * Completes all initialization, including thermal_lib, I2C, and UART libraries
 * Loads thermal camera parameter data and stores them.
 * Speeds up FPS of stream with camera and I2C lib
 */
void setup_data_read(float low_temp, float high_temp) {

    thermal_init(low_temp, high_temp, GRADIENT_IRON);
    MLX90640_I2CFreqSet(1000);
    uart_init();
    MLX90640_I2CInit();

    // add delay to allow initialization sequence to finish
    // otherwise, first frame gives garbage
    timer_delay_us(80000);

    int status;

    // start retrieving device parameters
    uint16_t eeMLX90640[832];
    status = MLX90640_DumpEE(MLX90640_address, eeMLX90640);
    
    if (status != 0) {
        printf("Failed to load system parameters");
    }

    status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
    if (status != 0) {
        printf("Parameter extraction failed, status: %d\n", status);
    }

    // set refresh rate of camera to 32 Hz per subpage
    // 2 subpages per frame --> 16 frames per second
    // Experimentally highest (theoretical highest = 64 Hz)
    MLX90640_SetRefreshRate(MLX90640_address, 0x06);
}

/*
 * Reads both subframes from the thermal camera and integrates the data to give
 * temperature readings for each pixel in the 32x24 array (stored globally)
 *
 */ 
void read_single_frame(void) {
    for (char x = 0; x < 2; x++) { //Read both subpages
        uint16_t mlx90640Frame[834];
        int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
        if (status < 0) {
            printf("GetFrame Error: %d\n", status);
        }

        float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);

        float tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
        float emissivity = 0.95;

        MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
    }
}

/*
 * Uses the display state determined by the save state machine
 * to choose which image to display
 *
 */
void display_state_image() {
    switch(get_disp_state()) {
        case STREAM:
            display_thermal_img(mlx90640To);
            break;
        case GALLERY:
            disp_pic();
            break;
    }
}

/*
 * Reads an entire frame from the thermal camera and displays
 * whatever image the user requests according to state machine
 *
 */
void get_and_display_image(void) {
    read_single_frame();
    display_state_image();
}


/*
 * Initializes for reading data from the thermal camera,
 * and transmits pixel data through UART-WiFi bridge (ESP32).
 *
 * This function is to be used only with wireless integration.
 */
void data_transmit(void) {
    
    setup_data_read(20.0f, 40.0f);

    while (1) {
        read_single_frame();
        transmit_thermal_img(mlx90640To);
    }
}