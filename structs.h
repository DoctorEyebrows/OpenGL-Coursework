#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

typedef struct Vertex
{
    float x, y, z;
} Vertex;

typedef struct Normal
{
    float x, y, z;
} Normal;

typedef struct UV
{
    float x, y;
} UV;

typedef struct Face {
    float* vertices[3];
    float* normals[3];
    float* texcoords[3];
} Face;
/* Face contains arrays of float* rather than Vertex* so that
 * face.vertices[0][0] works (rather than ((float*)face.vertices[0])[0]
 * The arrays really point to something that can be interpretted validly
 * as either an array of floats or a struct, but the former is more convenient here
*/

#endif // STRUCTS_H_INCLUDED
