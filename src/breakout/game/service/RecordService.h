#pragma once

#include <memory>
#include <string>
#include <vector>

#include "breakout/game/Record.h"

namespace brk {

class RecordRepository;

// As REGRAS de recorde: ranking por pontuacao e corte do top-N. Politica do jogo
// (ADR 0002 — recordes ficam nos jogos, nunca sobem para a engine).
//
// O corte do top-N acontece TAMBEM ao salvar (como no asteroids, e diferente do
// spaceinvaders, cujo arquivo crescia para sempre): o que nao entra na tabela
// nao e persistido. E a leitura normaliza (ordena + corta), para sobreviver a um
// arquivo editado a mao.
class RecordService
{
    std::shared_ptr<RecordRepository> m_repository;
    std::vector<Record>               m_records; // sempre ordenado, sempre <= kMaxRecords

public:
    static constexpr size_t kMaxRecords = 10;
    static constexpr size_t kMaxNameLength = 8;

    explicit RecordService(std::shared_ptr<RecordRepository> repository);

    /// Guarda o recorde e persiste — ja ordenado e cortado no top-N.
    void addRecord(const Record& record);

    /// O top-N, do maior para o menor.
    [[nodiscard]] const std::vector<Record>& listByScore() const { return m_records; }

    /// Esta pontuacao entra no top-N? (O que decide se o jogo pede o nome.)
    [[nodiscard]] bool isNewRecord(int score) const;

    /// Tira do nome o que quebraria o TSV (tab/quebra de linha) e corta o
    /// excesso. Nome vazio vira "ANONIMO" — uma linha sem nome e pior que um
    /// nome feio.
    [[nodiscard]] static std::string sanitizeName(const std::string& name);
};

} // namespace brk
