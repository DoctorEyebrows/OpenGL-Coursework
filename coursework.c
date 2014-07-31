#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glext.h>
#include <gl/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>    //don't even ask

#include "tgaheader.h"
#include "input.h"
#include "load.h"
#include "structs.h"


//=============================PROTOTYPES===============================

void setupRC();
void setupWorld();
void setupTextures();
void render();
void reshape(GLsizei w, GLsizei h);
void renderWater();
void renderBoat();
void renderSkybox();

void keyboardHandler();
void update(int value);


//=============================GLOBALS==================================

#define PI 3.141592653589793
const float degtorad = 2*PI/360;

Face* boat;
int nboatfaces;
const GLuint boatTexture    = 1;
const GLuint waterTexture   = 2;
const GLuint sailTexture    = 3;
const GLuint skyboxTexture  = 4;

extern bool keyStates[];
extern bool specialKeyStates[];
float theta = 0;

float boatx = 0;
float boaty = 0;
float boatz = 0;
float boatdir = 0;
float boatvel = 0;
float boatPitch = 0;
float boatRoll = 0;
float sailExtension = 0;
bool jimmiesMode = false;
float sailColor[3];
int sailTexFrame = 0;

const float AMBIENT[] = {0.3f,0.3f,0.3f,1.0f};
const float DIFFUSE[] = {0.6f,0.6f,0.6f,1.0f};
const float SUNPOS[] = {45.0f,45.0f,45.0f,1.0f};

//=============================FUNCTIONS================================

int main(int argc, char* argv[])
{
    glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640,480);
	glutCreateWindow("Coursework");

    printf("%s\n",glGetString(GL_VERSION));

	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPress);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKeyPress);
    glutSpecialUpFunc(specialKeyUp);
	glutTimerFunc(10, update, 1);

	setupRC();
	setupWorld();


    for(int i=0; i<256; i++)
    {
        keyStates[i] = false;
        specialKeyStates[i] = false;
    }

	glutMainLoop();

	return 0;
}

void setupRC()
{
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,AMBIENT);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,DIFFUSE);
    glLightfv(GL_LIGHT0,GL_POSITION,SUNPOS);

    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
}

void setupWorld()
{
    srand(time(NULL));
    boat = loadObjFile();
    setupTextures();
}

void setupTextures()
{
    GLbyte* tex;

    int width, height;
    GLenum format;
    char* filenames[4];
    filenames[0] = "Texture - Wood.tga";
    filenames[1] = "water.tga";
    filenames[2] = "notears.tga";
    filenames[3] = "skybox_texture.tga";
    for(int i=0; i<4; i++)
    {

        glBindTexture(GL_TEXTURE_2D,i+1);
        tex = loadTGAFile(filenames[i],&width,&height,&format);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,format,GL_UNSIGNED_BYTE,tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);

}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    renderSkybox();
    glLoadIdentity();
    //viewing transformation:
    glTranslatef(0.0f,0.0f,-14.0f);
    glRotatef(17,1,0,0);
    glRotatef(theta,0,1,0);

    renderBoat();
    glTranslatef(0.0f,-0.5f,0.0f);
    renderWater();

    glutSwapBuffers();
}

void renderBoat()
{
    glPushMatrix();
    glTranslatef(0.0f,boaty,0.0f);
    glRotatef(boatdir,0,1,0);
    float mod = sqrt(boatPitch*boatPitch + boatRoll*boatRoll);
    glRotatef(mod,boatRoll,0.0f,boatPitch);

    glColor3f(1.0f,1.0f,1.0f);
    glBindTexture(GL_TEXTURE_2D,boatTexture);
    glBegin(GL_TRIANGLES);
        for(int i=0; i<nboatfaces; i++)
        {
            for(int j=0; j<3; j++)
            {
                glNormal3fv(boat[i].normals[j]);
                glTexCoord2fv(boat[i].texcoords[j]);
                glVertex3fv(boat[i].vertices[j]);
            }
        }
    glEnd();

    //draw sail:
    glDisable(GL_CULL_FACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D,sailTexture);
    glColor3fv(sailColor);
    float offset = 0.5f*sailTexFrame;
    float bottom = 4.5f - 3.0f * sailExtension;
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,1.0f-offset);
        glVertex3f(-0.1f,4.5f,-1.8f);   //upper right
        glTexCoord2f(0.0f,0.5f-offset);
        glVertex3f(-0.11f,bottom,-1.8f);  //lower right
        glTexCoord2f(1.0f,0.5f-offset);
        glVertex3f(-0.11f,bottom,1.8f);   //lower left
        glTexCoord2f(1.0f,1.0f-offset);
        glVertex3f(-0.1f,4.5f,1.8f);    //upper left
    glEnd();

    glEnable(GL_CULL_FACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1.0f,1.0f,1.0f);

    glPopMatrix();
}

void renderWater()
{
    glBindTexture(GL_TEXTURE_2D,waterTexture);

    float q = 100.0f;
    float r = 400.0f;
    float soffset = boatz * (q/(2*r));
    float toffset = boatx * (q/(2*r));
    glBegin(GL_QUADS);
        glTexCoord2f(q+soffset, q+toffset);
        glVertex3f(r,0.0f,r);

        glTexCoord2f(0.0f+soffset, q+toffset);
        glVertex3f(r,0.0f,-r);

        glTexCoord2f(0.0f+soffset, 0.0f+toffset);
        glVertex3f(-r,0.0f,-r);

        glTexCoord2f(q+soffset, 0.0f+toffset);
        glVertex3f(-r,0.0f,r);
    glEnd();
}

void renderSkybox()
{
    glDisable(GL_CULL_FACE);
    glBindTexture(GL_TEXTURE_2D,skyboxTexture);
    float skylight[4] = {0.8f,0.8f,0.8f,1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,skylight);

    //draw sides:
    glLoadIdentity();
    glRotatef(20,1,0,0);
    glRotatef(theta,0,1,0);
    for(int i=0; i<4; i++)
    {
        glTranslatef(0.0f,0.0f,-5.0f);
        glBegin(GL_QUADS);
            glTexCoord2f(0.25f * i, 0.6666f);
            glVertex3f(-5.0f,5.0f,0.0f);
            glTexCoord2f(0.25f * (i+1), 0.6666f);
            glVertex3f(5.0f,5.0f,0.0f);
            glTexCoord2f(0.25f * (i+1), 0.3333f);
            glVertex3f(5.0f,-5.0f,0.0f);
            glTexCoord2f(0.25f * i, 0.3333f);
            glVertex3f(-5.0f,-5.0f,0.0f);
        glEnd();

        glTranslatef(0.0f,0.0f,5.0f);
        glRotatef(90,0,-1,0);
    }

    glEnable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,AMBIENT);
}

void reshape(GLsizei w, GLsizei h)
{
    GLfloat fAspect;

    if(h == 0)
        h = 1;
    fAspect = (GLfloat)w / (GLfloat)h;

    glViewport(0,0,w,h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0f, fAspect, 1.0f, 400.0f);

}

void keyboardHandler()
{
    if(keyStates['r'])
    {
        printf("derp\n");
        setupWorld();
        keyStates['r'] = false;
    }
    if(keyStates['w'])
    {
        if(boatvel < 0.15)
            boatvel += 0.001;
    }
    else
    {
        if(boatvel > 0)
            boatvel -= 0.001;
    }
    if(keyStates['a'])
    {
        boatdir += 0.2;
    }
    if(keyStates['d'])
    {
        boatdir -= 0.2;
    }
    if(keyStates[' '])
    {
        jimmiesMode ^= 1;
        keyStates[' '] = false;
    }

    if(specialKeyStates[GLUT_KEY_LEFT])
    {
        theta++;
    }
    if(specialKeyStates[GLUT_KEY_RIGHT])
    {
        theta--;
    }
}

void update(int value)
{
    static int tick = 0;

    keyboardHandler();

    boatx += cos(boatdir*degtorad) * boatvel;
    boatz -= sin(boatdir*degtorad) * boatvel;
    boaty = 0.2*sin(0.005*tick);
    boatPitch = 5*sin(tick*0.008);
    boatRoll = 5*sin(tick*0.01);

    if(tick%10 == 0)
    {
        sailColor[0] = 1.0f*(rand()%2);
        sailColor[1] = 1.0f*(rand()%2);
        sailColor[2] = 1.0f*(rand()%2);
    }
    if(tick%50 == 0)
        sailTexFrame ^= 1;

    if(jimmiesMode && sailExtension < 1.0f)
    {
        sailExtension += 0.01f;
    }
    if(!jimmiesMode)
    {
        if(sailExtension > 0.0f)
            sailExtension -= 0.01f;
        else
            sailExtension = 0.0f;
    }

    tick++;
    glutPostRedisplay();
    glutTimerFunc(10, update, 1);
}
