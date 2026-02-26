#pragma once
#include <GL/glew.h>
#include <vector>
#include <cstdlib>
#include <cmath>

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float life;
    float maxLife;
};

class ParticleSystem {
public:
    std::vector<Particle> particles;

    void spawnBlood(float startX, float startY, float startZ, int count) {
        for (int i = 0; i < count; ++i) {
            Particle p;
            p.x = startX;
            p.y = startY; 
            p.z = startZ;
            
            p.vx = ((rand() % 100) / 50.0f - 1.0f) * 6.0f; 
            p.vy = ((rand() % 100) / 100.0f) * 5.0f + 2.0f; 
            p.vz = ((rand() % 100) / 50.0f - 1.0f) * 6.0f;
            
            p.maxLife = ((rand() % 100) / 100.0f) * 1.5f + 1.0f; 
            p.life = p.maxLife;
            
            particles.push_back(p);
        }
    }

    void update(float dt) {
        for (auto it = particles.begin(); it != particles.end(); ) {
            it->life -= dt;
            if (it->life <= 0.0f) {
                it = particles.erase(it);
            } else {
                it->vy -= 9.8f * dt; // Gravidade
                it->x += it->vx * dt;
                it->y += it->vy * dt;
                it->z += it->vz * dt;
                
                if (it->y < 0.1f) { // Chão
                    it->y = 0.1f;
                    it->vx = 0.0f;
                    it->vz = 0.0f;
                }
                ++it;
            }
        }
    }

    void draw(float camX, float camZ) {
        if (particles.empty()) return;

        glPushAttrib(GL_ALL_ATTRIB_BITS);

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0); 
        glDisable(GL_CULL_FACE);         
        glDepthMask(GL_FALSE);           

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (const auto& p : particles) {
            float alpha = p.life / p.maxLife; 
            glColor4f(1.0f, 0.0f, 0.0f, alpha); 

            glPushMatrix();
            glTranslatef(p.x, p.y, p.z);
            
            float ddx = camX - p.x;
            float ddz = camZ - p.z;
            float angle = std::atan2(ddx, ddz) * 180.0f / 3.14159f;
            glRotatef(angle, 0.0f, 1.0f, 0.0f);

            float s = 0.06f; 
            glBegin(GL_QUADS);
            glVertex3f(-s, -s, 0.0f);
            glVertex3f( s, -s, 0.0f);
            glVertex3f( s,  s, 0.0f);
            glVertex3f(-s,  s, 0.0f);
            glEnd();

            glPopMatrix();
        }

        glPopAttrib(); 
    }
}; 

extern ParticleSystem gBloodParticles;