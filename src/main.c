#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
//#include <unistd.h> //for getcwd()
#include "display.h"
#include "triangle.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
//#include "readCSV.h"
#include "sort.h"
#include "matrix.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool is_running = false;
int previous_frame_time = 0;

RenderMode render_mode;
CullMode cull_mode;

/////////////////////////////////////////////////////////////////////////////////////
// Declare an array of vectors/points
/////////////////////////////////////////////////////////////////////////////////////
// const int N_POINTS = 9 * 9 * 9;
// vec3_t cube_points[N_POINTS]; //9*9*9 cube
// //FIX: projected will throw away one dimension, 
//so N_POINTS could be changed to 9 to save the memory space
// vec2_t projected_points[N_POINTS];
/////////////////////////////////////////////////////////////////////////////////////

//triangle_t triangles_to_render[N_MESH_FACES];
triangle_t* triangles_to_render = NULL;

vec3_t camera_position = {.x = 0.f, .y = 0.f, .z = 0.f}; 

float fov_factor = 640.f;
// float fov_factor = 20.f;

void setup(void) {
	// Initialize render mode and cull mode
	render_mode = RENDER_WIRE_DOT;
	cull_mode = CULL_ENABLE;

	//allocate the required memory in bytes to hold the color buffer
	color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);

	if(!color_buffer) {
		fprintf(stderr, "Error allocating color buffer!\n");
		exit(1);
	} else {
		// Creating a SDL texture that is used to display the color buffer
		color_buffer_texture = SDL_CreateTexture(
			renderer, 
			SDL_PIXELFORMAT_ARGB8888, 
			SDL_TEXTUREACCESS_STREAMING, 
			window_width, 
			window_height
		);
		if(!color_buffer_texture) {
			fprintf(stderr, "Error creating the color buffer texture!\n");
			exit(1);
		}   //else {
			/////////////////////////////////////////////////////////////////////////////////////
			// 9*9*9 Points Related Part
			// int point_count = 0;

			// //start loading my array of vectors
			// // NDC: From -1 to 1 (in this 9*9*9 cube)
			// for(float x = -1; x <= 1; x += 0.25) {
			// 	for(float y = -1; y <= 1; y += 0.25) {
			// 		for(float z = -1; z <= 1; z += 0.25) {
			// 			vec3_t new_point = { .x = x, .y = y, .z = z};
			// 			cube_points[point_count++] = new_point;
			// 		}
			// 	}
			// }
			/////////////////////////////////////////////////////////////////////////////////////

			//}
			else {
				// Load the 36 triangles cube
				load_cube_mesh_data();

				// Load Bunny -> .ply failed
				// readAddBunnyVertex("stanford_bunny_dataset_csv/bunny_vertices.csv");
				// readAddBunnyFaces("stanford_bunny_dataset_csv/bunny_faces.csv");

				// Load cat ->.obj
				// load_obj_file_data("assets/Cat_v1/12221_Cat_v1_l3.obj");

				// Load cube or spaceship
				// load_obj_file_data("assets/cubeAndSpaceship/f22.obj"); 
				// load_obj_file_data("assets/cubeAndSpaceship/cube.obj");
			}
	}
}

void process_input(void){
	SDL_Event event;
	SDL_PollEvent(&event);

	switch(event.type) {
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if(event.key.keysym.sym == SDLK_ESCAPE) {
				is_running = false;
			} else if(event.key.keysym.sym == SDLK_1) {
				// Display the wireframe and small red dot for each triangle vertex
				render_mode = RENDER_WIRE_DOT;
			} else if(event.key.keysym.sym == SDLK_2) {
				// Only display the wireframe lines
				render_mode = RENDER_WIRE;
			} else if(event.key.keysym.sym == SDLK_3) {
				// Display filled triangles with a solid color
				render_mode = RENDER_FILLED;
			} else if(event.key.keysym.sym == SDLK_4) {
				// Display both filled triangles and wireframe lines
				render_mode = RENDER_FILLED_WIRE;
			} else if(event.key.keysym.sym == SDLK_c) {
				// Enable the back-face culling
				cull_mode = CULL_ENABLE;
			} else if(event.key.keysym.sym == SDLK_d) {
				// Disable the back-face culling
				cull_mode = CULL_DISABLE;
			}
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Function that receives a 3D vector and returns a projected 2D point
// Left-handness xoordinate system :Project to the x-y plane
///////////////////////////////////////////////////////////////////////////////////////////////
vec2_t project(vec3_t point) {
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z,
		.y = (fov_factor * point.y) / point.z,
	};

	return projected_point;
}

// quickSort, mergeSort handler
int compare_by_avg_depth_desc(const void* a, const void* b) {
	float fa = ((const triangle_t*)a)->avg_depth;
	float fb = ((const triangle_t*)b)->avg_depth;
	if(fa < fb) return -1;
	if(fa > fb) return 1;
	return 0;
}

void update(void){
	//But this while loop might take 100% cpu performance to lock inside, which we don't really want that to happen
	// while(!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME)) {	
	// 	//just lock the execution inside the while loop if frame target time haven't reached
	// }

	//wait some time until the reach the target frame time in milliseconds
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

	//Only delay execution if we are running too fast
	if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}

	//For each frame we will initialize the pointer again
	triangles_to_render = NULL;

	previous_frame_time = SDL_GetTicks();

	// mesh.scale.x += 0.002f;
	// mesh.scale.y += 0.001f;

	// mesh.rotation.x += 0.01f;
	// mesh.rotation.y += 0.02f;
	// mesh.rotation.z += 0.03f;

	//NOTE: Test on arbitrary scale, applied for the cube
	// mesh.irregular_scale_axis.x = 0.45f;
	// mesh.irregular_scale_axis.y = 0.35f;
	// mesh.irregular_scale_axis.z = 0.15f;

	float seconds = SDL_GetTicks() / 1000.f;
	mesh.scale_factor = 1.0f * sinf(seconds * 2.0f);
	float t = seconds * 3.0f;
	mesh.irregular_scale_axis.x = 1.0f + 0.2f * sinf(t);
	mesh.irregular_scale_axis.y = 1.0f + 0.2f * sinf(t + 2.09f);  // +1/3 of a cycle
	mesh.irregular_scale_axis.z = 1.0f + 0.2f * sinf(t + 4.19f);  // +2/3 of a cycle

	//NOTE: Test on arbitrary rotation
	mesh.irregular_rotate_angle  = seconds;

	float tilt = 0.5f;                 // tilt in radians (~29°)
	float precession = seconds * 0.7f; // how fast the axis circles

	mesh.irregular_rotation_axis.x = sinf(tilt) * cosf(precession);
	mesh.irregular_rotation_axis.y = cosf(tilt);
	mesh.irregular_rotation_axis.z = sinf(tilt) * sinf(precession);
	// NOTE: Only for testing z axis rotate direction
	// mesh.irregular_rotation_axis.x = 0.f;
	// mesh.irregular_rotation_axis.y = 0.f;
	// mesh.irregular_rotation_axis.z = 1.0f;

	//mesh.translation.x += 0.01f;
	mesh.translation.z = 5.0f;

	//Create a scale matrix that will be used to multiply the mesh vertices
	// mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t scale_matrix = mat4_make_scale_arbitrary(mesh.irregular_scale_axis, mesh.scale_factor);
	
	//Create a rotation matrix 
	// mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
	// mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
	// mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);
	mat4_t rotation_matrix =  mat4_make_rotation_arbitrary(mesh.irregular_rotation_axis, mesh.irregular_rotate_angle);

	//Create a translation matrix that will be used by translation
	mat4_t translation_matrix = mat4_make_translate(mesh.translation.x, mesh.translation.y, mesh.translation.z);
	
	/////////////////////////////////////////////////////////////////////////////////////
	// 9*9*9 Points Related Part
	// for(int i = 0; i < N_POINTS; ++i ) {
	// 	vec3_t point = cube_points[i];

	// 	vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
	// 	transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
	// 	transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);

	// 	//Translate the point away from the camera 
	// 	transformed_point.z -= camera_position.z;

	// 	//project the current point
	// 	vec2_t projected_point = project(transformed_point);

	// 	//save the projected 2d vector in the array of projected points
	// 	projected_points[i] = projected_point;
	// }
	/////////////////////////////////////////////////////////////////////////////////////
	//Loop all triangle faces of our mesh
	int num_faces = array_length(mesh.faces);

	for(int i = 0; i < num_faces; ++ i) {
		//how many faces we gonna have, then for each face we will set three vertices
		//start off the first one, EBO in openGL
		face_t mesh_face_index = mesh.faces[i];

		//the EBO to the corresponding VBO
		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face_index.a - 1];
		face_vertices[1] = mesh.vertices[mesh_face_index.b - 1];
		face_vertices[2] = mesh.vertices[mesh_face_index.c - 1];

		vec4_t transformed_vertices[3];

		//Loop all three vertices of this current face and apply transformations
		for(int j = 0; j < 3; ++j) {
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

			//pipeline here: take openGL as reference
			// local space -> world space -> view space -> clip space -> screen space

			//////////////////////////////////////////////////////////////////////////
			// NOTE: Replace pure vertex rotation with the scale matrix
			//////////////////////////////////////////////////////////////////////////
			// rotation
			// transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
			// transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
			// transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);	
			//////////////////////////////////////////////////////////////////////////

			//multiply the scale_matrix by the vertex (scale without rotation)
			transformed_vertex = mat4_multiply_vec4(scale_matrix, transformed_vertex);

			//Rotate the vertex
			// transformed_vertex = mat4_multiply_vec4(rotation_matrix_x, transformed_vertex);
			// transformed_vertex = mat4_multiply_vec4(rotation_matrix_y, transformed_vertex);
			// transformed_vertex = mat4_multiply_vec4(rotation_matrix_z, transformed_vertex);
			transformed_vertex = mat4_multiply_vec4(rotation_matrix, transformed_vertex);

			//Translate the vertex away from the camera
			//transformed_vertex.z += 5.0f;
			transformed_vertex = mat4_multiply_vec4(translation_matrix, transformed_vertex);

			//save transformed vertex in the array of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}	

		if(cull_mode == CULL_ENABLE) {
				//after transformation, we want to test if current face should activate back_face culling
				//because we are following the clockwise order
				vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
				vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
				vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);

				//Get the vector subtraction, because we don't want to consider the magnitude of these
				// two vectors, so we could just normalize them
				vec3_t vector_ab = vec3_sub(vector_b, vector_a);
				vec3_t vector_ac = vec3_sub(vector_c, vector_a);
				vec3_normalize(&vector_ab);
				vec3_normalize(&vector_ac);

				//take the cross product to get perpendicular normal
				//clockwise order
				//Normalize the face normal vector
				vec3_t vector_normal = vec3_cross(vector_ab, vector_ac);
				vec3_normalize(&vector_normal);

				//get the camera ray --> normal to camera
				vec3_t camera_ray = vec3_sub(camera_position, vector_a);

				//take the dot product between N and camera ray, if dot product is less than 0, continue
				// greater than > 0 same direction, = 0 perpendicular
				if(vec3_dot(vector_normal, camera_ray) < 0) {
					continue;
				}
		}

		//triangle_t have three 2d points as one point collection
		//triangle_t projected_triangle;

		vec2_t projected_points[3];
		
		//loop all three vertices to perform projection
		//here the transformed_vertices is the fixed_array, not created by the dynamic array system
		//so do not use array_length() to get the length of the fixed array
		for(int j = 0; j < 3; ++j) {
			//projection(view space)
			//Project the current vertex
			projected_points[j] = project(vec3_from_vec4(transformed_vertices[j]));

			//Scale and translate the projected points to the middle of the screen
			projected_points[j].x += (window_width / 2);
			projected_points[j].y += (window_height/ 2);
		}

		// Calculate the average depth for each face based on the vertices z-value after transformation
		float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3;

		triangle_t projected_traingle = {
			.points = {	
					{projected_points[0].x, projected_points[0].y },
					{projected_points[1].x, projected_points[1].y},
					{projected_points[2].x, projected_points[2].y}
			},
			.color = mesh_face_index.color,
			.avg_depth = avg_depth,
		};

		//Save the projected triangle in the array of triangles to render
		//triangles_to_render[i] = projected_triangle;
		array_push(triangles_to_render, projected_traingle);
	}

	// Sort the triangles to render by their avg_depth
	int total_triangles = array_length(triangles_to_render);
	// +z towards the screen and -z towards the user
	// quickSort(0, total_triangles - 1, triangles_to_render, sizeof(triangle_t), compare_by_avg_depth_desc);
	mergeSort(0, total_triangles - 1, triangles_to_render, sizeof(triangle_t), compare_by_avg_depth_desc);
}

void render(void) {
	// SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	// SDL_RenderClear(renderer);

	clear_color_buffer(0xFF000000);

	//draw_grid(0xFFFFFFFF);
	//draw_dotted_grid(0xFFFFFFFF);

	/////////////////////////////////////////////////////////////////////////////////////
	// 9*9*9 Points Related Part
	//Loop all projected points and render them
	// for(int i = 0; i < N_POINTS; ++i) {
	// 	vec2_t temp = projected_points[i];
	// 	//single point might be invisible so we draw the rectangle to scale it
	// 	draw_rect(temp.x + (int)(window_width/2), temp.y + (int)(window_height/2), 4, 4, 0xFFFFFF00);
	// }

	// draw_rect(window_width/2, window_height/2, 200, 150, 0xFFFFB6C1);

	// draw_pixel(window_width/2, window_height/2, 0xFFFF0000);
	/////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// Temporarily comment out
	////////////////////////////////////////////////////////////////////////////////////
	//Loop all projected traingles and render them
	int num_triangles = array_length(triangles_to_render);

	for(int i = 0; i < num_triangles; ++i) {
		triangle_t triangle = triangles_to_render[i];
		// draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
		// draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
		// draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);
		
		//we may want to uncouple each funtion and give it a good context, so replace draw_line to draw_triangle
		// draw_line(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, 0xFFFFFFFF);
		// draw_line(triangle.points[1].x, triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, 0xFFFFFFFF);
		// draw_line(triangle.points[0].x, triangle.points[0].y, triangle.points[2].x, triangle.points[2].y, 0xFFFFFFFF);

		switch(render_mode) {
			case RENDER_WIRE_DOT:
						draw_triangle(
								triangle.points[0].x, triangle.points[0].y, 
								triangle.points[1].x, triangle.points[1].y, 
								triangle.points[2].x, triangle.points[2].y,
								0xFF00FF00
								);
						draw_rect(triangle.points[0].x - 2, triangle.points[0].y - 2, 4, 4, 0xFFFF0000);
						draw_rect(triangle.points[1].x - 2, triangle.points[1].y - 2, 4, 4, 0xFFFF0000);
						draw_rect(triangle.points[2].x - 2, triangle.points[2].y - 2, 4, 4, 0xFFFF0000);
						break;
			case RENDER_WIRE :
						draw_triangle(
							triangle.points[0].x, triangle.points[0].y, 
							triangle.points[1].x, triangle.points[1].y, 
							triangle.points[2].x, triangle.points[2].y,
							0xFFFFFFFF
						);
						break;
			case RENDER_FILLED:
						draw_filled_triangle(
							triangle.points[0].x, triangle.points[0].y, 
							triangle.points[1].x, triangle.points[1].y, 
							triangle.points[2].x, triangle.points[2].y,
							triangle.color
						);
						break;
			case RENDER_FILLED_WIRE:
						draw_filled_triangle(
							triangle.points[0].x, triangle.points[0].y, 
							triangle.points[1].x, triangle.points[1].y, 
							triangle.points[2].x, triangle.points[2].y,
							triangle.color
						);
						draw_triangle(
							triangle.points[0].x, triangle.points[0].y, 
							triangle.points[1].x, triangle.points[1].y, 
							triangle.points[2].x, triangle.points[2].y,
							0xFF000000
						);
						break;
		}
	}

	//Clear the array of triangles to render every frame loop
	array_free(triangles_to_render);
	/////////////////////////////////////////////////////////////////////////////////////

	render_color_buffer();

	SDL_RenderPresent(renderer);
}

void free_resources(void) {
	free(color_buffer);
	array_free(mesh.vertices);
	array_free(mesh.faces);
}

int main(void) {

	is_running = initialize_window();

	setup();

	//processor clock, each machine will have different animation speed
	//so we make some alteration in update function
	while(is_running) {
		process_input();
		update();
		render();		
	}

	destroy_window();
	free_resources();

	return 0;
}