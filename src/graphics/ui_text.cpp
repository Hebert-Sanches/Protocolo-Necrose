#include "graphics/ui_text.h"
#include <GL/glew.h>
#include <GL/glut.h>

void uiDrawStrokeText(float x, float y, const char* text, float scale)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    glLineWidth(2.0f);

    for (const char* c = text; *c; ++c)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);

    glPopMatrix();
}

float uiStrokeTextWidth(const char* text)
{
    float w = 0.0f;
    for (const char* c = text; *c; ++c)
        w += glutStrokeWidth(GLUT_STROKE_ROMAN, *c);
    return w;
}

float uiStrokeTextWidthScaled(const char* text, float scale)
{
    return uiStrokeTextWidth(text) * scale;
}
