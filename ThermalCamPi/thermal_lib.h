#ifndef THERMAL_LIB_H
#define THERMAL_LIB_H

#include "math.h"
#include "gl.h"

color_t get_thermal_color(float temp);
void thermal_init(float set_low, float set_high, unsigned char gradient);
void set_bounds(float set_low, float set_high);

void transmit_thermal_img(float* temp_arr);
void display_thermal_img(float* temp_arr);
void set_gradient(unsigned char gradient);

enum color_gradient {
	GRADIENT_IRON,
	GRADIENT_RAINBOW
};

#endif