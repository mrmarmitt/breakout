# Breakout

Quarto jogo de estudo sobre a [cengine](https://github.com/mrmarmitt/cengine),
com a plataforma The-Forge via
[platform-theforge-common](https://github.com/mrmarmitt/platform-theforge-common).

Alem de ser um jogo, ele existe para **trazer de volta a vida duas pecas das
libs que hoje nao tem nenhum consumidor vivo** — e codigo de lib sem consumidor
vivo apodrece calado:

- o **`forgesprite`** (batcher de sprites) nasceu no spaceinvaders, que esta
  congelado; o asteroids foi de wireframe. Nenhum jogo vivo o compila hoje.
- o **AABB** do `cengine::collision2d` foi promovido com evidencia do
  spaceinvaders (congelado); o unico consumidor vivo usa a metade do circulo.

Breakout e feito de retangulos e sprites. Ele usa os dois.

E ele e o primeiro jogo cujas **cenas consomem a porta de input da cengine**
(`cengine::input::Keyboard&` injetado, task 20 da engine) em vez das funcoes
globais da ponte de plataforma.

## Estrutura

```
src/main_theforge.cpp               entry point + composition root
src/breakout/game/                  dominio (C++ puro, sem The-Forge)
  GameRouter.h                      fachada de navegacao (FlowRouter + vocabulario)
  state/StateGameFlow.h             maquina de fluxo (transicoes por estado)
  state/StateGame.h                 estados concretos (initial/menu/game/exit)
src/platform/theforge/
  PC_VS2019/BreakoutForge.sln       build MSBuild (cadeia do The-Forge)
  src/BreakoutForge/                cenas + factory + PathStatement + gpu.cfg
```

## Build

Pre-requisitos (checkouts irmaos na mesma pasta): `The-Forge` com a solution
`Examples_3/Unit_Tests` buildada **na mesma Configuration**, `cengine` >= 0.8.0
e `platform-theforge-common` >= 0.3.0.

```
msbuild src\platform\theforge\PC_VS2019\BreakoutForge.sln /p:Configuration=Release /p:Platform=x64
out\theforge\x64\Release\BreakoutForge\BreakoutForge.exe
```

> A Configuration tem de existir no The-Forge. Nesta maquina so a **Release**
> esta buildada — dai o exemplo usar Release.

## Controles (casco atual)

- Splash: ENTER entra no menu.
- Menu: SETAS navegam, ENTER confirma, ESC sai.
- Jogo (placeholder do casco): SETAS `<-` `->` seguradas movem a raquete, ESC
  volta ao menu.

## Plano

O plano de desenvolvimento vive em [`.ai/task/`](.ai/task/), um degrau validado
por vez, comecando pelo [bootstrap do casco](.ai/task/01-bootstrap-casco.md).
