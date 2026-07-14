# 01 - Bootstrap do casco (The-Forge + cengine 0.8.0 + common 0.3.0)

- **Status:** in-progress (build verde; falta a validacao jogavel)
- **Prioridade:** alta - nada existe sem o casco.
- **Categoria:** Plataforma
- **Depende de:** checkouts irmaos The-Forge (Unit_Tests buildada), cengine
  >= 0.8.0, platform-theforge-common >= 0.3.0.

## Contexto

O quarto jogo nasce com o ecossistema ja maduro: `EngineManager::owned()`,
`FlowRouter`, `collision2d` e — a novidade que ele existe para validar — a
**porta de input** (`cengine::input`, task 20 da engine).

## Objetivo

Janela abrindo com o fluxo splash -> menu -> game(placeholder) -> exit rodando
de ponta a ponta, dirigido pela cengine em modo proprio.

## Escopo

1. Dominio minimo: `GameRouter` (FlowRouter + menu/game/exit), `StateGameFlow`,
   estados initial/menu/game/exit (`exit` casa com `kExitStateCode`).
2. Cenas Forge: splash, menu (JOGAR/SAIR), game placeholder (raquete de texto),
   exit.
3. `main_theforge.cpp`: subsistemas de processo + composicao + `owned()`.
4. `BreakoutForge.vcxproj/.sln` consumindo os tres repos irmaos.

## Decisoes

**As cenas recebem `cengine::input::Keyboard&` INJETADO** — nao chamam as
funcoes globais do `forgeui`. E a diferenca em relacao aos tres jogos
anteriores, e o ultimo criterio de aceite da task 20 da cengine: uma cena
consumindo a PORTA, e nao a ponte.

A instancia e a do casco (`forgeui::keyboard()`), porque quem CAPTURA continua
sendo a plataforma — o WndProc traduzindo `VK_*` para `Key` e empurrando para
dentro do teclado da porta. A engine ganhou o contrato; nao ganhou (e nao vai
ganhar) o teclado fisico.

**O desenho continua vindo do `forgeui` global.** Nao existe porta de DESENHO na
cengine, e nao ha evidencia que justifique invento-la agora — seria exatamente a
especulacao que a ADR 0002 proibe. Input subiu porque tinha quatro copias; o
desenho tem uma ponte so, viva e sem duplicacao.

**Os dois batchers nascem DESLIGADOS.** O casco e so de texto. O `forgesprite`
liga na task 03 — e este jogo e quem o traz de volta a vida (hoje nenhum jogo
vivo o compila).

## Criterios de Aceite

- [x] Build MSBuild verde (Release|x64) com os tres repos irmaos.
- [x] Cenas consumindo a porta de input da cengine (nao as globais do forgeui).
- [x] Nenhum include do The-Forge em `src/breakout/` (dominio puro).
- [ ] Fluxo completo jogavel: splash -> menu -> game -> menu -> exit, janela
      fecha limpa.
- [ ] Raquete anda com as setas SEGURADAS (o estado continuo da porta validado)
      e o menu NAO varre varios itens com a seta segurada (a fila de edges
      validada — um evento por input()).
