#include "core/player.h"
#include "core/game.h"
#include "core/camera.h"
#include "audio/audio_system.h"
#include <cmath>
#include "core/particles.h"

extern ParticleSystem gBloodParticles;

constexpr int MAX_MAGAZINE = 12;

static constexpr float HIT_RADIUS = 0.55f;

static constexpr float MAX_RANGE  = 17.0f;

static bool rayCircleIntersectXZ(
    float ox, float oz,     
    float dx, float dz,     
    float cx, float cz,     
    float r,                
    float &outT             
)
{
    // vetor da origem até o centro
    float fx = cx - ox;
    float fz = cz - oz;

    // projeção no raio
    float proj = fx * dx + fz * dz;
    if (proj < 0.0f)
        return false; // inimigo atrás

    // ponto mais próximo no raio
    float px = ox + dx * proj;
    float pz = oz + dz * proj;

    // distância do centro ao ponto mais próximo
    float ex = cx - px;
    float ez = cz - pz;
    float d2 = ex * ex + ez * ez;

    float r2 = r * r;
    if (d2 > r2)
        return false; // não cruza o círculo

    // calcula ponto de entrada na circunferência (o mais próximo)
    float thc = std::sqrt(r2 - d2);
    float t0 = proj - thc; // entrada
    float t1 = proj + thc; // saída

    // se estiver dentro do círculo, t0 pode ser negativo
    outT = (t0 >= 0.0f) ? t0 : t1;
    return outT >= 0.0f;
}

void playerTryReload()
{
    auto &g = gameContext();
    auto &audio = gameAudio();

    if (g.weapon.state != WeaponState::W_IDLE)
        return;
    if (g.player.currentAmmo >= MAX_MAGAZINE)
        return;
    if (g.player.reserveAmmo <= 0)
        return;

    g.weapon.state = WeaponState::W_RELOAD_1;
    g.weapon.timer = 0.50f;

    audioPlayReload(audio);
}

void playerTryAttack()
{
    auto &g = gameContext();
    auto &lvl = gameLevel();
    auto &audio = gameAudio();

    if (g.weapon.state != WeaponState::W_IDLE)
        return;
    if (g.player.currentAmmo <= 0)
        return;

    g.player.currentAmmo--;

    audioOnPlayerShot(audio);
    audioPlayShot(audio);

    g.weapon.state = WeaponState::W_FIRE_1;
    g.weapon.timer = 0.08f;

    // 1) raio sai do centro da visão do player (yaw)
    float radYaw = yaw * 3.14159f / 180.0f;
    float dirX = std::sin(radYaw);
    float dirZ = -std::cos(radYaw);

    // normaliza direção
    float len = std::sqrt(dirX * dirX + dirZ * dirZ);
    if (len <= 0.0f)
        return;
    dirX /= len;
    dirZ /= len;

    // 2) procura o inimigo mais próximo que o raio intersecta
    int bestIdx = -1;
    float bestT = MAX_RANGE;

    for (int i = 0; i < (int)lvl.enemies.size(); ++i)
    {
        auto &en = lvl.enemies[i];
        if (en.state == STATE_DEAD)
            continue;

        float tHit = 0.0f;
        if (rayCircleIntersectXZ(camX, camZ, dirX, dirZ, en.x, en.z, HIT_RADIUS, tHit))
        {
            if (tHit <= bestT)
            {
                bestT = tHit;
                bestIdx = i;
            }
        }
    }

    // 3) se acertou alguém, aplica dano e ESPIRRA SANGUE!
    if (bestIdx >= 0)
    {
        auto &en = lvl.enemies[bestIdx];

        en.hp -= 30;
        en.hurtTimer = 0.5f;

        // 150 partículas de sangue saindo do zumbi!
        gBloodParticles.spawnBlood(en.x, 1.5f, en.z, 150);

if (en.hp <= 0)
        {
            en.state = STATE_DEAD;
            
            if (en.type == 4) {
                en.respawnTimer = 60.0f; // Boss 'K' demora 1 minuto
            } else {
                en.respawnTimer = 15.0f; // Normais demoram 15s
            }

            if (en.type == 2) 
            {
                Item drop;
                drop.type = ITEM_CARTAO;
                drop.x = en.x;
                drop.z = en.z;
                drop.active = true;
                drop.respawnTimer = 0.0f;

                lvl.items.push_back(drop);
            }
            
        }
    }
}

void updateWeaponAnim(float dt)
{
    auto &g = gameContext();
    auto &audio = gameAudio();

    const float TIME_FRAME_2 = 0.12f;
    const float RELOAD_T2 = 0.85f;
    const float RELOAD_T3 = 0.25f;

    if (g.weapon.state == WeaponState::W_IDLE)
        return;

    g.weapon.timer -= dt;
    if (g.weapon.timer > 0.0f)
        return;

    if (g.weapon.state == WeaponState::W_FIRE_1)
    {
        g.weapon.state = WeaponState::W_FIRE_2;
        g.weapon.timer = TIME_FRAME_2;
    }
    else if (g.weapon.state == WeaponState::W_FIRE_2)
    {
        g.weapon.state = WeaponState::W_PUMP;
        g.weapon.timer = AudioTuning::PUMP_TIME;
        audioPlayPumpClick(audio);
    }
    else if (g.weapon.state == WeaponState::W_RETURN)
    {
        g.weapon.state = WeaponState::W_IDLE;
        g.weapon.timer = 0.0f;
    }
    else if (g.weapon.state == WeaponState::W_PUMP)
    {
        g.weapon.state = WeaponState::W_IDLE;
        g.weapon.timer = 0.0f;
    }
    else if (g.weapon.state == WeaponState::W_RELOAD_1)
    {
        g.weapon.state = WeaponState::W_RELOAD_2;
        g.weapon.timer = RELOAD_T2;
    }
    else if (g.weapon.state == WeaponState::W_RELOAD_2)
    {
        g.weapon.state = WeaponState::W_RELOAD_3;
        g.weapon.timer = RELOAD_T3;
    }
    else if (g.weapon.state == WeaponState::W_RELOAD_3)
    {
        g.weapon.state = WeaponState::W_IDLE;
        g.weapon.timer = 0.0f;

        int needed = MAX_MAGAZINE - g.player.currentAmmo;
        if (needed > g.player.reserveAmmo)
            needed = g.player.reserveAmmo;

        g.player.currentAmmo += needed;
        g.player.reserveAmmo -= needed;
    }
}
