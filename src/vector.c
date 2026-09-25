#include "vector.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////
// 2d functions
//////////////////////////////////////////////////////////////////////////
float vec2_length(vec2_t v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

vec2_t vec2_add(vec2_t a, vec2_t b) {
	vec2_t tmp = {
		.x = a.x + b.x,
		.y = a.y + b.y
	};
	return tmp;
}

vec2_t vec2_sub(vec2_t a, vec2_t b) {
	vec2_t tmp = {
		.x = a.x - b.x,
		.y = a.y - b.y
	};
	return tmp;
}

vec2_t vec2_mul(vec2_t v, float scalar) {
	vec2_t tmp = {
		.x = v.x * scalar,
		.y = v.y * scalar
	};
	return tmp;
}

vec2_t vec2_div(vec2_t v, float scalar) {
	if(scalar != 0.0f) {
		vec2_t tmp = {
			.x = v.x / scalar,
			.y = v.y / scalar
		};
		return tmp;
	} else {
		return v;
	}
}

float vec2_dot(vec2_t a, vec2_t b) {
	return (a.x * b.x) + (a.y * b.y);
}

void vec2_normalize(vec2_t *v) {
	float length = sqrt(v->x * v->x + v->y * v->y);
	v->x /= length;
	v->y /= length;
}

//////////////////////////////////////////////////////////////////////////
// 3d functions
//////////////////////////////////////////////////////////////////////////

vec3_t vec3_add(vec3_t a, vec3_t b) {
	vec3_t tmp = {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};
	return tmp;
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
	vec3_t tmp = {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};
	return tmp;
}

vec3_t vec3_mul(vec3_t v, float scalar) {
	vec3_t tmp = {
		.x = v.x * scalar,
		.y = v.y * scalar,
		.z = v.z * scalar
	};
	return tmp;
}

vec3_t vec3_div(vec3_t v, float scalar) {
	if(scalar != 0.0f) {
		vec3_t tmp = {
			.x = v.x / scalar,
			.y = v.y / scalar,
			.z = v.z / scalar
		};
		return tmp;
	} else {
		return v;
	}
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
	vec3_t tmp = {
		.x = a.y * b.z - a.z * b.y,
		.y = a.z * b.x - a.x * b.z,
		.z = a.x * b.y - a.y * b.x
	};
	return tmp;
}

float vec3_dot(vec3_t a, vec3_t b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float vec3_length(vec3_t v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

void vec3_normalize(vec3_t *v) {
	float length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x /= length;
	v->y /= length;
	v->z /= length;
}

vec3_t vec3_rotate_x(vec3_t v, float angles) {
	vec3_t rotated_vector = {
		.x = v.x,
		.y = v.y * cos(angles) - v.z * sin(angles),
		.z = v.z * cos(angles) + v.y * sin(angles)
	};
	return rotated_vector;
};

vec3_t vec3_rotate_y(vec3_t v, float angles) {
	vec3_t rotated_vector = {
		.x = v.x * cos(angles) - v.z * sin(angles),
		.y = v.y,
		.z = v.z * cos(angles) + v.x * sin(angles)
	};
	return rotated_vector;
};

vec3_t vec3_rotate_z(vec3_t v, float angles){
	vec3_t rotated_vector = {
		.x = v.x * cos(angles) - v.y * sin(angles),
		.y = v.y * cos(angles) + v.x * sin(angles),
		.z = v.z
	};

	return rotated_vector;
};

vec4_t vec4_from_vec3(vec3_t v) {
	vec4_t v4 = {
		.x = v.x,
		.y = v.y,
		.z = v.z,
		.w = 1.0f
	};

	return v4;
}

vec3_t vec3_from_vec4(vec4_t v) {
	vec3_t v3 = {v.x, v.y, v.z};
	return v3;
}