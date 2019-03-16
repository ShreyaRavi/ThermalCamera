#ifndef THERMAL_LIB_H
#define THERMAL_LIB_H

#include "math.h"
#include "gl.h"

color_t get_thermal_color(float temp);
void thermal_init(float set_low, float set_high);
void set_bounds(float set_low, float set_high);
int normalize(float temp);
void display_thermal_img(float* temp_arr);

#endif