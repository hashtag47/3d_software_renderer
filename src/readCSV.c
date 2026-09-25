#include "array.h"
#include "mesh.h"
#include <stdio.h>
#include "readCSV.h"

//Add stanford bunny vertex to global mesh structs -> CSV: Comma Separated Values
void readAddBunnyVertex(char* filename) {
    FILE* fp = fopen(filename, "r");
    //File reading failed
    if(fp == NULL) {
        printf("Could not open the file: %s", filename);
        return;
    } else {
        mesh.vertices = NULL;
        char line_max_counts[100];
        fgets(line_max_counts, sizeof(line_max_counts), fp);
        //If NULL returns from fgets, represent that line is empty, no data inside
        //File reading will stop when we strike the end of the file
        while(fgets(line_max_counts, sizeof(line_max_counts), fp)) {
            vec3_t temp_vertex;
            if(sscanf(line_max_counts, "%f, %f, %f", &temp_vertex.x, &temp_vertex.y, &temp_vertex.z) == 3) {
                array_push(mesh.vertices, temp_vertex);
            }
        }

        fclose(fp);
        int vertex_length_check = array_length(mesh.vertices);
        printf("Read %d vertices.\n", vertex_length_check);
        //Allocated memory will be released in main.c file
    }
}

// Add stanford bunny faces to global mesh structs -> CSV: Comma Separated Values
void readAddBunnyFaces(char* filename) {
    FILE* fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("Could not open the file: %s", filename);
        return;
    } else {
        mesh.faces = NULL;
        char line_max_counts[100];
        fgets(line_max_counts, sizeof(line_max_counts), fp);
        while(fgets(line_max_counts, sizeof(line_max_counts), fp)) {
            face_t temp_face;
            if(sscanf(line_max_counts, "%d, %d, %d", &temp_face.a, &temp_face.b, &temp_face.c) == 3) {
                array_push(mesh.faces, temp_face);
            }
        }

        fclose(fp);
        int faces_length_check = array_length(mesh.faces);
        printf("Read %d faces.\n", faces_length_check);
        //Allocated memory will be released in main.c file
    }

}