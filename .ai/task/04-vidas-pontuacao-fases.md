# 04 - Vidas, pontuacao e fases

- **Status:** in-progress (suite verde; falta a validacao jogavel)
- **Prioridade:** alta - e o que transforma a arena numa PARTIDA.
- **Categoria:** Dominio + Fluxo
- **Depende de:** task 02 (o World).

## Objetivo

Uma partida com comeco, meio e fim: pontuar por tijolo, perder vidas quando a
bola cai, virar a fase quando a parede acaba, e o gameOver quando as vidas
acabam.

## Escopo

1. `brk::World`: `score()`, `lives()`, `level()`, `outcome()` (Playing/GameOver),
   `ballSpeed()`. O `ballsLost()` foi APOSENTADO — era andaime da task 02.
2. Fluxo: `gameOver` entra no `StateGameFlow`/`GameRouter`/`StateGame`.
   `game -> gameOver -> menu`, e `gameOver -> game` (jogar de novo).
3. `PlaySession` (padrao dos irmaos) + `ForgeGameOverScene`.
4. HUD: pontos, vidas (em raquetes) e fase.

## Decisoes

**Pontos por LINHA, e as de cima valem mais** (7/7/4/4/1/1, valores do arcade):
sao as mais dificeis de alcancar, porque para chegar la a bola tem de furar as
de baixo. A pontuacao ja embute a dificuldade, sem nenhuma regra a mais.

**A bola acelera — de dois jeitos, e os dois com TETO.** A cada 8 tijolos
quebrados ela ganha 4%, e cada fase nova comeca 8% mais rapida. E a tensao que o
arcade cria sem mudar nenhuma regra: o mesmo jogo, so que cada vez mais dificil
de acompanhar. O teto (`kMaxSpeedFactor`) existe porque sem ele a bola vira um
raio e o jogo, um sorteio.

**A bola em VOO tambem acelera** quando o limiar cruza: mesma direcao, novo
modulo. Se so a proxima bola acelerasse, o jogador sentiria um solavanco no
saque em vez de uma escalada.

**Fase nova comeca no SAQUE, nao em movimento**: a parede volta inteira e a bola
volta presa a raquete. Quem escolhe a hora de recomecar e o jogador.

**O World constata; o fluxo decide.** O `World` nao conhece cena nem router: ele
expoe `outcome() == GameOver`. Quem le isso, publica o resultado no
`PlaySession` e roteia e a CENA — e por isso a suite dirige o World sem router
nenhum.

## Nota de teste

O teste da aceleracao precisou de um **jogador de mentira** (`autoPlay`: saca e
persegue a bola com a raquete). Sem ele, a bola caia no primeiro retorno, a
partida acabava e a aceleracao — que so o IMPACTO dispara — nunca acontecia. O
teste falhou primeiro por isso, e o `autoPlay` acabou virando um mini teste de
integracao do jogo inteiro.

## Criterios de Aceite

- [x] Suite CMake verde (31 testes; 8 novos: placar inicial, pontos por linha,
      rebote nao pontua, aceleracao por acertos com teto, fase nova mais rapida,
      teto de fase, fim das vidas, gameOver congela).
- [x] Build MSBuild verde (Release|x64), sem warnings.
- [x] `ballsLost()` removido do World (o andaime da task 02 saiu).
- [ ] Validacao jogavel: HUD com pontos/vidas/fase; perder 3 bolas leva ao game
      over com o placar certo; limpar a parede vira a fase e a bola fica mais
      rapida; ENTER joga de novo (partida zerada).
