#ifndef THERMAL_LIB_H
#define THERMAL_LIB_H

#include "math.h"

unsigned char get_thermal_color(float temp);

void display_thermal_img(float* temp_arr);

#endif