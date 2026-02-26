#include "graphics/lighting.h"
#include <GL/glew.h>
#include <GL/glut.h>

void setupIndoorLightOnce()
{
    glEnable(GL_LIGHT1);
    GLfloat lampDiffuse[]  = {1.7f, 1.7f, 1.8f, 1.0f};
    GLfloat lampSpecular[] = {0, 0, 0, 1.0f};
    GLfloat lampAmbient[]  = {0.98f, 0.99f, 1.41f, 1.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lampDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lampSpecular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, lampAmbient);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.6f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.06f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.02f);
    glDisable(GL_LIGHT1);
}

void setupSunLightOnce()
{
    glEnable(GL_LIGHT0);
    
    // 1. ZERANDO A LUZ GLOBAL
    GLfloat sceneAmbient[] = {0.02f, 0.02f, 0.02f, 1.0f}; 
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, sceneAmbient);
    
    // 2. Zerando propriedades antigas do sol do professor
    GLfloat sunSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpecular);
    
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void setSunDirectionEachFrame()
{
    // --- TRAVA DE SEGURANÇA ---
    glEnable(GL_LIGHT0);
    GLfloat breuTotal[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, breuTotal);

    glPushMatrix();
    glLoadIdentity();

    // 1. POSIÇÃO DA LUZ (O Truque: Um pouco atrás da sua cabeça!)
    // Usamos 1.5f no eixo Z. Isso puxa a lâmpada para trás de você,
    // impedindo que ela "entre" na parede quando você chega muito perto.
    GLfloat lightPos[] = {0.0f, 0.0f, 1.5f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // 2. DIREÇÃO DA LUZ (Sempre para a frente: eixo -Z)
    GLfloat lightDir[] = {0.0f, 0.0f, -1.0f};
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDir);

    glPopMatrix(); 

    // 3. O CONE DA LANTERNA (Mais aberto)
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 35.0f);   // Aumentamos o ângulo para iluminar mais as laterais
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 5.0f);  // Deixa a luz espalhar de forma mais uniforme

    // 4. ATENUAÇÃO (A luz vai mais longe agora)
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.02f);    // Antes era 0.05 (agora perde força mais devagar)
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.005f); // Antes era 0.01 (deixa o brilho chegar mais longe)

    // 5. A COR DA LUZ (Mais intensa)
    GLfloat luzEscura[] = {0.0f, 0.0f, 0.0f, 1.0f}; 
    GLfloat luzBranca[] = {1.0f, 1.0f, 0.9f, 1.0f}; // Luz branca no brilho máximo (1.0)
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzEscura);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzBranca);
}