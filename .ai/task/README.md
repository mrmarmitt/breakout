# Plano de trabalho - Breakout

Quarto jogo do ecossistema cengine, e o consumidor de validacao da vez. Ele foi
escolhido por um motivo especifico, alem de ser um jogo: **duas pecas das libs
nao tem NENHUM consumidor vivo hoje**, e isso e como codigo morto — apodrece
calado.

- o `forgesprite` (batcher de sprites do common) nasceu no spaceinvaders, que
  esta congelado; o asteroids desligou e foi de wireframe. Nenhum jogo vivo o
  compila.
- o **AABB** do `cengine::collision2d` foi promovido com evidencia do
  spaceinvaders (congelado); o consumidor vivo, o asteroids, usa a metade do
  circulo.

O Breakout e feito de retangulos e sprites. Ele traz os dois de volta a vida.

## Indice

| # | Task | Status | Categoria |
|---|------|--------|-----------|
| 01 | [Bootstrap do casco](01-bootstrap-casco.md) | done | Plataforma |
| 02 | [Dominio: raquete, bola e tijolos](02-dominio-raquete-bola-tijolos.md) | done | Dominio |
| 03 | [Sprites](03-sprites.md) | done | Plataforma |
| 04 | [Vidas, pontuacao e fases](04-vidas-pontuacao-fases.md) | done | Dominio + Fluxo |
| 05 | [Pausa](05-pausa.md) | done | Plataforma |
| 06 | [Audio](06-audio.md) | in-progress | Plataforma |

## Backlog (tasks a abrir quando chegarem)

- **07 — Recordes** (politica do jogo, padrao TSV dos irmaos).

## Regra pratica

Dominio em `src/breakout/` sem nenhum include do The-Forge; plataforma em
`src/platform/theforge/`. Toda mexida na cengine ou no common e validada por
este jogo antes de taggear.
