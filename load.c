#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glext.h>

#include "tgaheader.h"
#include "load.h"

extern int nboatfaces;

GLbyte* loadTGAFile(char* filename, int* width, int* height, GLenum* format)
{
    FILE* fin = fopen(filename,"rb");
    if(fin==NULL)
    {
        printf("Couldn't open file %s\n",filename);
        //return NULL;
    }

    //read in TGA header:
    TGAHeader header;
    fread(&header,18,1,fin);
    /*
    printf("%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",
           header.id_len,header.map_t,header.img_t,header.map_first,header.map_len,
           header.map_entry,header.x,header.y,header.width,header.height,
           header.depth,header.alpha);
    */
    printf("width = %d\nheight = %d\ndepth = %d\n",header.width,header.height,header.depth);
    *width = header.width;
    *height = header.height;
    if(header.depth == 24)
        *format = GL_BGR;
    else
        *format = GL_BGRA;

    long int imgSize = header.width*header.height*header.depth / 8;
    GLbyte* data = malloc(imgSize);
    fread(data,imgSize,1,fin);

    printf("Done loading texture\n");
    return data;
}

Face* loadObjFile()
{
    FILE* fin = fopen("ship.obj","r");
    if(fin == NULL)
        printf("Couldn't load ship.obj\n");

    char lineBuffer[80];

    //count everything
    int nverts=0, nnorms=0, ntex=0, nfaces=0;
    while(fgets(lineBuffer,80,fin))
    {
        if(strlen(lineBuffer) < 2)
            continue;

        if(strncmp(lineBuffer,"v ",2)==0)
            nverts++;
        else if(strncmp(lineBuffer,"vn",2)==0)
            nnorms++;
        else if(strncmp(lineBuffer,"vt",2)==0)
            ntex++;
        else if(strncmp(lineBuffer,"f ",2)==0)
            nfaces++;
    }
    printf("v: %d\nvn: %d\nvt: %d\nf: %d\n",nverts,nnorms,ntex,nfaces);
    nboatfaces = nfaces;

    //create temporary arrays:
    Vertex* vertices = malloc(sizeof(Vertex) * nverts);
    Normal* normals = malloc(sizeof(Vertex) * nnorms);
    UV* tex = malloc(sizeof(UV) * ntex);
    Face* faces = malloc(sizeof(Face) * nfaces);

    //populate arrays:
    fseek(fin,0,SEEK_SET);
    int v=0, n=0, t=0, f=0;
    while(fgets(lineBuffer,80,fin))
    {
        if(strlen(lineBuffer) < 2)
            continue;

        if(strncmp(lineBuffer,"v ",2)==0) {
            parsef(lineBuffer,(float*)&vertices[v],3);
            v++;
        }
        else if(strncmp(lineBuffer,"vn",2)==0) {
            parsef(lineBuffer,(float*)&normals[n],3);
            n++;
        }
        else if(strncmp(lineBuffer,"vt",2)==0) {
            parsef(lineBuffer,(float*)&tex[t],2);
            t++;
        }
        else if(strncmp(lineBuffer,"f ",2)==0) {
            parseFace(lineBuffer,&faces[f],vertices,normals,tex);
            f++;
        }
    }
    printf("Finished loading object file\n");

    return faces;
}

void parsef(char* line, float* vert, int n)
{
    char* p = strtok(line," ");

    for(int i=0; i<n; i++)
    {
        p = strtok(NULL," ");
        vert[i] = atof(p);
    }
}

void parseFace(char* line, Face* face, Vertex* verts, Normal* norms, UV* tex)
{
    int floatindex;
    char* p = strtok(line," /");

    for(int i=0; i<3; i++)
    {
        p = strtok(NULL," /");
        floatindex = atoi(p) - 1;   //data are 1-indexed in the file
        face->vertices[i] = &verts[floatindex];
        p = strtok(NULL," /");
        floatindex = atoi(p) - 1;
        face->texcoords[i] = &tex[floatindex];
        p = strtok(NULL," /");
        floatindex = atoi(p) - 1;
        face->normals[i] = &norms[floatindex];
    }
/*
    printf("(%f,%f,%f), (%f,%f), (%f,%f,%f)\n",
           face->vertices[0][0], face->vertices[0][1], face->vertices[0][2],
           face->texcoords[0][0], face->texcoords[0][1],
           face->normals[0][0], face->normals[0][1], face->normals[0][2]);
*/
}
