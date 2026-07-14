#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

// Audio do Breakout — XAudio2 (nativo do Windows SDK), com os sons SINTETIZADOS
// na inicializacao. Nao ha arquivo de audio no repo: as ondas sao geradas por
// codigo, no mesmo espirito do atlas de sprites (tools/make-atlas-dds.ps1).
//
// POR QUE ISTO MORA NO JOGO, e nao na cengine nem no platform-theforge-common:
//
// - a cengine e mecanismo puro sem plataforma; audio precisa de device, e um
//   `IAudioPlayer` na engine hoje teria ZERO evidencia de necessidade (nenhum
//   outro jogo do ecossistema tem som) — seria a especulacao que a ADR 0002
//   proibe;
// - o platform-theforge-common e explicitamente "coisas do The-Forge", e o
//   The-Forge NAO tem modulo de audio. Audio nao e assunto dele.
//
// Quando um SEGUNDO jogo precisar de som, havera duas evidencias e a ponte podera
// subir — provavelmente como porta da cengine (mecanismo: `play(id)`) com o
// backend na plataforma. Ate la, fica aqui, e a copia e o custo aceito.
//
// O que o jogo NAO decide: como cada evento SOA. Isso e apresentacao, e mora na
// cena, que traduz `brk::Events` em chamadas de `play()`.

enum class Sound : uint8_t
{
    PaddleHit,   // a bola volta da raquete: o "toc" grave
    WallBounce,  // parede lateral/teto: mais seco e mais agudo
    BrickBreak,  // tijolo quebrado: o "blip" curto do arcade
    LifeLost,    // a bola caiu: uma queda descendente
    LevelUp,     // parede limpa: um arpejo curto
    Count,
};

// PCM 16-bit mono. Um formato so para todas as vozes — e o que permite um pool
// unico de vozes reaproveitaveis.
class AudioPlayer
{
public:
    static constexpr uint32_t kSampleRate = 44100;

    /// Vozes simultaneas. Poucas de proposito: quando estouram, a mais antiga e
    /// reciclada — num jogo de arcade, um som novo importa mais que um som velho
    /// terminando.
    static constexpr size_t kVoiceCount = 8;

    AudioPlayer() = default;
    ~AudioPlayer();

    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;

    /// Abre o device e sintetiza os sons. Falhar NAO e fatal: sem placa de som,
    /// o jogo roda mudo (retorna false, e todo `play()` vira no-op).
    bool init();
    void shutdown();

    void play(Sound sound);

    /// HRESULT do ultimo erro (0 se nao houve). Este arquivo nao loga — ele nao
    /// conhece o ILog do The-Forge, e nao pode: os headers dos dois brigam
    /// (`_WIN32_WINNT`). Quem loga e o composition root.
    [[nodiscard]] uint32_t lastError() const { return m_lastError; }

private:
    struct Impl;
    Impl* m_impl = nullptr; // pimpl: os headers do XAudio2 nao vazam para as cenas

    bool     m_ownsCom = false; // fomos nos que inicializamos o COM desta thread?
    uint32_t m_lastError = 0;

    std::vector<int16_t> m_samples[static_cast<size_t>(Sound::Count)];
};
