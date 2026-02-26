#include "graphics/hud.h"
#include "graphics/ui_text.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <string>
#include <cstdlib>

static void begin2D(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

static void end2D()
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

static void drawCrosshair(int w, int h)
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    begin2D(w, h);

    glColor3f(0.0f, 1.0f, 0.0f);
    glLineWidth(2.0f);

    float cx = w / 2.0f;
    float cy = h / 2.0f;
    float size = 10.0f;

    glBegin(GL_LINES);
    glVertex2f(cx - size, cy); glVertex2f(cx + size, cy);
    glVertex2f(cx, cy - size); glVertex2f(cx, cy + size);
    glEnd();

    end2D();

    glPopAttrib();
}

static void drawDamageOverlay(int w, int h, GLuint texDamage, float alpha)
{
    if (alpha <= 0.0f || texDamage == 0)
        return;

    begin2D(w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, texDamage);
    glColor4f(1, 1, 1, alpha);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f((float)w, 0);
    glTexCoord2f(1, 0); glVertex2f((float)w, (float)h);
    glTexCoord2f(0, 0); glVertex2f(0, (float)h);
    glEnd();

    glDisable(GL_BLEND);

    end2D();
}

static void drawHealthOverlay(int w, int h, GLuint texHealth, float alpha)
{
    if (alpha <= 0.0f || texHealth == 0)
        return;

    begin2D(w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, texHealth);
    glColor4f(1, 1, 1, alpha);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f((float)w, 0);
    glTexCoord2f(1, 0); glVertex2f((float)w, (float)h);
    glTexCoord2f(0, 0); glVertex2f(0, (float)h);
    glEnd();

    glDisable(GL_BLEND);

    end2D();
}

static void drawWeaponHUD(int w, int h, const HudTextures& tex, WeaponState ws)
{
    GLuint currentTex = tex.texGunDefault;

    if (ws == WeaponState::W_FIRE_1 || ws == WeaponState::W_RETURN) currentTex = tex.texGunFire1;
    else if (ws == WeaponState::W_FIRE_2) currentTex = tex.texGunFire2;
    else if (ws == WeaponState::W_RELOAD_1 || ws == WeaponState::W_RELOAD_3) currentTex = tex.texGunReload1;
    else if (ws == WeaponState::W_RELOAD_2) currentTex = tex.texGunReload2;

    if (currentTex == 0)
        return;

    begin2D(w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, currentTex);
    glColor4f(1, 1, 1, 1);

    float gunH = h * 0.5f;
    float gunW = gunH;
    float x = ((w - gunW) / 2.0f) + 350.0f;
    float y = -50.0f;

    if (ws != WeaponState::W_IDLE)
    {
        y -= 20.0f;
        x += (float)(std::rand() % 10 - 5);
    }

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(x, y);
    glTexCoord2f(1, 1); glVertex2f(x + gunW, y);
    glTexCoord2f(1, 0); glVertex2f(x + gunW, y + gunH);
    glTexCoord2f(0, 0); glVertex2f(x, y + gunH);
    glEnd();

    glDisable(GL_BLEND);

    end2D();
}

static void drawDoomBar(int w, int h, const HudTextures& tex, const HudState& s)
{
    if (tex.texHudFundo == 0)
        return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    begin2D(w, h);

    float hBar = h * 0.10f;

    // Fundo (tile)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex.texHudFundo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    float repeticaoX = 6.0f;
    float repeticaoY = 1.0f;

    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);                 glVertex2f(0, 0);
    glTexCoord2f(repeticaoX, 0);         glVertex2f((float)w, 0);
    glTexCoord2f(repeticaoX, repeticaoY);glVertex2f((float)w, hBar);
    glTexCoord2f(0, repeticaoY);         glVertex2f(0, hBar);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // bordas
    glLineWidth(3.0f);
    glColor3f(0.7f, 0.7f, 0.75f);
    glBegin(GL_LINES); glVertex2f(0, hBar); glVertex2f((float)w, hBar); glEnd();

    glColor3f(0.2f, 0.2f, 0.25f);
    glBegin(GL_LINES); glVertex2f(w / 2.0f, 0); glVertex2f(w / 2.0f, hBar); glEnd();

    // texto
    float scaleLbl = 0.0018f * hBar;
    float scaleNum = 0.0035f * hBar;

    float colLbl[3] = {1.0f, 0.8f, 0.5f};
    float colNum[3] = {0.8f, 0.0f, 0.0f};

    // HEALTH label
    float xTextHealth = w * 0.08f;
    float yLblHealth = hBar * 0.35f;
    glColor3fv(colLbl);
    uiDrawStrokeText(xTextHealth, yLblHealth, "HEALTH", scaleLbl);

    // barra vida
    float barH = hBar * 0.5f;
    float barY = (hBar - barH) / 2.0f;
    float barX = xTextHealth + (w * 0.08f);
    float barMaxW = (w * 0.45f) - barX;

    glColor4f(0, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY); glVertex2f(barX + barMaxW, barY);
    glVertex2f(barX + barMaxW, barY + barH); glVertex2f(barX, barY + barH);
    glEnd();

    float pct = (float)s.playerHealth / 100.0f;
    if (pct < 0) pct = 0;
    if (pct > 1) pct = 1;

    if (pct > 0.6f) glColor3f(0.0f, 0.8f, 0.0f);
    else if (pct > 0.3f) glColor3f(1.0f, 0.8f, 0.0f);
    else glColor3f(0.8f, 0.0f, 0.0f);

    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + (barMaxW * pct), barY);
    glVertex2f(barX + (barMaxW * pct), barY + barH);
    glVertex2f(barX, barY + barH);
    glEnd();

    // arma ícone
    if (tex.texGunHUD != 0)
    {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(1, 1, 1);

        float iconSize = hBar * 1.5f;
        float iconY = (hBar - iconSize) / 2.0f + (hBar * 0.1f);

        glBindTexture(GL_TEXTURE_2D, tex.texGunHUD);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        float weaponWidth = iconSize * 2.2f;
        float xIconGun = (w * 0.75f) - (weaponWidth / 2.0f);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(xIconGun, iconY);
        glTexCoord2f(1, 1); glVertex2f(xIconGun + weaponWidth, iconY);
        glTexCoord2f(1, 0); glVertex2f(xIconGun + weaponWidth, iconY + iconSize);
        glTexCoord2f(0, 0); glVertex2f(xIconGun, iconY + iconSize);
        glEnd();

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);

        // AMMO número + label
        float xAmmoBlock = xIconGun + weaponWidth + 10.0f;
        float yNum = hBar * 0.50f;
        float xNum = xAmmoBlock + 5.0f;

        glColor3fv(colNum);
        glPushMatrix();
        glTranslatef(xNum, yNum, 0);
        glScalef(scaleNum, scaleNum, 1);
        {
            std::string sAmmo = std::to_string(s.currentAmmo);
            for (char c : sAmmo) glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, c);
        }
        glPopMatrix();

        glColor3fv(colLbl);
        uiDrawStrokeText(xAmmoBlock, hBar * 0.20f, "AMMO", scaleLbl);
    }

    end2D();
    glPopAttrib();
}

void hudRenderAll(
    int screenW,
    int screenH,
    const HudTextures& tex,
    const HudState& state,
    bool showCrosshair,
    bool showWeapon,
    bool showDoomBar)
{
    // 1. Arma desenhada PRIMEIRO 
    if (showWeapon)  drawWeaponHUD(screenW, screenH, tex, state.weaponState);

    // 2. O HUD Minimalista com Barras Geométricas (NO TOPO)
    if (showDoomBar) {
        begin2D(screenW, screenH); 
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D); 

        // ==========================================
        // BARRA DE VIDA (Superior Esquerdo)
        // ==========================================
        float barX = 60.0f;
        float barY = screenH - 80.0f;  
        float barW = 200.0f; 
        float barH = 25.0f;  

        // Cruz Médica ao lado da barra
        glColor3f(0.9f, 0.1f, 0.1f);
        glBegin(GL_QUADS);
            glVertex2f(barX - 35, barY + 4);  glVertex2f(barX - 15, barY + 4);
            glVertex2f(barX - 15, barY + 21); glVertex2f(barX - 35, barY + 21);
            
            glVertex2f(barX - 43, barY + 8);  glVertex2f(barX - 7, barY + 8);
            glVertex2f(barX - 7, barY + 17);  glVertex2f(barX - 43, barY + 17);
        glEnd();

        // Fundo da barra (Vazia/Dano Tomado)
        glColor3f(0.2f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(barX, barY);               glVertex2f(barX + barW, barY);
            glVertex2f(barX + barW, barY + barH); glVertex2f(barX, barY + barH);
        glEnd();

        // Preenchimento da barra (Vida Atual - Vai diminuindo!)
        glColor3f(0.9f, 0.1f, 0.1f);
        float currentHpW = (state.playerHealth / 100.0f) * barW;
        if (currentHpW < 0) currentHpW = 0;
        glBegin(GL_QUADS);
            glVertex2f(barX, barY);                    glVertex2f(barX + currentHpW, barY);
            glVertex2f(barX + currentHpW, barY + barH); glVertex2f(barX, barY + barH);
        glEnd();

        // Borda Branca da Barra
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(barX, barY);               glVertex2f(barX + barW, barY);
            glVertex2f(barX + barW, barY + barH); glVertex2f(barX, barY + barH);
        glEnd();

        // Número FLUTUANDO ACIMA da Barra de Vida
        glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para o texto
        glRasterPos2f(barX, barY + barH + 10.0f); // <-- AQUI: 10 pixels ACIMA da barra
        std::string hpStr = "HP: " + std::to_string(state.playerHealth) + " %";
        for (char c : hpStr) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

        // ==========================================
        // BARRA DE MUNIÇÃO (Superior Direito)
        // ==========================================
        float ammoX = screenW - 260.0f;
        float ammoY = screenH - 80.0f; 
        
        // Fundo da Munição
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_QUADS);
            glVertex2f(ammoX, ammoY);               glVertex2f(ammoX + barW, ammoY);
            glVertex2f(ammoX + barW, ammoY + barH); glVertex2f(ammoX, ammoY + barH);
        glEnd();

        // Preenchimento da Munição (Dourado/Balas)
        glColor3f(0.8f, 0.6f, 0.1f); 
        float currentAmmoW = (state.currentAmmo / 12.0f) * barW; 
        if (currentAmmoW < 0) currentAmmoW = 0;
        glBegin(GL_QUADS);
            glVertex2f(ammoX, ammoY);                      glVertex2f(ammoX + currentAmmoW, ammoY);
            glVertex2f(ammoX + currentAmmoW, ammoY + barH); glVertex2f(ammoX, ammoY + barH);
        glEnd();

        // Borda Branca
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(ammoX, ammoY);               glVertex2f(ammoX + barW, ammoY);
            glVertex2f(ammoX + barW, ammoY + barH); glVertex2f(ammoX, ammoY + barH);
        glEnd();

        // Número FLUTUANDO ACIMA da Barra de Munição
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(ammoX, ammoY + barH + 10.0f); // <-- AQUI: 10 pixels ACIMA da barra
        std::string ammoStr = "AMMO: " + std::to_string(state.currentAmmo) + " / " + std::to_string(state.reserveAmmo);
        for (char c : ammoStr) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

        glEnable(GL_TEXTURE_2D);
        end2D();
    }

    // 3. Mira e Sangue por cima de tudo
    if (showCrosshair) drawCrosshair(screenW, screenH);
    drawDamageOverlay(screenW, screenH, tex.texDamage, state.damageAlpha);
    drawHealthOverlay(screenW, screenH, tex.texHealthOverlay, state.healthAlpha);
}
