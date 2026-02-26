#pragma once

enum class GameState
{
    MENU_INICIAL,
    JOGANDO,
    PAUSADO,
    GAME_OVER
};

enum class WeaponState
{
    W_IDLE,
    W_FIRE_1,
    W_FIRE_2,
    W_RETURN,
    W_PUMP,
    W_RELOAD_1,
    W_RELOAD_2,
    W_RELOAD_3
};
