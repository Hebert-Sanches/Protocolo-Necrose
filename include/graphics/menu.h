#pragma once

#include <string>

struct RenderAssets; // forward decl (vem de core/game_state.h)

void menuRender(int screenW, int screenH, float tempo, const std::string &title, const std::string &subTitle, const RenderAssets &a);

void pauseMenuRender(int screenW, int screenH, float tempo);

void menuMeltRequestStart();
bool menuMeltIsActive();

void menuMeltRenderOverlay(int screenW, int screenH, float tempo);

