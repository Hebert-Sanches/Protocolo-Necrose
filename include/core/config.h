#pragma once

namespace GameConfig
{
    constexpr float TILE_SIZE = 4.0f;
    constexpr float PLAYER_EYE_Y = 1.5f;
    constexpr int TIMER_MS = 16; // ~60fps

    // fração do tile usada como raio do jogador (colisão)
    constexpr float PLAYER_RADIUS_FACTOR = 0.35f;
    constexpr float PLAYER_STEPS = 0.15f;
}
