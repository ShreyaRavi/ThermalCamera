#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "math.h"
#include "MLX90640_I2C_Driver.h"
#include "MLX90640_API.h"
#include "thermal_lib.h"
#include "gl.h"


const char MLX90640_address = 0x33; //Default 7-bit unshifted address of the MLX90640

#define TA_SHIFT 8 //Default shift for MLX90640 in open air

static int test = 0;
static float mlx90640To[768];
paramsMLX90640 mlx90640;

void test_data_get(void) {
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
        for (char x = 0; x < 2; x++) //Read both subpages
        {
            uint16_t mlx90640Frame[834];
            int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
            if (status < 0)
            {
                printf("GetFrame Error: %d\n", status);
            }

            float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);

            float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);

            float tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
            float emissivity = 0.95;

            MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
            display_thermal_img(mlx90640To);
        }

        // for (int x = 0 ; x < 10 ; x++)
        // {

        //     int temp = (int)(roundf(mlx90640To[x]));
        //     printf("Pixel %d: %03dC\n", x, temp);
            
        // }

    }

    //Once params are extracted, we can release eeMLX90640 array
}

void test_data_transmit(void) {
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
        for (char x = 0; x < 2; x++) //Read both subpages
        {
            uint16_t mlx90640Frame[834];
            int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
            if (status < 0)
            {
                printf("GetFrame Error: %d\n", status);
            }

            float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);

            float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);

            float tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
            float emissivity = 0.95;

            MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
            transmit_thermal_img(mlx90640To);
        }
    }
    //Once params are extracted, we can release eeMLX90640 array
}

void test_send_single_char(void) {

    uart_init();

    timer_delay(7);

    while (1) {
        uart_putchar(test);

        test++;
        if (test > 'z') {
            test = 'a';
        }

        timer_delay_ms(1000);
        uart_flush();
    }
    
}

void test_thermal_color(void) {
    thermal_init(0.0, 80.0, GRADIENT_IRON);

    float temp = 0.0;
    color_t color = 0;

    for (; temp <= 80.0; temp += 5.0) {
        color = get_thermal_color(temp);
        printf("Color: %x\n", color);
    }    
}

void main(void) 
{
    // test_data_get();
    // test_data_transmit();
    // test_thermal_color();
    test_send_single_char();
}

