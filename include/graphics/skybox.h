#pragma once
#include <GL/glew.h>

struct RenderAssets; // forward decl (vem de core/game_state.h)

void drawSkydome(float camX, float camY, float camZ, const RenderAssets &r);