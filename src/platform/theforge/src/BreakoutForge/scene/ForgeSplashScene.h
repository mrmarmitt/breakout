#pragma once

#include <memory>

#include <cengine/core/IScene.hpp>
#include <cengine/core/Time.hpp>
#include <cengine/input/Keyboard.hpp>

class GameRouter;

// Splash (estado "initial"): titulo + "pressione ENTER" piscando.
//
// Novidade deste jogo (task 20 da cengine): a cena recebe o TECLADO DA PORTA
// (`cengine::input::Keyboard&`) injetado, em vez de chamar as funcoes globais do
// forgeui. O desenho continua vindo do forgeui — a porta de DESENHO nao existe,
// e nao ha evidencia que justifique invento-la.
class ForgeSplashScene final: public cengine::core::IScene
{
    std::shared_ptr<GameRouter> m_gameRouter;
    cengine::input::Keyboard&   m_keyboard;

    double m_elapsed = 0.0;

public:
    ForgeSplashScene(std::shared_ptr<GameRouter> gameRouter, cengine::input::Keyboard& keyboard);

    void onEnter() override {}
    void update(cengine::core::Seconds dt) override;
    void draw() override;
    void input() override;
    void onExit() override {}
};
