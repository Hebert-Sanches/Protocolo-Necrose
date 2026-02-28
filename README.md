# Protocolo: Necrose
**Trabalho Final - Computação Gráfica**

Este é o nosso projeto final da disciplina de Computação Gráfica. Pegamos o motor base do jogo estilo "Doom" feito em aula e o transformamos em um jogo próprio com tema de sobrevivência/zumbi, adicionando novas mecânicas, fases e inimigos.

## 📌 O que fizemos no jogo
Cumprimos todos os requisitos pedidos no trabalho:
- **3 Fases Distintas:** O jogo progride do Nível 1 (Hospital) para o Nível 2 (Floresta - com neblina verde e skydome) e finaliza no Nível 3. A transição ocorre ao achar a porta de saída.
- **Identidade e Mecânicas:** Criamos diferentes tipos de zumbis. O Zumbi "M" é o único que dropa o cartão de acesso para passar de fase. O Boss "K" é mais forte e demora 60 segundos para renascer.
- **Gráficos e Câmera 3D:** Câmera controlada pelo mouse (pitch e yaw). Uso de texturas próprias e ordenação de profundidade (Z-buffer) para os sprites.
- **Efeitos e Shaders:** Implementamos sistema de partículas (sangue espirrando ao atirar) e Shaders (GLSL) que simulam lava, dano na tela do jogador e iluminação direcional.
- **Áudio 3D e Estados:** Sons de tiro, recarga e dano baseados na posição da câmera. Menus completos de Início, Pausa, Game Over e Vitória.

## 🎮 Controles
- **W, A, S, D:** Movimentação
- **Mouse:** Mira / Movimentação da câmera
- **Botão Esquerdo do Mouse:** Atirar
- **R:** Recarregar
- **P:** Pausar
- **ENTER:** Iniciar jogo / Reiniciar após Game Over
- **ESC:** Sair do jogo

## 🚀 Como Executar
O jogo já está compilado para Windows. Basta abrir a pasta raiz do projeto e executar o arquivo:
' g++ (Get-ChildItem -Recurse -Filter *.cpp).FullName -I . -I src -I include -o DoomLike.exe -lglew32 -lfreeglut -lopengl32 -lglu32 -lopenal '
' .\DoomLike.exe '


## 👥 Equipe
Hebert Vinícius
Alender Melo
Athirson Sá
Vinícius Nobre