#pragma once

#include <cstdint>
#include <ctime>
#include <string>
#include <utility>

// Recorde de uma partida (nome, pontuacao, fase alcancada, data).
//
// Isto e POLITICA DO JOGO e fica aqui, nao na cengine: o que e um recorde, qual
// metrica ranqueia, quantos guardar e onde persistir sao decisoes deste jogo
// (ADR 0002 da engine — o veredito para recordes foi explicitamente "fica nos
// jogos", e a duplicacao entre eles e o custo aceito).

namespace brk {

class Record
{
    std::string m_name;
    int         m_score = 0;
    uint32_t    m_level = 1;
    std::string m_playedAt;

public:
    Record() = default;

    Record(std::string name, const int score, const uint32_t level, std::string playedAt):
        m_name(std::move(name)), m_score(score), m_level(level), m_playedAt(std::move(playedAt))
    {
    }

    [[nodiscard]] const std::string& name() const { return m_name; }
    [[nodiscard]] int                score() const { return m_score; }
    [[nodiscard]] uint32_t           level() const { return m_level; }
    [[nodiscard]] const std::string& playedAt() const { return m_playedAt; }

    [[nodiscard]] bool scoresHigherThan(const Record& other) const { return m_score > other.m_score; }
};

/// Data/hora local "YYYY-MM-DD HH:MM" para carimbar o recorde.
inline std::string nowAsString()
{
    const std::time_t now = std::time(nullptr);
    std::tm           tm = {};
#if defined(_WIN32)
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &tm);
    return buffer;
}

} // namespace brk
