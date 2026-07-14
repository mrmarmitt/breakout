# 05 - Pausa

- **Status:** in-progress (build verde; falta a validacao jogavel)
- **Prioridade:** media - conforto de jogo; e o gatilho do gate da task 18 da
  cengine.
- **Categoria:** Plataforma (cena)
- **Depende de:** task 04.
- **Dispara:** o gate da task 18 da cengine (scene stack / overlays) — avaliado
  e REPROVADO; ver Decisoes.

## Objetivo

ESC pausa a partida: o jogo congela onde estava (a bola no ar), aparece um painel
por cima, e ENTER continua de onde parou. ESC de novo abandona para o menu.

## Decisoes

**O gate da task 18 da cengine foi avaliado e REPROVADO — a pausa fica no jogo.**
Dois motivos:

1. **Uma evidencia nao sao duas** (criterio 2 da ADR 0002). Nenhum outro jogo do
   ecossistema tem pausa, modal ou overlay — nem os congelados. A contagem era
   ZERO antes deste jogo, e passou a UM. O criterio pede dois.
2. **O caso custou 10 linhas aqui dentro** — e a propria task 18 mandava fazer
   assim ("se o caso for simples e exclusivo de um jogo, preferir implementar
   dentro do proprio jogo primeiro").

**Pausar nao e navegar.** A tentacao era criar um estado `paused` no router. Seria
errado: o router DESCARREGA a cena atual no commit da navegacao — a cena do jogo
morreria, e com ela o `World`. A partida acabaria em vez de pausar. Pausa e um
overlay DA CENA, e por isso mora nela: um `bool m_paused`.

E as tres politicas do `SceneLayerPolicy` que a task 18 desenha saem de graca
quando a camada de baixo E VOCE MESMO:

| Politica do desenho da engine | Como fica aqui |
|---|---|
| `blocksInputBelow = true` | o `input()` trata a pausa e retorna |
| `updatesBelow = false` | early-return no `update()` (o World congela) |
| `drawsBelow = true` | desenhar o jogo ANTES do veu |

**O veu e um sprite.** A tela escurecida e o sprite da bola (solido no atlas)
esticado sobre a tela inteira com tint preto e alpha parcial — o `drawSpriteRect`
do common 0.4.0 servindo para algo que nao e um corpo do jogo.

## Fora do Escopo

- `SceneStack` na cengine (ver acima).
- Pausa que continua rodando alguma coisa (debug overlay) — nao ha caso.

## Nota de teste

Nao ha teste novo de dominio: a pausa NAO toca no `World` (e por isso ela e
correta — congelar o jogo e nao atualizar a simulacao, nao mexer nela). A
validacao e jogavel.

## Criterios de Aceite

- [x] Build MSBuild verde (Release|x64).
- [x] O `World` nao mudou (a suite segue verde, 31 testes).
- [x] Gate da task 18 avaliado e documentado nos dois repos (evidencia 1/2).
- [ ] Validacao jogavel: ESC pausa com a bola no ar; o jogo aparece congelado
      atras do veu; ENTER continua de onde parou (a bola segue na mesma
      direcao); ESC abandona para o menu.
