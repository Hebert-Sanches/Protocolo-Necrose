// #include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "core/game_enums.h"
#include "core/game_state.h"

#include "core/game.h"

#include "level/level.h"

#include "core/camera.h"
#include "input/input.h"
#include "input/keystate.h"

#include "graphics/drawlevel.h"
#include "graphics/skybox.h"
#include "graphics/hud.h"
#include "graphics/menu.h"
#include "graphics/lighting.h"

#include "core/movement.h"
#include "core/player.h"
#include "core/entities.h"
#include "core/particles.h" // Nossa nova física

#include "audio/audio_system.h"

#include "utils/assets.h"
#include "core/config.h"

#include "core/window.h"

#include <GL/glew.h>
#include <GL/glut.h>

// ---> CORREÇÃO 1: Declarando as variáveis globais dos Shaders aqui no topo
static GLint locLavaTime = -1;
static GLint locBloodTime = -1;
static GLint locNeblinaCameraPos = -1;

GLuint gProgNeblinaGlobal = 0;

ParticleSystem gBloodParticles; // Criando o sistema

static HudTextures gHudTex;
static GameContext g;

constexpr int MAX_MAGAZINE = 12;
static int g_nivelAtual = 1; 

// --- Assets / Level ---
static GameAssets gAssets;
Level gLevel;
static AudioSystem gAudioSys;

GameContext &gameContext() { return g; }

AudioSystem &gameAudio() { return gAudioSys; }

Level &gameLevel() { return gLevel; }

GameState gameGetState() { return g.state; }

void gameSetState(GameState s) { g.state = s; }

void gameTogglePause()
{
    if (g.state == GameState::JOGANDO)
        g.state = GameState::PAUSADO;
    else if (g.state == GameState::PAUSADO)
        g.state = GameState::JOGANDO;
}

// --- INIT ---
bool gameInit(const char *mapPath)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    setupSunLightOnce();
    setupIndoorLightOnce();

    if (!loadAssets(gAssets))
        return false;

    g.r.texChao = gAssets.texChao;
    g.r.texParede = gAssets.texParede;
    g.r.texSangue = gAssets.texSangue;
    g.r.texLava = gAssets.texLava;
    g.r.texChaoInterno = gAssets.texChaoInterno;
    g.r.texParedeInterna = gAssets.texParedeInterna;
    g.r.texTeto = gAssets.texTeto;

    g.r.texParedeFloresta = gAssets.texParedeFloresta;
    g.r.texChaoFloresta   = gAssets.texChaoFloresta;

    g.r.texPorta = gAssets.texPorta;

    g.r.texSkydome = gAssets.texSkydome;
    g.r.texMenuBG = gAssets.texMenuBG;

    gHudTex.texHudFundo = gAssets.texHudFundo;
    gHudTex.texGunHUD = gAssets.texGunHUD;
    g.r.progMelt = gAssets.progMelt;
    g.r.progDano = gAssets.progDano;

    gHudTex.texGunDefault = gAssets.texGunDefault;
    gHudTex.texGunFire1 = gAssets.texGunFire1;
    gHudTex.texGunFire2 = gAssets.texGunFire2;
    gHudTex.texGunReload1 = gAssets.texGunReload1;
    gHudTex.texGunReload2 = gAssets.texGunReload2;

    gHudTex.texDamage = gAssets.texDamage;
    gHudTex.texHealthOverlay = gAssets.texHealthOverlay;

    for (int i = 0; i < 5; i++)
    {
        g.r.texEnemies[i] = gAssets.texEnemies[i];
        g.r.texEnemiesRage[i] = gAssets.texEnemiesRage[i];
        g.r.texEnemiesDamage[i] = gAssets.texEnemiesDamage[i];
    }

    g.r.texHealth = gAssets.texHealth;
    g.r.texAmmo = gAssets.texAmmo;
    g.r.texCartao = gAssets.texCartao;

    g.r.progSangue = gAssets.progSangue;
    g.r.progLava = gAssets.progLava;
    g.r.progNeblina = gAssets.progNeblina;

    gProgNeblinaGlobal = g.r.progNeblina;

    // ---> CORREÇÃO 2: Agora as variáveis existem e podem receber os endereços!
    locLavaTime = glGetUniformLocation(g.r.progLava, "uTime");
    locBloodTime = glGetUniformLocation(g.r.progSangue, "uTime");
    locNeblinaCameraPos = glGetUniformLocation(g.r.progNeblina, "cameraPos");

    if (!loadLevel(gLevel, mapPath, GameConfig::TILE_SIZE))
        return false;

    applySpawn(gLevel, camX, camZ);
    camY = GameConfig::PLAYER_EYE_Y;

    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutSetCursor(GLUT_CURSOR_NONE);

    // Audio init + ambient + enemy sources
    audioInit(gAudioSys, gLevel);

    g.state = GameState::MENU_INICIAL;
    g.time = 0.0f;
    g.player = PlayerState{};
    g.weapon = WeaponAnim{};

    return true;
}

// Reinicia o jogo
void gameReset()
{
    g.player.health = 100;
    g.player.currentAmmo = 5;
    g.player.reserveAmmo = 10;

    g.player.damageAlpha = 0.0f;
    g.player.healthAlpha = 0.0f;

    g.player.temCartao = false;
    
    g.nivelAtual = 1; // <-- ADICIONE ESTA LINHA PARA VOLTAR PRO MAPA 1 SE MORRER!

    g.weapon.state = WeaponState::W_IDLE;
    g.weapon.timer = 0.0f;
    // Respawna o jogador
    applySpawn(gLevel, camX, camZ);
}

void gameUpdate(float dt)
{
    g.time += dt;

    // 1. SE NÃO ESTIVER JOGANDO, NÃO RODA A LÓGICA DO JOGO
    if (g.state != GameState::JOGANDO)
    {
        return;
    }

    atualizaMovimento();

    AudioListener L;
    L.pos = {camX, camY, camZ};
    {
        float ry = yaw * 3.14159f / 180.0f;
        float rp = pitch * 3.14159f / 180.0f;
        L.forward = {cosf(rp) * sinf(ry), sinf(rp), -cosf(rp) * cosf(ry)};
    }
    L.up = {0.0f, 1.0f, 0.0f};
    L.vel = {0.0f, 0.0f, 0.0f};

    bool moving = (keyW || keyA || keyS || keyD);
    audioUpdate(gAudioSys, gLevel, L, dt, moving, g.player.health);

    if (g.player.damageAlpha > 0.0f)
    {
        g.player.damageAlpha -= dt * 0.5f;
        if (g.player.damageAlpha < 0.0f)
            g.player.damageAlpha = 0.0f;
    }
    if (g.player.healthAlpha > 0.0f)
    {
        g.player.healthAlpha -= dt * 0.9f;
        if (g.player.healthAlpha < 0.0f)
            g.player.healthAlpha = 0.0f;
    }

    updateEntities(dt);
    gBloodParticles.update(dt); // Atualiza a gravidade do sangue

    updateWeaponAnim(dt);

    // LÓGICA DA PORTA DE SAÍDA ('P')
    int pTx = (int)((camX - gLevel.metrics.offsetX) / gLevel.metrics.tile);
    int pTz = (int)((camZ - gLevel.metrics.offsetZ) / gLevel.metrics.tile);

    const auto& mapData = gLevel.map.data();
    bool encostouNaPorta = false;

    for (int dz = -1; dz <= 1; dz++) {
        for (int dx = -1; dx <= 1; dx++) {
            int vz = pTz + dz;
            int vx = pTx + dx;
            
            if (vz >= 0 && vz < gLevel.map.getHeight() && vx >= 0 && vx < (int)mapData[vz].size()) {
                if (mapData[vz][vx] == 'P') {
                    float portaX, portaZ;
                    gLevel.metrics.tileCenter(vx, vz, portaX, portaZ);
                    
                    float dist = std::sqrt((camX - portaX)*(camX - portaX) + (camZ - portaZ)*(camZ - portaZ));
                    
                    if (dist < 4.5f) { 
                        encostouNaPorta = true;
                    }
                }
            }
        }
    }

        if (encostouNaPorta) {
            // PORTA DESTRAVADA! Ignorando a checagem do cartão:
            // if (g.player.temCartao) { 
            
            g.player.temCartao = false; 
            g.nivelAtual++; 

            if (g.nivelAtual == 2) { 
                loadLevel(gLevel, "maps/map2.txt", gLevel.metrics.tile);
                applySpawn(gLevel, camX, camZ); 
            } 
            else if (g.nivelAtual == 3) { 
                loadLevel(gLevel, "maps/map3.txt", gLevel.metrics.tile);
                applySpawn(gLevel, camX, camZ); 
            } 
            else if (g.nivelAtual > 3) { 
                // ZEROU O JOGO!
                g.state = GameState::VICTORY;
            }
            
            // } <-- A chave que fechava o 'if' do cartão também foi comentada
        }

    // 3. CHECAGEM DE GAME OVER
    if (g.player.health <= 0)
    {
        g.state = GameState::GAME_OVER;
        g.player.damageAlpha = 1.0f;
    }
}

void drawWorld3D()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    
    // ==========================================
    // CONTROLE DE AMBIENTAÇÃO POR FASE
    // ==========================================
    if (g.nivelAtual == 2) {
        // FLORESTA: Céu aberto e neblina verde musgo densa
        GLfloat corNeblinaFloresta[4] = {0.12f, 0.16f, 0.12f, 1.0f}; 
        glClearColor(0.12f, 0.16f, 0.12f, 1.0f); // O fundo da tela DEVE ser igual à neblina
        glFogfv(GL_FOG_COLOR, corNeblinaFloresta);
        
        glFogf(GL_FOG_START, 2.0f);  // A neblina começa super perto
        glFogf(GL_FOG_END, 18.0f);   // Não dá para ver nada muito longe
    } else {
        // HOSPITAL (Mapas 1 e 3): Teto fechado e escuridão
        GLfloat corNeblinaNormal[4] = {0.02f, 0.02f, 0.02f, 1.0f}; 
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glFogfv(GL_FOG_COLOR, corNeblinaNormal);
        
        glFogf(GL_FOG_START, 3.0f);  
        glFogf(GL_FOG_END, 20.0f);   
    }

    float radYaw = yaw * 3.14159265f / 180.0f;
    float radPitch = pitch * 3.14159265f / 180.0f;
    float dirX = cosf(radPitch) * sinf(radYaw);
    float dirY = sinf(radPitch);
    float dirZ = -cosf(radPitch) * cosf(radYaw);
    gluLookAt(camX, camY, camZ, camX + dirX, camY + dirY, camZ + dirZ, 0.0f, 1.0f, 0.0f);

    setSunDirectionEachFrame();

    if (g.nivelAtual == 2) {
        drawSkydome(camX, camY, camZ, g.r);
    }

    if (g.r.progLava > 0) {
        glUseProgram(g.r.progLava);
        glUniform1f(locLavaTime, g.time);
    }
    if (g.r.progSangue > 0) {
        glUseProgram(g.r.progSangue);
        glUniform1f(locBloodTime, g.time);
    }
    glUseProgram(0); 

    // NÃO ESQUEÇA DE PASSAR O g.nivelAtual AQUI!
    drawLevel(gLevel.map, camX, camZ, dirX, dirZ, g.r, g.time, g.nivelAtual);
    
    drawEntities(gLevel.enemies, gLevel.items, camX, camZ, dirX, dirZ, g.r);
    gBloodParticles.draw(camX, camZ); 
    
    glDisable(GL_FOG);
}

// FUNÇÃO PRINCIPAL DE DESENHO (REFATORADA: usa menuRender / pauseMenuRender / hudRenderAll)
void gameRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Monta o estado do HUD a partir das variáveis globais do jogo
    HudState hs;
    hs.playerHealth = g.player.health;
    hs.currentAmmo = g.player.currentAmmo;
    hs.reserveAmmo = g.player.reserveAmmo;
    hs.damageAlpha = g.player.damageAlpha;
    hs.healthAlpha = g.player.healthAlpha;
    hs.weaponState = g.weapon.state;

    // --- ESTADO: MENU INICIAL ---
    if (g.state == GameState::MENU_INICIAL)
    {
        // menuRender já cuida do fogo (update + render)
        menuRender(janelaW, janelaH, g.time, "PROTOCOLO: NECROSE", "Pressione ENTER para Iniciar a Infeccao", g.r);
    }
    
   // --- ESTADO: GAME OVER ---
    else if (g.state == GameState::GAME_OVER)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        drawWorld3D();

        // 2. Tela de Sangue e Textos
        gameOverRender(janelaW, janelaH, g.time, "VOCE FOI INFECTADO", "Pressione ENTER para Tentar Novamente");
    }

    // --- ESTADO: PAUSADO ---
    else if (g.state == GameState::PAUSADO)
    {
        // 1) Mundo 3D congelado
        drawWorld3D();

        // 2) HUD normal (arma + barra + mira + overlays)
        hudRenderAll(janelaW, janelaH, gHudTex, hs, true, true, true);

        // 3) Menu escuro por cima
        pauseMenuRender(janelaW, janelaH, g.time);
    }

    else if (g.state == GameState::VICTORY)
    {
        // Limpa a tela e desenha o fundo de vitória
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        victoryRender(janelaW, janelaH, g.time);
    }

    // --- ESTADO: JOGANDO ---
else // JOGANDO
    {
        drawWorld3D();
        
        // Desenha a arma e o HUD normal
        hudRenderAll(janelaW, janelaH, gHudTex, hs, true, true, true);

        // ===============================================
        // SHADER DE DANO PULSANTE (VIDA <= 30)
        // ===============================================
        if (g.player.health <= 30 && g.r.progDano > 0) {
            
            glUseProgram(g.r.progDano);
            glUniform1f(glGetUniformLocation(g.r.progDano, "uTime"), g.time);
            
            // Salva o estado do OpenGL e desliga o que atrapalha o 2D
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_FOG);
            glDisable(GL_CULL_FACE); // <-- O herói que salva o shader!
            
            // Liga a transparência
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Câmera 2D (0 a 1)
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            gluOrtho2D(0, 1, 1, 0); 
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
            
            // Desenha o quadrado invisível onde o shader vai agir
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 0.0f);
                glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 0.0f);
                glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
                glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 1.0f);
            glEnd();
            
            // Restaura as matrizes e atributos
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            
            glPopAttrib();
            glUseProgram(0);
        }
    }
    
    glutSwapBuffers();
}