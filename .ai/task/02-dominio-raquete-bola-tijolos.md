# 02 - Dominio: raquete, bola e tijolos

- **Status:** done (2026-07-14 — validado jogando pelo usuario)
- **Prioridade:** alta - e o jogo.
- **Categoria:** Dominio
- **Depende de:** task 01 (casco).
- **Fecha uma divida das libs:** o **AABB** da `cengine::collision2d` ganha o
  **primeiro consumidor VIVO** (a evidencia que o promoveu vinha do
  spaceinvaders, congelado).

## Objetivo

Uma partida de Breakout: a raquete anda, a bola saca, ricocheteia nas paredes e
na raquete, e quebra tijolos.

## Escopo

1. `brk::World` (C++ puro): raquete com limites, bola quadrada com saque,
   paredes, grade de 6x12 tijolos, e a colisao.
2. Deteccao pela `cengine::collision2d::intersects(Aabb, Aabb)`; a RESOLUCAO
   (refletir, matar o tijolo, perder a bola) e do jogo.
3. `CMakeLists` + suite (23 testes) linkando `cengine::collision2d`.
4. `ForgeGameScene` vira casca: le a porta de input, dirige o World, desenha as
   consultas.

## Fora do Escopo

- Vidas, pontuacao e fases (task 04). Por ora `ballsLost()` e o contador que
  PROVA que a bola pode ser perdida, sem inventar politica de partida.
- Sprites (task 03) — os retangulos sao fileiras de `#` por enquanto.

## Decisoes

**A bola e um QUADRADO, e isso e fidelidade.** No Breakout original a bola e um
blob retangular, nao um circulo. Entao AABB x AABB aqui nao e simplificacao — e
o formato certo. (Foi por isso que este jogo pode ser o consumidor vivo do AABB
sem forcar a barra.)

**A ENGINE detecta; o JOGO resolve.** `intersects()` responde "estes dois
retangulos se tocam?" e para por ai. Tudo o que vem depois e politica de
gameplay e mora no `World`:

- **a reflexao** e feita pelo eixo de MENOR penetracao (entrou 2 em Y e 18 em X?
  veio de cima) — geometria aplicada, decidida pelo jogo;
- **o rebote na raquete** NAO e um espelho: o angulo de saida depende de ONDE a
  bola bateu. Centro devolve reto; ponta devolve aberto, ate 60 graus. E isso
  que da CONTROLE ao jogador — sem isso o Breakout vira um pinball burro. A
  velocidade nao muda, so o angulo (senao o jogo acelera sozinho na ponta).
- **o fundo da arena nao e parede**: e por ali que a bola se perde. A unica
  borda que o jogo trata como derrota, e nao como geometria.

**Um tijolo por quadro.** Com dois vizinhos atingidos no mesmo passo, refletir
duas vezes empurraria a bola para dentro da parede. O corte esta no codigo e tem
teste (a bola encaixada no canto entre dois tijolos).

## Aprendizado (custou tres testes vermelhos)

**A bola nao cabe entre duas fileiras**: o vao e de 4 e ela tem 12. Encostar
numa linha e encostar na debaixo. Meus primeiros testes posicionavam a bola
"logo abaixo" de um tijolo e ela ja nascia dentro da fileira seguinte — os
testes estavam errados, o codigo estava certo. O teste de "tijolo morto nao
colide" agora abre uma COLUNA inteira e manda a bola pelo corredor.

## Criterios de Aceite

- [x] Suite CMake verde (23 testes: raquete/limites, saque, paredes, bola
      perdida no fundo, rebote por posicao na raquete com velocidade constante,
      grade, quebra de tijolo por baixo e por lado, tijolo morto, um por quadro).
- [x] Build MSBuild verde (Release|x64), sem warnings.
- [x] `src/breakout/` sem nenhum include do The-Forge.
- [x] O AABB da cengine tem consumidor VIVO.
- [x] Validacao jogavel: sacar, rebater, controlar o angulo pela posicao da
      raquete, quebrar a parede, perder a bola no fundo. (2026-07-14.)
