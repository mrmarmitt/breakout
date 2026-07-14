#pragma once

#include <cstdint>

// Resultado da ultima partida, compartilhado entre a cena do jogo (escreve no
// game over) e a cena de game over (le para exibir). Mesmo papel do PlaySession
// dos irmaos — as cenas nascem e morrem a cada visita, entao o placar precisa de
// alguem que sobreviva a troca de estado.
//
// Nao guarda o World: o que atravessa a fronteira e o RESULTADO, nao a
// simulacao. A partida seguinte comeca zerada, de proposito.
class PlaySession
{
    int      m_score = 0;
    uint32_t m_level = 1;

public:
    void setResult(const int score, const uint32_t level)
    {
        m_score = score;
        m_level = level;
    }

    [[nodiscard]] int      score() const { return m_score; }
    [[nodiscard]] uint32_t level() const { return m_level; }
};
