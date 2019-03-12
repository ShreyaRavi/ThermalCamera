#include "printf.h"
#include "uart.h"
#include "MLX90640_I2C_Driver.h"
#include "MLX90640_API.h"


const char MLX90640_address = 0x33; //Default 7-bit unshifted address of the MLX90640

#define TA_SHIFT 8 //Default shift for MLX90640 in open air

static float mlx90640To[768];
paramsMLX90640 mlx90640;

void main(void) 
{
	MLX90640_I2CInit(); // problem --  undefined reference to `MLX90640_I2CInit'

}

