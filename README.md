# Breakout

Quarto jogo de estudo sobre a [cengine](https://github.com/mrmarmitt/cengine),
com a plataforma The-Forge via
[platform-theforge-common](https://github.com/mrmarmitt/platform-theforge-common).

Alem de ser um jogo, ele nasceu para **trazer de volta a vida duas pecas das libs
que nao tinham nenhum consumidor vivo** — e codigo de lib sem consumidor vivo
apodrece calado:

- o **`forgesprite`** (batcher de sprites) nasceu no spaceinvaders, que esta
  congelado; o asteroids foi de wireframe. Nenhum jogo vivo o compilava.
- o **AABB** do `cengine::collision2d` foi promovido com evidencia do
  spaceinvaders (congelado); o unico consumidor vivo usava a metade do circulo.

Breakout e feito de retangulos e sprites — e a bola dele e QUADRADA, como no
arcade original. Ele usa os dois.

## O que este jogo devolveu para o ecossistema

- **Validou a porta de input** da cengine (`cengine::input`, task 20): e o
  primeiro jogo cujas cenas recebem `Keyboard&` INJETADO, em vez das funcoes
  globais da ponte de plataforma.
- **Forcou o `drawSpriteRect`** (common 0.4.0): a escala uniforme do batcher,
  moldada pelo spaceinvaders, nao servia a corpos de proporcoes diferentes.
- **Trouxe AUDIO** ao ecossistema pela primeira vez (sintetizado, sem arquivos).
- **Reprovou** a task 18 da engine (scene stack): a pausa custou 10 linhas dentro
  do jogo — nao paga uma abstracao nova.

## Estrutura

```
src/main_theforge.cpp                entry point + composition root
src/breakout/game/                   dominio (C++ puro, sem The-Forge)
  World.{h,cpp}                      raquete, bola, tijolos, colisao, partida
  Record.h + service/                recordes (politica do jogo, TSV)
  GameRouter.h + state/              fluxo (FlowRouter da cengine + vocabulario)
test/breakout/                       a suite do dominio (GoogleTest/CTest)
src/platform/theforge/
  PC_VS2019/BreakoutForge.sln        build MSBuild (cadeia do The-Forge)
  src/BreakoutForge/                 cenas, sprites, audio, PathStatement, gpu.cfg
tools/make-atlas-dds.ps1             gera o atlas (DDS escrito byte a byte)
```

O dominio nao inclui NADA do The-Forge: os testes exercitam o `World` do mesmo
jeito que a cena (comandos -> `update(dt fixo)` -> consultas), sem GPU e sem som.

## Build

### Dominio + testes (CMake)

```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

> **Nota MSYS2/MinGW:** os presets usam Ninja, e o `ninja` do MSYS2 roda os
> comandos pelo `/bin/sh`, que destroi caminhos nativos do Windows. Nessas
> maquinas, use um preset proprio com `MinGW Makefiles` no seu
> `CMakeUserPresets.json` (nao versionado) — mesma armadilha da cengine.

### Jogo (MSBuild, Windows)

Pre-requisitos (checkouts irmaos na mesma pasta): `The-Forge` com a solution
`Examples_3/Unit_Tests` buildada **na mesma Configuration**, `cengine` >= 0.8.0 e
`platform-theforge-common` >= 0.4.0.

```
msbuild src\platform\theforge\PC_VS2019\BreakoutForge.sln /p:Configuration=Release /p:Platform=x64
out\theforge\x64\Release\BreakoutForge\BreakoutForge.exe
```

O atlas e gerado por `powershell -File tools\make-atlas-dds.ps1` (ja versionado
em `assets/textures/atlas.dds`).

## Controles

- Splash: ENTER entra no menu.
- Menu: SETAS navegam, ENTER confirma, ESC sai (JOGAR / RECORDES / SAIR).
- Jogo: SETAS `<-` `->` movem a raquete, ESPACO saca, ESC pausa.
- Pausa: ENTER continua de onde parou, ESC abandona a partida.

A raquete devolve a bola em ANGULO conforme onde ela bate: centro devolve reto,
ponta devolve aberto. E dai que vem o controle.

## Plano

O plano vive em [`.ai/task/`](.ai/task/), um degrau validado por vez. Feito:
casco, dominio (raquete/bola/tijolos), sprites, partida (vidas/pontos/fases),
pausa, audio e recordes.
