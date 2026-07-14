# 06 - Audio

- **Status:** done (2026-07-14 — validado ouvindo pelo usuario)
- **Prioridade:** media - conforto; e a **borda de conhecimento nova** do
  ecossistema (nenhum jogo tinha som).
- **Categoria:** Plataforma (jogo)
- **Depende de:** task 04.

## Objetivo

O jogo soa: a bola bate na raquete, quebra tijolo, ricocheteia na parede, cai
(vida perdida) e a fase vira.

## Decisoes

**O audio fica DENTRO do jogo — nao sobe para lib nenhuma.**

- A **cengine** e mecanismo puro. Uma porta `IAudioPlayer` hoje teria **zero**
  evidencia de necessidade: nenhum outro jogo do ecossistema tem som. Seria a
  especulacao que a ADR 0002 proibe — e eu estaria adivinhando a forma da ponte
  sem nenhum consumidor real para me corrigir.
- O **platform-theforge-common** e explicitamente "coisas do The-Forge", e o
  The-Forge **nao tem modulo de audio** (conferido: nao ha nada de som em
  `Common_3`). Audio nao e assunto dele.

Quando um SEGUNDO jogo precisar de som, havera duas evidencias e a ponte podera
subir — provavelmente como porta da cengine (`play(id)`) com o backend na
plataforma. Ate la, a copia e o custo aceito (corolario da ADR 0002).

**O World RELATA; a cena decide como soa.** O dominio ganhou `brk::Events` —
contadores do que aconteceu NESTE quadro (batidas na raquete, na parede, tijolos
quebrados, vida perdida, fase virada), zerados no comeco de cada `update`.

A alternativa seria a cena ADIVINHAR os eventos comparando o placar entre
quadros ("o score subiu, entao um tijolo quebrou"). Seria fragil e, pior, nao
distinguiria um tijolo de DOIS no mesmo quadro. Como os eventos sao contados, o
jogo soa como o que aconteceu, e nao como uma media.

E o dominio continua sem saber o que e um alto-falante: `Events` sao FATOS, nao
sons. Quem escolhe o timbre e a cena — e por isso os eventos sao testaveis sem
placa de som (6 testes novos).

**Sons SINTETIZADOS, nenhum arquivo de audio.** Ondas quadradas com envelope
exponencial, geradas em codigo na inicializacao — no mesmo espirito do atlas de
sprites (que tambem e gerado, byte a byte). Onda quadrada e a voz do arcade;
senoide soaria macia demais para um jogo feito de blocos.

**Backend: XAudio2 2.9**, que vem no proprio Windows SDK — sem redistribuivel,
sem terceiros no repo. Um pool de 8 vozes com o mesmo formato (PCM 16-bit mono):
sons simultaneos nao se atropelam, e quando o pool estoura a voz mais antiga e
reciclada (num arcade, o som NOVO importa mais que um som velho terminando).

**Sem audio, o jogo roda mudo** — e isso nao e um erro fatal. `init()` devolve
false e todo `play()` vira no-op.

## Aprendizado (custou dois builds vermelhos)

**Os headers do The-Forge e do XAudio2 brigam.** O `Config.h` do The-Forge fixa
`_WIN32_WINNT` em **Windows 7**; o `xaudio2.h` do SDK **recusa** qualquer alvo
anterior ao Windows 8 (a versao velha vivia no DirectX SDK). Incluir os dois no
mesmo arquivo da erro de compilacao — ou, se a ordem "der certo" por acaso, um
device que nao abre **em silencio**, que foi exatamente o que aconteceu na
primeira tentativa.

Solucao: o `AudioPlayer.cpp` e uma **ilha Win32 pura** — nenhum header do
The-Forge entra la. Ele nao loga: **devolve** o HRESULT, e quem loga e o
composition root. O acoplamento que nao existe e o que nao quebra.

(E o XAudio2 exige **COM inicializado** na thread; o The-Forge nao faz isso por
nos. Sem `CoInitializeEx`, o device falha calado.)

## Criterios de Aceite

- [x] Suite CMake verde (37 testes; 6 novos: eventos comecam vazios, parede,
      raquete, tijolo, vida/fase, e os eventos sao LIMPOS a cada quadro).
- [x] Build MSBuild verde (Release|x64), sem warnings.
- [x] Nenhum arquivo de audio no repo (sons gerados em codigo).
- [x] Device de audio abrindo (log limpo); sem device, o jogo roda mudo.
- [x] Validacao OUVINDO: cada evento tem o seu som; dois tijolos no mesmo quadro
      soam duas vezes; a pausa nao toca nada (o World esta congelado).
      (2026-07-14 — os timbres sintetizados nao precisaram de ajuste.)
