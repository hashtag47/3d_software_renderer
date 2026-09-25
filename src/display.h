#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include "vector.h"

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern int window_width;
extern int window_height;

extern uint32_t* color_buffer; //ARGB format, point to the first address
extern SDL_Texture* color_buffer_texture; // texture goes to the renderer

//NOTE: could also use the bitmask technique
typedef enum {
	RENDER_WIRE_DOT,
	RENDER_WIRE,
	RENDER_FILLED,
	RENDER_FILLED_WIRE,
} RenderMode;

typedef enum {
  CULL_DISABLE,
  CULL_ENABLE,
} CullMode;

bool initialize_window(void);
void draw_dotted_grid(uint32_t color);
void draw_grid(uint32_t gridline);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_rect1(int x, int y, int width, int height, uint32_t color);
void draw_rect2(int x, int y, int width, int height, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_line_in_bresenham(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
//void old_faulty_fill_triangle(vec2_t p0, vec2_t p1, vec2_t p2, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void destroy_window(void);

#endif