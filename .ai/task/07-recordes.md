# 07 - Recordes

- **Status:** in-progress (suite verde; falta a validacao jogavel)
- **Prioridade:** media - fecha o ciclo da partida (jogar -> perder -> se ver na
  tabela -> querer de novo). E a ultima task do jogo.
- **Categoria:** Politica do jogo
- **Depende de:** task 04 (o `PlaySession` ja carrega score + fase).

## Objetivo

Top-10 persistido: ao acabar a partida, se a pontuacao entra na tabela, o jogo
pede o nome e grava; a tabela e vista pelo menu ou logo apos gravar.

## Escopo

1. Dominio: `Record` (nome, pontos, fase, data), `RecordRepository` (porta),
   `FileRecordRepository` (TSV) e `RecordService` (as REGRAS: ranking, corte do
   top-10, `isNewRecord`, `sanitizeName`).
2. Fluxo: `records` entra no `StateGameFlow`/`GameRouter`/`StateGame`.
3. Cenas: `ForgeRecordsScene` (a tabela) e a entrada de nome no
   `ForgeGameOverScene`. Menu ganha a opcao RECORDES.

## Decisoes

Nenhuma nova — e esse e o ponto. Este e o **quarto** jogo a implementar recordes,
e a ADR 0002 ja tinha dado o veredito: o que e um recorde, qual metrica ranqueia,
quantos guardar e onde persistir sao decisoes DO JOGO. A duplicacao entre os
jogos e o custo explicitamente aceito.

O desenho e o mesmo do asteroids, com a metrica deste jogo (pontos + fase, em vez
de pontos + onda), e carrega as mesmas tres regras que o spaceinvaders nao tinha:

- **o corte do top-N acontece AO SALVAR**, nao so ao exibir (la o arquivo crescia
  para sempre);
- **a leitura NORMALIZA** (ordena + corta), para sobreviver a um arquivo editado
  a mao;
- **placar nao vale um crash**: sem arquivo a tabela nasce vazia, sem permissao de
  escrita o recorde se perde e o jogo segue, e uma linha corrompida e ignorada
  sem levar as boas junto.

## Criterios de Aceite

- [x] Suite CMake verde (52 testes; 15 novos: regras sem disco + o TSV em disco).
- [x] Build MSBuild verde (Release|x64), sem warnings.
- [x] Um `records.tsv` existente NAO e destruido ao abrir o jogo (conferido no
      smoke test).
- [ ] Validacao jogavel: fazer pontos, morrer, ver NOVO RECORDE, digitar o nome,
      ver a tabela; abrir RECORDES pelo menu; fechar e reabrir o jogo e a tabela
      continuar la.
