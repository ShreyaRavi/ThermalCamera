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
 * Initializes for reading data from the thermal camera,
 * and loads pixels into local framebuffer.
 *
 * This function is to be used without wireless integration.
 */
void data_get(void) {
    thermal_init(20.0f, 40.0f, GRADIENT_IRON);
    MLX90640_I2CFreqSet(1000);
    uart_init();
    MLX90640_I2CInit();

    timer_delay_us(80000);

    int status;
    //Get device parameters - We only have to do this once
    uint16_t eeMLX90640[832];
    status = MLX90640_DumpEE(MLX90640_address, eeMLX90640);
    
    if (status != 0) {
        printf("Failed to load system parameters");
    }

    status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
    if (status != 0) {
        printf("Parameter extraction failed, status: %d\n", status);
    }

    MLX90640_SetRefreshRate(MLX90640_address, 0x06); // 32 Hz -- 16 fps (Highest I can go i think) 

    while (1) {
        for (char x = 0; x < 2; x++) { //Read both subpages
            uint16_t mlx90640Frame[834];
            int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
            if (status < 0) {
                printf("GetFrame Error: %d\n", status);
            }

            MLX90640_GetVdd(mlx90640Frame, &mlx90640);

            float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);

            float tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
            float emissivity = 0.95;

            MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
        }

        switch(get_disp_state()) {
            case STREAM:
                display_thermal_img(mlx90640To);
                break;
            case GALLERY:
                disp_pic();
                break;
            case HELP:
                break;
        }
        

        if (!rb_empty(rb_scancode)) {
            char c = keyboard_read_next_ext_custom();
            printf("%c\n", c);

            if (c == 'e' || 
                c == 'l' || 
                c == 'j' || 
                c == ' ' || 
                c == 'h') {
                interpret_user_keys(c);
            }

        }
        
    }
    //Once params are extracted, we can release eeMLX90640 array
}

/*
 * Initializes for reading data from the thermal camera,
 * and transmits pixel data through UART-WiFi bridge (ESP32).
 *
 * This function is to be used only with wireless integration.
 */
void data_transmit(void) {
    thermal_init(20.0f, 40.0f, GRADIENT_IRON);
    MLX90640_I2CFreqSet(1000);
    uart_init();
    MLX90640_I2CInit();

    timer_delay_us(80000);

    int status;

    //Get device parameters - We only have to do this once
    uint16_t eeMLX90640[832];
    
    status = MLX90640_DumpEE(MLX90640_address, eeMLX90640);
    if (status != 0) {
        printf("Failed to load system parameters");
    }

    status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
    if (status != 0) {
        printf("Parameter extraction failed, status: %d\n", status);
    }

    MLX90640_SetRefreshRate(MLX90640_address, 0x02); // 2 Hz -- 1 fps

    while (1) {
        for (char x = 0; x < 2; x++) { //Read both subpages
        
            uint16_t mlx90640Frame[834];

            int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
            if (status < 0) {
                printf("GetFrame Error: %d\n", status);
            }

            MLX90640_GetVdd(mlx90640Frame, &mlx90640);
            float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);
            float tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
            float emissivity = 0.95;

            MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
            transmit_thermal_img(mlx90640To);
        }
    }
    //Once params are extracted, we can release eeMLX90640 array
}