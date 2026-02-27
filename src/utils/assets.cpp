#include "utils/assets.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include <cstdio>

bool loadAssets(GameAssets &a)
{
    a.texMenuBG = carregaTextura("assets/menu_bg.png");

    a.texChao = carregaTextura("assets/chao_sujo.png");
    a.texParede = carregaTextura("assets/parede_suja.png");
    a.texSangue = carregaTextura("assets/016.png");
    a.texLava = carregaTextura("assets/179.png");
    a.texChaoInterno = carregaTextura("assets/chao_sujo.png"); 
    a.texParedeInterna = carregaTextura("assets/parede_suja.png"); 
    a.texTeto = carregaTextura("assets/teto_sujo.png");

    // Compilando os shaders
    a.progSangue = criaShader("shaders/blood.vert", "shaders/blood.frag");
    a.progLava = criaShader("shaders/lava.vert", "shaders/lava.frag");
    a.progMelt = criaShader("shaders/melt.vert", "shaders/melt.frag");
    a.progDano = criaShader("shaders/damage.vert", "shaders/damage.frag");

    // --- TODOS OS INIMIGOS AGORA SÃO ZUMBIS ---
    for (int i = 0; i < 5; i++)
    {
        a.texEnemies[i] = carregaTextura("assets/enemies/zumbi_base.png");
        a.texEnemiesRage[i] = carregaTextura("assets/enemies/zumbi_ataque.png");
        a.texEnemiesDamage[i] = carregaTextura("assets/enemies/zumbi_dano.png");

        a.texEnemies[0]       = carregaTextura("assets/enemies/zumbi_j_base.png");
        a.texEnemiesRage[0]   = carregaTextura("assets/enemies/zumbi_j_ataque.png");
        a.texEnemiesDamage[0] = carregaTextura("assets/enemies/zumbi_j_dano.png");

        a.texEnemies[1]       = carregaTextura("assets/enemies/zumbi_t_base.png");
        a.texEnemiesRage[1]   = carregaTextura("assets/enemies/zumbi_t_ataque.png");
        a.texEnemiesDamage[1] = carregaTextura("assets/enemies/zumbi_t_dano.png");

        a.texEnemies[2]       = carregaTextura("assets/enemies/zumbi_m_base.png");
        a.texEnemiesRage[2]   = carregaTextura("assets/enemies/zumbi_m_ataque.png");
        a.texEnemiesDamage[2] = carregaTextura("assets/enemies/zumbi_m_dano.png");

        a.texEnemies[3]       = carregaTextura("assets/enemies/zumbi_k_base.png");
        a.texEnemiesRage[3]   = carregaTextura("assets/enemies/zumbi_k_ataque.png");
        a.texEnemiesDamage[3] = carregaTextura("assets/enemies/zumbi_k_dano.png");
    }

    a.texHealthOverlay = carregaTextura("assets/heal.png");
    a.texGunDefault = carregaTextura("assets/gun_default.png");
    a.texGunFire1 = carregaTextura("assets/gun_fire1.png");
    a.texGunFire2 = carregaTextura("assets/gun_fire2.png");
    a.texGunReload1 = carregaTextura("assets/gun_reload1.png");
    a.texGunReload2 = carregaTextura("assets/gun_reload2.png");
    a.texDamage = carregaTextura("assets/damage.png");

    a.texHealth = carregaTextura("assets/health.png");
    a.texAmmo = carregaTextura("assets/066.png");
    a.texCartao = carregaTextura("assets/cartao.png");

    a.texSkydome = carregaTextura("assets/Va4wUMQ.png");
    a.texPorta = carregaTextura("assets/Porta.png");

    a.texGunHUD = carregaTextura("assets/Shotgun.png");
    a.texHudFundo = carregaTextura("assets/088.png");

// ---> CORREÇÃO: Tiramos a neblina e adicionamos o progDano e progMelt
    if (!a.texChao || !a.texParede || !a.texSangue || !a.texLava || !a.progSangue ||
        !a.progLava || !a.progMelt || !a.progDano || !a.texHealth || !a.texGunDefault || !a.texGunFire1 ||
        !a.texGunFire2 || !a.texSkydome || !a.texGunReload1 || !a.texGunReload2 ||
        !a.texDamage || !a.texAmmo || !a.texHealthOverlay || !a.texEnemies[0] ||
        !a.texEnemiesRage[0] || !a.texEnemiesDamage[0] || !a.texEnemies[1] ||
        !a.texEnemiesRage[1] || !a.texEnemiesDamage[1] || !a.texEnemies[2] ||
        !a.texEnemiesRage[2] || !a.texEnemiesDamage[2] || !a.texGunHUD || !a.texHudFundo || !a.texMenuBG)
    {
        std::printf("ERRO: falha ao carregar algum asset (textura/shader).\n");
        return false;
    }
    return true;
}