#include "gl.h"
#include "thermal_lib.h"

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

static const int scale_size = 119;
static const float MIN_TEMP = 0;
static const float MAX_TEMP = 80.0;

static float low = 0;
static float high = 0;

color_t get_thermal_color(float temp) {
	// 1. convert temp to normalized scale (integer) from 0 to 119
	// 2. get color associated with normalized index --> return
	int index = normalize(temp);
	return gl_color(iron[index][0], iron[index][1], iron[index][2]);
}

void thermal_init(float set_low, float set_high) {
	set_bounds(set_low, set_high);
}

void set_bounds(float set_low, float set_high) {

	if (set_low > set_high) {
		return;
	}

	set_high = (set_high < MIN_TEMP) ? (MIN_TEMP) : (set_high);
	set_high = (set_high > MAX_TEMP) ? (MAX_TEMP) : (set_high);

	set_low = (set_low < MIN_TEMP) ? (MIN_TEMP) : (set_low);
	set_low = (set_low > MAX_TEMP) ? (MAX_TEMP) : (set_low);

	low = set_low;
	high = set_high;
}

int normalize(float temp) {
	return (int)(roundf((((temp - low) / (high - low)) * (scale_size - 1.0))));
}

void display_thermal_img(float* temp_arr) {
	//get_thermal_color()
}
