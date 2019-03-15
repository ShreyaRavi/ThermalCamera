#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "math.h"
#include "MLX90640_I2C_Driver.h"
#include "MLX90640_API.h"


const char MLX90640_address = 0x33; //Default 7-bit unshifted address of the MLX90640

#define TA_SHIFT 8 //Default shift for MLX90640 in open air

static float mlx90640To[768];
paramsMLX90640 mlx90640;

void main(void) 
{
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
        }

        for (int x = 0 ; x < 10 ; x++)
        {

            int temp = (int)(roundf(mlx90640To[x]));
            printf("Pixel %d: %03dC\n", x, temp);
            
        }

        //timer_delay_us(20);
	}

	//Once params are extracted, we can release eeMLX90640 array

}

