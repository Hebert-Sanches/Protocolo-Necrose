#include "core/entities.h"
#include "core/game.h"
#include "core/camera.h"
#include "audio/audio_system.h"
#include <cmath>

bool isWalkable(float x, float z)
{
    auto& lvl = gameLevel();
    float tile = lvl.metrics.tile;
    float offX = lvl.metrics.offsetX;
    float offZ = lvl.metrics.offsetZ;

    int tx = (int)((x - offX) / tile);
    int tz = (int)((z - offZ) / tile);

    const auto& data = lvl.map.data();

    if (tz < 0 || tz >= (int)data.size()) return false;
    if (tx < 0 || tx >= (int)data[tz].size()) return false;

    char c = data[tz][tx];
    if (c == '1' || c == '2' || c == 'P') return false;

    return true;
}

void updateEntities(float dt)
{
    auto& g = gameContext();
    auto& lvl = gameLevel();
    auto& audio = gameAudio();

    // 1. ATUALIZAÇÃO DOS INIMIGOS
    for (auto& en : lvl.enemies)
    {
        if (en.state == STATE_DEAD)
        {
            en.respawnTimer -= dt;
            if (en.respawnTimer <= 0.0f)
            {
                en.state = STATE_IDLE;
                en.hp = 100;
                en.x = en.startX;
                en.z = en.startZ;
                en.hurtTimer = 0.0f;
            }
            continue;
        }

        if (en.hurtTimer > 0.0f) en.hurtTimer -= dt;

        float dx = camX - en.x;
        float dz = camZ - en.z;
        float dist = std::sqrt(dx * dx + dz * dz);

        switch (en.state)
        {
        case STATE_IDLE:
            if (dist < ENEMY_VIEW_DIST) en.state = STATE_CHASE;
            break;

        case STATE_CHASE:
            if (dist < ENEMY_ATTACK_DIST)
            {
                en.state = STATE_ATTACK;
                en.attackCooldown = 0.5f;
            }
            else if (dist > ENEMY_VIEW_DIST * 1.5f)
            {
                en.state = STATE_IDLE;
            }
            else
            {
                float dirX = dx / dist;
                float dirZ = dz / dist;
                float moveStep = ENEMY_SPEED * dt;

                float nextX = en.x + dirX * moveStep;
                if (isWalkable(nextX, en.z)) en.x = nextX;

                float nextZ = en.z + dirZ * moveStep;
                if (isWalkable(en.x, nextZ)) en.z = nextZ;
            }
            break;

        case STATE_ATTACK:
            if (dist > ENEMY_ATTACK_DIST)
            {
                en.state = STATE_CHASE;
            }
            else
            {
                en.attackCooldown -= dt;
                if (en.attackCooldown <= 0.0f)
                {
                    g.player.health -= 10;
                    en.attackCooldown = 1.0f;
                    g.player.damageAlpha = 1.0f;
                    audioPlayHurt(audio);
                }
            }
            break;
        }
    }

    // 2. ATUALIZAÇÃO DOS ITENS (COLETA)
    for (auto& item : lvl.items)
    {
        if (!item.active)
        {
            // Respawn de itens de cura/bala no mapa (ajustado para demorar mais)
            item.respawnTimer -= dt;
            if (item.respawnTimer <= 0.0f) item.active = true;
            continue;
        }

        float dx = camX - item.x;
        float dz = camZ - item.z;

        if (dx * dx + dz * dz < 1.0f)
        {
            if (item.type == ITEM_HEALTH)
            {
                if (g.player.health < 100) { // Só pega se estiver ferido
                    item.active = false;
                    item.respawnTimer = 45.0f; // Demora 45s para reaparecer
                    g.player.health += 50;
                    if (g.player.health > 100) g.player.health = 100;
                    g.player.healthAlpha = 1.0f;
                }
            }
            else if (item.type == ITEM_AMMO)
            {
                // Só pega munição se não estiver com a reserva cheia (ex: 30 balas)
                if (g.player.reserveAmmo < 30) {
                    item.active = false;
                    item.respawnTimer = 60.0f; // Munição demora 1 minuto para voltar
                    g.player.reserveAmmo += 10; // Soma 10 em vez de fixar em 20
                }
            }
            else if (item.type == ITEM_CARTAO)
            {
                item.active = false;
                item.respawnTimer = 999999.0f; // Cartão nunca volta
                g.player.temCartao = true;
            } 
        } 
    } 
}