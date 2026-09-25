#include "matrix.h"
#include "math.h"
#include "vector.h"

//NOTE: Use v * column_vector format, so be aware of this, and remember 
// rotate x made y towards z, rotate y made z towards x, rotate z made x towards y
//  x -> y -> z -> x
// and this matrix used left-handed coordinate
mat4_t mat4_identity(void) {
  mat4_t m = {{
    { 1, 0, 0, 0},
    { 0, 1, 0, 0},
    { 0, 0, 1, 0},
    { 0, 0, 0, 1}
  }};

  return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
  mat4_t m = mat4_identity();
  m.m[0][0] = sx;
  m.m[1][1] = sy;
  m.m[2][2] = sz;

  return m;
}

mat4_t mat4_make_scale_arbitrary(vec3_t v, float scale) {
  mat4_t m = mat4_identity();
  vec3_normalize(&v);
  vec4_t normal = vec4_from_vec3(v);
  // Change row vector to column vector
  // m.m[0][0] = 1 + ((scale - 1) * normal.x * normal.x);
  // m.m[0][1] = (scale - 1) * normal.x * normal.y;
  // m.m[0][2] = (scale - 1) * normal.x * normal.z;
  // m.m[1][0] = (scale - 1) * normal.x * normal.y;
  // m.m[1][1] = 1 + ((scale - 1) * normal.y * normal.y);
  // m.m[1][2] = (scale - 1) * normal.y * normal.z;
  // m.m[2][0] = (scale - 1) * normal.x * normal.z;
  // m.m[2][1] = (scale - 1) * normal.y * normal.z;
  // m.m[2][2] = 1 + ((scale - 1) * normal.z * normal.z);
  
  m.m[0][0] = 1 + ((scale - 1) * normal.x * normal.x);
  m.m[1][0] = (scale - 1) * normal.x * normal.y;
  m.m[2][0] = (scale - 1) * normal.x * normal.z;
  m.m[0][1] = (scale - 1) * normal.x * normal.y;
  m.m[1][1] = 1 + ((scale - 1) * normal.y * normal.y);
  m.m[2][1] = (scale - 1) * normal.y * normal.z;
  m.m[0][2] = (scale - 1) * normal.x * normal.z;
  m.m[1][2] = (scale - 1) * normal.y * normal.z;
  m.m[2][2] = 1 + ((scale - 1) * normal.z * normal.z);
  return m;
}

vec4_t mat4_multiply_vec4(mat4_t m, vec4_t v) {
  vec4_t result;
  // sx 0 0 0   x
  // 0 sy 0 0 * y
  // 0 0 sz 0   z
  // 0 0  0 1   w(1)
  result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
  result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
  result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
  result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

  return result;
}

mat4_t mat4_make_translate(float tx, float ty, float tz) {
  mat4_t m = mat4_identity();
  m.m[0][3] = tx;
  m.m[1][3] = ty;
  m.m[2][3] = tz;

  return m;
}

mat4_t mat4_make_rotation_x(float angle) {
  mat4_t m = mat4_identity();
  float c = cos(angle);
  float s = sin(angle);
  m.m[1][1] = c;
  m.m[1][2] = -s;
  m.m[2][1] = s;
  m.m[2][2] = c;
  return m;
}

mat4_t mat4_make_rotation_y(float angle) {
  mat4_t m = mat4_identity();
  float c = cos(angle);
  float s = sin(angle);
  m.m[0][0] = c;
  m.m[0][2] = s;
  m.m[2][0] = -s;
  m.m[2][2] = c;
  return m;
}

mat4_t mat4_make_rotation_z(float angle) {
  mat4_t m = mat4_identity();
  float c = cos(angle);
  float s = sin(angle);
  m.m[0][0] = c;
  m.m[0][1] = -s;
  m.m[1][0] = s;
  m.m[1][1] = c;
  return m;
}

mat4_t mat4_make_rotation_arbitrary(vec3_t v, float angle) {
  float c = cos(angle);
  float s = sin(angle);
  mat4_t m = mat4_identity();
  vec3_normalize(&v);
  vec4_t normal = vec4_from_vec3(v);
  // Change row vector to column vector
  // m.m[0][0] = (normal.x * normal.x * (1 - c)) + c;
  // m.m[0][1] = ((1 - c) * normal.x * normal.y) + (normal.z * s);
  // m.m[0][2] = ((1 - c) * normal.x * normal.z) - (normal.y * s);
  // m.m[1][0] = ((1 - c) * normal.x * normal.y) - (normal.z * s);
  // m.m[1][1] = normal.y * normal.y * (1 - c) + c;
  // m.m[1][2] = ((1 - c) * normal.y * normal.z) + (normal.x * s);
  // m.m[2][0] = ((1 - c) * normal.x * normal.z) + (normal.y * s);
  // m.m[2][1] = ((1 - c) * normal.y * normal.z) - (normal.x * s);
  // m.m[2][2] = (normal.z * normal.z * (1 - c)) + c;

  m.m[0][0] = (normal.x * normal.x * (1 - c)) + c;
  m.m[1][0] = ((1 - c) * normal.x * normal.y) + (normal.z * s);
  m.m[2][0] = ((1 - c) * normal.x * normal.z) - (normal.y * s);
  m.m[0][1] = ((1 - c) * normal.x * normal.y) - (normal.z * s);
  m.m[1][1] = normal.y * normal.y * (1 - c) + c;
  m.m[2][1] = ((1 - c) * normal.y * normal.z) + (normal.x * s);
  m.m[0][2] = ((1 - c) * normal.x * normal.z) + (normal.y * s);
  m.m[1][2] = ((1 - c) * normal.y * normal.z) - (normal.x * s);
  m.m[2][2] = (normal.z * normal.z * (1 - c)) + c;
  return m;
}
