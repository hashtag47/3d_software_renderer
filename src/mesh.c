#include "mesh.h"
#include "array.h"
#include "stdio.h"
#include "vector.h"
#include <stdio.h>
#include <string.h> //for strncmp()

mesh_t mesh = {
	.vertices = NULL,
	.faces = NULL,
	.rotation = {0.f, 0.f, 0.f},
	.irregular_rotation_axis = {1.0f, 0.f, 0.f},
	.irregular_rotate_angle = 0.0f,
	.scale = {1.f, 1.f, 1.f},
	.irregular_scale_axis = {1.0f, 0.0f, 0.0f},
	.scale_factor = 1.f,
	.translation = {0.f, 0.f, 0.f}
};

//think of VBO in openGL, the vertex data stored in rank3 vectors
vec3_t cube_vertices[N_CUBE_VERTICES] = {
	{.x = -1, .y = -1, .z = -1}, //1 -- index: 0
	{.x = -1, .y = 1, .z = -1}, //2 -- index: 1
	{.x = 1, .y = 1, .z = -1}, //3 -- index: 2
	{.x =  1, .y = -1, .z = -1}, //4 -- index: 3
	{.x = 1, .y = 1, .z = 1}, //5 -- index: 4
	{.x = 1, .y = -1, .z = 1}, //6 -- index: 5
	{.x = -1, .y= 1, .z = 1}, //7 -- index: 6
	{.x = -1, .y = -1, .z = 1}  //8 -- index: 7
};

//think of EBO in openGL
face_t cube_faces[N_CUBE_FACES]= {
	//front
	{.a = 1, .b = 2, .c = 3, .color = 0xFFFF0000}, //red
	{.a = 1, .b = 3, .c = 4, .color = 0xFFFF0000}, //red
	//right
	{.a = 4, .b= 3, .c = 5, .color = 0xFF00FF00}, //green
	{.a = 4, .b = 5, .c = 6, .color = 0xFF00FF00}, //green
	//back
	{ .a = 6, .b = 5, .c = 7, .color = 0xFF0000FF}, //blue
	{ .a = 6, .b = 7, .c = 8, .color = 0xFF0000FF}, //blue
	//left
	{ .a = 8, .b = 7, .c = 2, .color = 0xFFFFFF00}, //red + green = yellow
	{ .a = 8, .b = 2, .c = 1, .color = 0xFFFFFF00}, //red + green = yellow
	//top
	{.a = 2, .b = 7, .c = 5, .color = 0xFFFF00FF}, //red + blue = magenta
	{.a = 2, .b = 5, .c = 3, .color = 0xFFFF00FF}, //red + blue = magenta
	//bottom
	{ .a = 6, .b = 8, .c = 1, .color = 0xFF00FFFF}, //green + blue = cyan
	{.a = 6, .b = 1, .c = 4, .color = 0xFF00FFFF} // green + blue = cyan
};

void load_cube_mesh_data(void) {
	for(int i = 0; i < N_CUBE_VERTICES; ++i) {
		vec3_t cube_vertex = cube_vertices[i];
		array_push(mesh.vertices, cube_vertex);
	}

	for(int i = 0; i< N_CUBE_FACES; ++i) {
		face_t cube_face = cube_faces[i];
		array_push(mesh.faces, cube_face);
	}
}

void load_obj_file_data(char* filename) {
	//Read the contents of the .obj file --> cat file
	FILE* fp = fopen(filename, "r");
	if(fp == NULL) {
		printf("Cannot open the file: %s", filename);
		return;
	} else {
		//Read the vertex and faces in -> v / f
		mesh.vertices = NULL;
		mesh.faces = NULL;

		char line_max_counts[100];
	
		//NULL returns from fgets, represent that line is empty, no data inside
        //File reading will stop when we strike the end of the file
        while(fgets(line_max_counts, sizeof(line_max_counts), fp)) {
        	// vertex starts at v
        	//Use mesh.vertices and mesh.face

        	///////////////////////////////////////////////////////////////////////////////////////////////////
        	// My way of comparing two letters
        	///////////////////////////////////////////////////////////////////////////////////////////////////
        	// if(line_max_counts[0] == 'v' && line_max_counts[1] == ' ') {
        	// 	vec3_t temp_vertex;
        	// 	if(sscanf(line_max_counts, "v %f %f %f", &temp_vertex.x, &temp_vertex.y, &temp_vertex.z) == 3) {
        	// 		array_push(mesh.vertices, temp_vertex);
        	// 	}
        	// } else if(line_max_counts[0] == 'f' && line_max_counts[1] == ' ') {
        	// 	face_t temp_face;
        	// 	if(sscanf(line_max_counts, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &temp_face.a, &temp_face.b, &temp_face.c) == 3) {
        	// 		array_push(mesh.faces, temp_face);
        	// 	}
        	// }        	
        	if(strncmp(line_max_counts, "v ", 2) == 0) {
        		vec3_t tmp_v;
        		sscanf(line_max_counts, "v %f %f %f", &tmp_v.x, &tmp_v.y, &tmp_v.z);
        		array_push(mesh.vertices, tmp_v);
        	}

        	if(strncmp(line_max_counts, "f ", 2) == 0) {
        		// Record it even though we won't use it right now
        		int vertex_indices[3];
        		int texture_indices[3];
        		int normal_indices[3];
        		sscanf(line_max_counts, "f %d/%d/%d %d/%d/%d %d/%d/%d",
        			&vertex_indices[0], &texture_indices[0], &normal_indices[0],
        			&vertex_indices[1], &texture_indices[1], &normal_indices[1],
        			&vertex_indices[2], &texture_indices[2], &normal_indices[2]
        		);
        		face_t tmp_f = {
        			.a = vertex_indices[0],
        			.b = vertex_indices[1],
        			.c = vertex_indices[2]
        		};
        		array_push(mesh.faces, tmp_f);
        	}	
        }
	}
	fclose(fp);
	printf("Loaded: %d vertex, %d faces.", array_length(mesh.vertices), array_length(mesh.faces));
}
