#include <gl/glut.h>
#include <stdbool.h>    //don't even ask

#include "input.h"

bool keyStates[256];
bool specialKeyStates[256];

void keyPress(unsigned char key, int x, int y)
{
    keyStates[key] = true;
}

void keyUp(unsigned char key, int x, int y)
{
    keyStates[key] = false;
}

void specialKeyPress(int key, int x, int y)
{
    specialKeyStates[key] = true;
}

void specialKeyUp(int key, int x, int y)
{
    specialKeyStates[key] = false;
}
