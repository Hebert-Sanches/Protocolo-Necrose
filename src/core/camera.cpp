#include "core/camera.h"

float camX = 0.0f;
float camY = 1.5f;
float camZ = 25.0f;

float yaw = 0.0f;
float pitch = 0.0f;

void setCamera(float x, float y, float z, float newYaw, float newPitch)
{
    camX = x;
    camY = y;
    camZ = z;
    yaw = newYaw;
    pitch = newPitch;
}
