#pragma once

// Fachada de navegacao de dominio do Breakout sobre o roteador da cengine.
// Aqui so existe o VOCABULARIO: a mecanica (guardar o router, castar o estado
// atual, delegar o agendamento) vem do FlowRouter da cengine — o asteroids foi
// o primeiro consumidor, este e o segundo.

#include <cengine/routing/FlowRouter.hpp>

#include "state/StateGameFlow.h"

class GameRouter final: public cengine::routing::FlowRouter<StateGameFlow>
{
public:
    using FlowRouter::FlowRouter;

    // As transicoes despacham sobre o estado ATUAL do router.
    void menu() const { current().menu(*this); }
    void game() const { current().game(*this); }
    void gameOver() const { current().gameOver(*this); }
    void records() const { current().records(*this); }
    void exit() const { current().exit(*this); }
};
