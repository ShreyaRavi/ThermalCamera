#include "gl.h"
#include "thermal_lib.h"
#include "uart.h"
#include "timer.h"

/*
 * Data structure mapping pixel data to colors.
 * This color gradient is called "iron," and goes from
 * blue to purple to red to yellow; it is the default
 * gradient for this thermal camera.
 *
 * This lookup table is stored in memory for increased 
 * computational efficiency, helping decrease image lag.
 */

unsigned char iron[119][3] = {
{0, 0, 0},
{0, 0, 36},
{0, 0, 51},
{0, 0, 66},
{0, 0, 81},
{2, 0, 90},
{4, 0, 99},
{7, 0, 106},
{11, 0, 115},
{14, 0, 119},
{20, 0, 123},
{27, 0, 128},
{33, 0, 133},
{41, 0, 137},
{48, 0, 140},
{55, 0, 143},
{61, 0, 146},
{66, 0, 149},
{72, 0, 150},
{78, 0, 151},
{84, 0, 152},
{91, 0, 153},
{97, 0, 155},
{104, 0, 155},
{110, 0, 156},
{115, 0, 157},
{122, 0, 157},
{128, 0, 157},
{134, 0, 157},
{139, 0, 157},
{146, 0, 156},
{152, 0, 155},
{157, 0, 155},
{162, 0, 155},
{167, 0, 154},
{171, 0, 153},
{175, 1, 152},
{178, 1, 151},
{182, 2, 149},
{185, 4, 149},
{188, 5, 147},
{191, 6, 146},
{193, 8, 144},
{195, 11, 142},
{198, 13, 139},
{201, 17, 135},
{203, 20, 132},
{206, 23, 127},
{208, 26, 121},
{210, 29, 116},
{212, 33, 111},
{214, 37, 103},
{217, 41, 97},
{219, 46, 89},
{221, 49, 78},
{223, 53, 66},
{224, 56, 54},
{226, 60, 42},
{228, 64, 30},
{229, 68, 25},
{231, 72, 20},
{232, 76, 16},
{234, 78, 12},
{235, 82, 10},
{236, 86, 8},
{237, 90, 7},
{238, 93, 5},
{239, 96, 4},
{240, 100, 3},
{241, 103, 3},
{241, 106, 2},
{242, 109, 1},
{243, 113, 1},
{244, 116, 0},
{244, 120, 0},
{245, 125, 0},
{246, 129, 0},
{247, 133, 0},
{248, 136, 0},
{248, 139, 0},
{249, 142, 0},
{249, 145, 0},
{250, 149, 0},
{251, 154, 0},
{252, 159, 0},
{253, 163, 0},
{253, 168, 0},
{253, 172, 0},
{254, 176, 0},
{254, 179, 0},
{254, 184, 0},
{254, 187, 0},
{254, 191, 0},
{254, 195, 0},
{254, 199, 0},
{254, 202, 1},
{254, 205, 2},
{254, 208, 5},
{254, 212, 9},
{254, 216, 12},
{255, 219, 15},
{255, 221, 23},
{255, 224, 32},
{255, 227, 39},
{255, 229, 50},
{255, 232, 63},
{255, 235, 75},
{255, 238, 88},
{255, 239, 102},
{255, 241, 116},
{255, 242, 134},
{255, 244, 149},
{255, 245, 164},
{255, 247, 179},
{255, 248, 192},
{255, 249, 203},
{255, 251, 216},
{255, 253, 228},
{255, 254, 239}
};

int normalize(float temp, int scale_size);
color_t get_thermal_color_rainbow(int normalized_temp);

// Maximum and minimum temperatures allowed by hardware
// Measured in celsius
static const float MIN_TEMP = 0;
static const float MAX_TEMP = 80.0;

// User-determined ranges for maximum and minimum temperatures to read
static float low = 0;
static float high = 0;

// Default gradient used
static unsigned char gradient = GRADIENT_IRON;

// Hardware-determined dimensions of the frame buffer
static const int sensor_disp_width = 32;
static const int sensor_disp_height = 24;

/*
 * Retrieves the pixel data for a particular temperature.
 * Normalizes the temperature on a range of possible colors depending 
 * on the palette size, and returns the mapped color at that normalized index.
 *
 * Two palettes are supported: iron (blue to red to yellow), and rainbow.
 *
 * @param temp The temperature to map to a color
 * @return The color of the temperature on the indicated range and gradient
 */
color_t get_thermal_color(float temp) {
	switch (gradient) {
		case GRADIENT_IRON:
			;
			int palette_size = 119;
			int index = normalize(temp, palette_size);
			return gl_color(iron[index][0], iron[index][1], iron[index][2]);
		case GRADIENT_RAINBOW:
			;
			palette_size = 1021;
			index = normalize(temp, palette_size);
			return get_thermal_color_rainbow(index);
	}
	return -1;
}

/*
 * Calculates the color of an indicated temperature on the rainbow gradient.
 * Function is called from the get_thermal_color() function.
 *
 * @param normalized_temp The normalized temperature to map to a color
 * @return The color of the temperature on the indicated range and rainbow gradient
 */
color_t get_thermal_color_rainbow(int normalized_temp) {
	int base = 0;
	while (normalized_temp > 255) {
		base++;
		normalized_temp -= 255;
	}

	switch (base) {
		case 0:
			return gl_color(0, normalized_temp, 255);
		case 1:
			return gl_color(0, 255, 255 - normalized_temp);
		case 2:
			return gl_color(normalized_temp, 255, 0);
		case 3:
			return gl_color(255, 255 - normalized_temp, 0);
	}
	return -1;
}

/*
 * Initializer for thermal library.
 *
 * @param set_low Low bound for temperature reading
 * @param set_high Upper bound for temperature reading
 * @param gradient Choice of gradient to use for image
 */
void thermal_init(float set_low, float set_high, unsigned char gradient) {
	set_gradient(gradient);
	set_bounds(set_low, set_high);
	gl_init(sensor_disp_width, sensor_disp_height, GL_DOUBLEBUFFER);
}

/*
 * Sets upper and lower bounds for temperature reading. 
 * Accounts for out-of-range values.
 * If the indicated set_low value is greater than set_high, no values are set.
 *
 * @param set_low Low bound for temperature reading
 * @param set_high Upper bound for temperature reading
 */
void set_bounds(float set_low, float set_high) {
	if (set_low > set_high) {
		return;
	}

	// Bounds checking
	set_high = (set_high < MIN_TEMP) ? (MIN_TEMP) : (set_high);
	set_high = (set_high > MAX_TEMP) ? (MAX_TEMP) : (set_high);

	set_low = (set_low < MIN_TEMP) ? (MIN_TEMP) : (set_low);
	set_low = (set_low > MAX_TEMP) ? (MAX_TEMP) : (set_low);

	low = set_low;
	high = set_high;
}

/*
 * Normalizes temperature data between a low and a high value,
 * on an indicated scale size value.
 *
 * @param temp Temperature to normalize
 * @param scale_size Size of scale (gradient) to normalize to
 */
int normalize(float temp, int scale_size) {
	if (temp < low) {
		temp = low;
	} else if (temp > high) {
		temp = high;
	}
	return (int)(roundf((((temp - low) / (high - low)) * (scale_size - 1.0))));
}

/*
 * Sets the gradient to be used for pixel mapping.
 *
 * @param new_gradient Gradient to set
 */
void set_gradient(unsigned char new_gradient) {
	gradient = new_gradient;
}

/*
 * Displays a thermal image to the local frame buffer.
 * To be used without wireless integration.
 *
 * @param temp_arr Pixel data to put in frame buffer
 */
void display_thermal_img(float* temp_arr) {
	for (int row = 0; row < sensor_disp_width; row++) { // row -- y value
		for (int col = 0; col < sensor_disp_height; col++) { // col -- x value
			float temp = temp_arr[(row * sensor_disp_width) + col];
			color_t color = get_thermal_color(temp);
			gl_draw_pixel(col, row, color);
		}
	}

	gl_swap_buffer();	
}

/*
 * Transmits a thermal image wirelessly through UART-WiFi bridge (ESP32).
 * To be used with wireless integration.
 * Transmits color value by color value (3 values per RGB pixel)
 *
 * @param temp_arr Pixel data to put in frame buffer
 */
void transmit_thermal_img(float* temp_arr) {
	for (int row = 0; row < sensor_disp_width; row++) { // row -- y value
		for (int col = 0; col < sensor_disp_height; col++) { // col -- x value
			float temp = temp_arr[(row * sensor_disp_width) + col];
			color_t color = get_thermal_color(temp);

			// Blue pixel value
			uart_putchar(color & 0xFF);
			// timer_delay_us(20);

			// Green pixel value
			uart_putchar((color >> 8) & 0xFF);
			// timer_delay_us(20);

			// Red pixel value
			uart_putchar((color >> 16) & 0xFF);
			// timer_delay_us(20);
		}
	}
}

