#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdio>

#include "core/app.h"
#include "core/window.h"
#include "core/game.h"

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(janelaW, janelaH);
    glutCreateWindow("Agora é DOOM");

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::printf("Erro GLEW: %s\n", glewGetErrorString(err));
        return 1;
    }

    appInit();

    if (!gameInit("maps/map1.txt"))
        return 1;

    glutMainLoop();
    return 0;
}
