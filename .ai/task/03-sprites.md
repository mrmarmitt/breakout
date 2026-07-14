# 03 - Sprites: o forgesprite volta a ter consumidor vivo

- **Status:** done (2026-07-14 — validado jogando pelo usuario)
- **Prioridade:** media - o jogo funciona sem isto, mas a lib nao.
- **Categoria:** Plataforma
- **Depende de:** task 02 (o World), platform-theforge-common >= 0.4.0.

## Contexto

O `forgesprite` nasceu na PoC do spaceinvaders e, desde entao, **nenhum jogo vivo
o compilava**: o spaceinvaders congelou (ADR 0003) e o asteroids foi de wireframe
(task 06 de la). Um batcher de sprites inteiro — textura, sampler, pipeline,
vertex buffer dinamico — que ninguem exercitava. Isso e codigo morto com aparencia
de codigo vivo.

O Breakout e feito de retangulos coloridos. Ele e o consumidor natural.

## Escopo

1. `tools/make-atlas-dds.ps1` + `assets/textures/atlas.dds`: atlas 32x16 escrito
   byte a byte (DDS RGBA8 sem compressao, header DX10, lido direto pelo tinydds
   do ResourceLoader) — mesma tecnica do spaceinvaders, sem ferramenta externa.
2. `BreakoutSprites.h`: a tabela de regioes **do jogo**.
3. `main_theforge.cpp`: `windowDesc.sprites.atlasPath = "atlas.dds"`.
4. `ForgeGameScene`: tijolos, raquete e bola desenhados pelo batcher.

## Decisoes

**A escala uniforme do batcher nao servia — e isso virou o common 0.4.0.** O
`drawSprite(region, x, y, ESCALA, cor)` foi moldado pelo spaceinvaders, onde o
sprite era 1:1 com as unidades do mundo. Aqui os corpos tem proporcoes DIFERENTES
entre si (tijolo 60x20, raquete 110x16, bola 12x12) e a projecao arena->tela
estica X e Y de forma diferente (800x600 numa janela 1280x720). Nasceu o
`drawSpriteRect(region, x, y, w, h, cor)`: retangulo de destino arbitrario. O
quad ja era montado a partir de dois cantos — a funcao nova so para de derivar o
segundo canto de uma escala.

**A tabela de regioes e do JOGO, nao da lib.** O common conhece `SpriteRegion`
(um retangulo); "tijolo" e "raquete" sao vocabulario de jogo e ficam aqui
(mesma regra da ADR 0002, aplicada a ponte de plataforma).

**O atlas e BRANCO; a cor vem do tint.** Cada linha de tijolo tem sua cor, e ela
e passada por vertice — como nos arcades com overlay de cor. O relevo (bisel) vem
do ALPHA da borda, nao da cor: assim o tint continua mandando, e o bisel aparece
de graca em qualquer cor.

## Criterios de Aceite

- [x] Atlas gerado pelo script do repo (sem ferramenta externa) e carregado sem
      erro no log.
- [x] Build MSBuild verde (Release|x64).
- [x] `forgesprite` com consumidor VIVO de novo.
- [x] O asteroids (outro consumidor vivo do common) segue compilando com a 0.4.0.
- [x] Validacao jogavel: tijolos, raquete e bola como sprites; cores por linha;
      HUD por cima do jogo. (2026-07-14.)
