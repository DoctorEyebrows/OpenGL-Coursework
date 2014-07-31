#ifndef LOAD_H_INCLUDED
#define LOAD_H_INCLUDED

#include <gl/gl.h>
#include "structs.h"

GLbyte* loadTGAFile(char* filename, int* width, int* height, GLenum* format);
Face* loadObjFile();
void parsef(char* line, float* vert, int n);
void parseFace(char* line, Face* face, Vertex* verts, Normal* norms, UV* tex);

#endif // LOAD_H_INCLUDED
