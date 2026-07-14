#pragma once

#include "ForgeSpriteUi.h"

// As regioes do atlas do Breakout, em PIXELS. Esta tabela e do JOGO — o common
// so conhece `SpriteRegion` (um retangulo), nunca "tijolo" ou "raquete": nome de
// coisa do jogo e vocabulario de jogo, e nao sobe para a lib.
//
// Casadas com `tools/make-atlas-dds.ps1` (atlas 32x16). Mudou la, mude aqui.
//
// O atlas e BRANCO: a cor vem do tint por vertice (o jogo passa a cor por linha
// de tijolo). O relevo vem do ALPHA da borda — assim o tint continua mandando na
// cor, e o bisel aparece de graca em qualquer cor.
namespace sprites {

inline constexpr forgesprite::SpriteRegion kBrick = { 0.0f, 0.0f, 16.0f, 8.0f };
inline constexpr forgesprite::SpriteRegion kPaddle = { 16.0f, 0.0f, 16.0f, 4.0f };
inline constexpr forgesprite::SpriteRegion kBall = { 16.0f, 8.0f, 4.0f, 4.0f };

} // namespace sprites
