#include <stdint.h>
#include <stdio.h>
#include "display.h"

bool initialize_window(void) {
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	};
	
	//use the SDL to query what is the fullscreen max.width and height
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	window_width = display_mode.w;
	window_height = display_mode.h;

	//CREATE A SDL WINDOW
	window = SDL_CreateWindow(
		NULL, 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		window_width, 
		window_height, 
		SDL_WINDOW_BORDERLESS
	);

	if(!window) {
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	//CREATE A SDL RENDERER
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(!renderer) {
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}
	//to set the SDL from the video mode to the real fullscreen mode
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	return true;
};

// void static swap_points(vec2_t* a, vec2_t* b) {
// 	vec2_t tmp = *a;
// 	*a = *b;
// 	*b = tmp;
// }

void draw_dotted_grid(uint32_t color) {
	for(size_t y = 0; y < window_height; ++y) {
		for(size_t x = 0; x < window_width; ++x) {
			if(x % 10 == 0 && y % 10 == 0) {
				draw_pixel(x, y, color);
			}
		}
	}
}

//renders a background grid that shows a line every row or column of pixels
//that is a multiple of 10
void draw_grid(uint32_t gridline) {
	for(size_t y = 0; y < window_height; ++y) {
		for(size_t x = 0; x < window_width; ++x) {
			if(x % 10 == 0 || y % 10 == 0) {
				color_buffer[(window_width * y) + x] = gridline;
			}
		}
	}
}

void draw_pixel(int x, int y, uint32_t color) {
	if(x >= 0 && x < window_width && y >= 0 && y < window_height) {
		color_buffer[(window_width * y) + x] = color;
	}
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
	//first try to locate the top-left point of the rectangle
	//int startPoint = x + (y * window_width);
	for(int col = y; col < y + height; ++col) {
		for(int row = x; row < x + width; ++row) {
			draw_pixel(row, col, color);
		}
		//startPoint += (window_width - width);
	}
}

//arthur: Alex Campbell-Black
//move the calculation of the limits of the rectangle out of the loop
void draw_rect1(int x, int y, int width, int height, uint32_t color) {
	int j_max = y + height;
	int i_max = x + width;
	for(int j = y; j < j_max; ++j) {
		for(int i = x; i < i_max; ++i) {
			color_buffer[(window_width * j) + i] = color;
		}
	}
}

//arthur: pikuma solution
void draw_rect2(int x, int y, int width, int height, uint32_t color) {
	for(int i = 0; i < width; ++i) {
		for(int j = 0; j < height; ++j) {
			int current_x = x + i;
			int current_y = y + j;
			color_buffer[(window_width) * current_y + current_x] = color;
		}
	}
}

//DDA: Digital Differential Analyzer
void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
	int delta_x = x1 - x0;
	int delta_y = y1 - y0;
	int side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

	//third condition
	if (side_length == 0) {
        draw_pixel(x0, y0, color);
        return;
    }
	// Find how much we should increment in both x and y for each step
	//either x be 1 or y be 1
	float x_inc = delta_x / (float)side_length;
	float y_inc = delta_y / (float)side_length;

	float current_x = x0;
	float current_y = y0;

	for(int i = 0; i <= side_length; ++i) {
		draw_pixel(round(current_x), round(current_y), color);
		current_x += x_inc;
		current_y += y_inc;
	}
}

// Bresenham's algorithm
void draw_line_in_bresenham(int x0, int y0, int x1, int y1, uint32_t color) {
	// octants, decide x is dependent or y is dependent
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	// octants, decide nextX or nextX should be negative or positive
	int stepX = (x1 > x0) ? 1 : -1;
	int stepY = (y1 > y0) ? 1 : -1;

	// horizontal line
	if(dy == 0) {
		for(int i = 0; i < dx + 1; ++i) {
			draw_pixel(x0 + i * stepX, y0, color);
		}
		return;
	}

	// vertical line
	if(dx == 0) {
		for(int i = 0; i < dy + 1; ++i) {
			draw_pixel(x0, y0 + i * stepY, color);
		}
		return;
	}

	// y depends on the changement of x, y -> y+ 1 or y - 1
	if(dx >= dy) {
		int y = y0;
		int p = (2 * dy) - dx;
		for(int i = 0;  i < dx + 1; ++i) {
			draw_pixel(x0 + i * stepX, y, color);
			if(p >= 0) {
				y += stepY;
				p = p - (2 * dx);
			}
			p = p + (2 * dy);
		}
	// x depends on the changement of y, x -> x+1 or x - 1
	} else {
		int x = x0;
		int p = (2 * dx) - dy;
		for(int i = 0; i < dy + 1; ++i) {
			draw_pixel(x, y0 + i * stepY, color);
			if(p >= 0) {
				x += stepX;
				p = p - (2 * dy);
			}
			p = p + (2 * dx);
		}
	}
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	draw_line_in_bresenham(x0, y0, x1, y1, color);
	draw_line_in_bresenham(x1, y1, x2, y2, color);
	draw_line_in_bresenham(x2, y2, x0, y0, color);
}

// void old_faulty_fill_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color) {
// 	//1. Re-order the points
// 	if(p0.y > p1.y) {
// 		swap_points(&p0, &p1);
// 	}
// 	if(p0.y > p2.y) {
// 		swap_points(&p0, &p2);
// 	}
// 	if(p1.y > p2.y) {
// 		swap_points(&p1, &p2);
// 	}
// 	//2. Find the sum of Xs, convert Xs into seperate arrays
// 	int x02 = p0.x + p2.x;
// 	int x12 = p1.x + p2.x;
// 	int x01 = p0.x + p1.x;

// 	int x02_arr[x02];
// 	int x12_arr[x12];
// 	int x01_arr[x01];
// 	int x012_arr[x01 + x12 - 1];

// 	for(int i = 0 ; i < x02; ++i) {
// 		x02_arr[i] = p0.x + i;
// 	}

// 	for(int i = 0; i < x12; ++i) {
// 		x12_arr[i] = p1.x + i;
// 	}

// 	for(int i = 0; i < x01; ++i) {
// 		x01_arr[i] = p0.x + i;
// 	}
// 	//3. Remove the repetitive point of the adjoint two short sides
// 	//remove_last(x01)?
// 	for(int i = 0; i < (x01 + x12); ++i) {
// 		if(i == x01 - 1) {
// 			continue;
// 		}
// 		x012_arr[i] = p0.x + i;
// 	}
// 	//4. Determine which is left and which is right
// 	int* x_left = NULL;
// 	int* x_right = NULL;
// 	int m = floor((sizeof(x012_arr) / sizeof(x012_arr[0])) / 2);
// 	if(x02_arr[m] < x012_arr[m]) {
// 		x_left = x02_arr;
// 		x_right = x012_arr;
// 	} else {
// 		x_left = x012_arr;
// 		x_right = x02_arr;
// 	}

// 	//5. Draw the hotizontal segments
// 	for(int y = p0.y; y <= p2.y; ++y) {
// 		int row = y - p0.y;
// 		draw_line(x_left[row], y, x_right[row], y, color);
// 	}
// }

//render the color buffer array in a texture
//and display it
void render_color_buffer(void) {
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);

	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

//clear(draw) in memory color buffer
void clear_color_buffer(uint32_t color) {
	//column major y -> column, x -> row
	for(size_t y = 0; y < window_height; ++y) {
		for(size_t x = 0; x < window_width; ++x) {
			color_buffer[(window_width * y) + x] = color;
		}
	}
}

void destroy_window(void) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}