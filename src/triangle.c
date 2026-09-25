#include "triangle.h"
#include "display.h"
#include <math.h>

static void swap_orders(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

static void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	// Calculate the slope1 and slope2
	// NOTE:
	// mathematical definition for the slope is rise to run, but here the x is actually the dependant value,
	// so under this context, slope should be the delta x to the delta y, think of DDA
	float slope1 = (float)(x1 - x0) / (y1 - y0);
	float slope2 = (float)(x2 - x0) / (y2 - y0);

	// Loop all scanlines from y0 to y2
	// float x_start = x0;
	// float x_end = x0;
	for(int y = y0; y <= y2; ++y) {
		float x_start = x0 + slope1 * (y - y0);
		float x_end = x0 + slope2 * (y - y0);
		draw_line_in_bresenham((int)roundf(x_start), y, (int)roundf(x_end), y, color);
	}
}

static void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	float slope1 = (float)(x2 - x0) / (y2 - y0);
	float slope2 = (float)(x2 - x1) / (y2 - y1);

	// float x_start = x2;
	// float x_end = x2;
	for(int y = y2; y >= y0; --y) {
		float x_start = x2 - slope1 * (y2 - y);
		float x_end = x2 - slope2 * (y2 - y);
		draw_line_in_bresenham((int)roundf(x_start), y, (int)roundf(x_end), y, color);
	} 
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	if(y0 > y1) {
		swap_orders(&y0, &y1);
		swap_orders(&x0, &x1);
	}
	if(y0 > y2) {
		swap_orders(&y0, &y2);
		swap_orders(&x0, &x2);
	}
	if(y1 > y2) {
		swap_orders(&y1, &y2);
		swap_orders(&x1, &x2);
	}

	if(y0 == y2) return;

	if(y1 == y2) {
		fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
	} else if(y0 == y1) {
		fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
	} else {
		int My = y1;
		int Mx = (int)roundf(x0 + (float)(x2 - x0) * (y1 - y0) / (y2 - y0));

	//draw_line_in_bresenham(x1, y1, Mx, My, 0xFFFFFF00);
	//Draw flat_bottom triangle
	fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);

	//Draw flat_top triangle
	fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
	}
}