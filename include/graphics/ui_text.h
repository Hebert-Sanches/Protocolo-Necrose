#pragma once

void uiDrawStrokeText(float x, float y, const char* text, float scale);
float uiStrokeTextWidth(const char* text); // largura "crua" (sem scale)
float uiStrokeTextWidthScaled(const char* text, float scale);
