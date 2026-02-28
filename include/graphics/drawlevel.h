#ifndef DRAWLEVEL_H
#define DRAWLEVEL_H

#include "level/maploader.h"
#include "core/entities.h"
#include <vector>

struct RenderAssets; 

void drawLevel(const MapLoader &map, float px, float pz, float dx, float dz, const RenderAssets &r, float time, int nivelAtual);

void drawEntities(const std::vector<Enemy> &enemies,
                  const std::vector<Item> &items,
                  float camX, float camZ, float dx, float dz,
                  const RenderAssets &r);
#endif