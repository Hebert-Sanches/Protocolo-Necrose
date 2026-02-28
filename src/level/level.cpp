#include "level/level.h"
#include "core/config.h" 
#include <cstdio>

static const float ENEMY_START_HP = 100.0f;

bool loadLevel(Level &lvl, const char *mapPath, float tileSize)
{
    // 1. Carrega o mapa de texto (paredes, chão)
    if (!lvl.map.load(mapPath))
    {
        std::printf("ERRO: nao foi possivel carregar o mapa: %s\n", mapPath);
        return false;
    }

    lvl.metrics = LevelMetrics::fromMap(lvl.map, tileSize);
    
    lvl.enemies.clear();
    lvl.items.clear();

    // 2. Escaneia o mapa procurando Entidades (E, H, etc)
    
    const auto& data = lvl.map.data();
    int H = lvl.map.getHeight();

    for(int z = 0; z < H; z++)
    {
        const std::string& row = data[z];
        for(int x = 0; x < (int)row.size(); x++)
        {
            char c = row[x];
            float wx, wz;
            lvl.metrics.tileCenter(x, z, wx, wz);

            // --- ALTERAÇÃO AQUI: Lógica para múltiplos inimigos (E, F, G) ---
            int enemyType = -1; 

            if (c == 'J') enemyType = 0;      // Inimigo Tipo 1
            else if (c == 'T') enemyType = 1; // Inimigo Tipo 2 
            else if (c == 'M') enemyType = 2; // Inimigo Tipo 3 
            else if (c == 'G') enemyType = 3; // Inimigo Tipo 4
            else if (c == 'K') enemyType = 4; // Inimigo Tipo 5

            if (enemyType != -1) 
            {
                Enemy e;
                e.type = enemyType; 

                e.x = wx;
                e.z = wz;

               
                e.startX = wx; 
                e.startZ = wz;
                e.respawnTimer = 0.0f;

                e.hp = ENEMY_START_HP;
                e.state = STATE_IDLE;
                e.animFrame = 0;
                e.animTimer = 0;
                e.hurtTimer = 0.0f;
                e.attackCooldown = 0.0f; 

                lvl.enemies.push_back(e);
            }
            

            else if (c == 'H') // Health Kit
            {
                Item i;
                i.x = wx;
                i.z = wz;
                i.type = ITEM_HEALTH;
                i.active = true;
                i.respawnTimer = 0.0f;
                lvl.items.push_back(i);
            }
             else if (c == 'A') // Ammo (Munição)
            {
                Item i;
                i.x = wx;
                i.z = wz;
                i.type = ITEM_AMMO;
                i.active = true;
                lvl.items.push_back(i);
            }
        }
    }

    return true;
}

void applySpawn(const Level &lvl, float &camX, float &camZ)
{
    lvl.metrics.spawnPos(lvl.map, camX, camZ);
}