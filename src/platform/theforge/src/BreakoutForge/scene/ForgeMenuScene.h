#pragma once

#include <memory>

#include <cengine/core/IScene.hpp>
#include <cengine/core/Time.hpp>
#include <cengine/input/Keyboard.hpp>

class GameRouter;

// Menu (estado "menu"): JOGAR / SAIR, navegado pela FILA DE EDGES da porta de
// input — um evento por input(), que e o que impede a seta segurada de varrer o
// menu inteiro num quadro.
class ForgeMenuScene final: public cengine::core::IScene
{
    std::shared_ptr<GameRouter> m_gameRouter;
    cengine::input::Keyboard&   m_keyboard;

    int m_selected = 0;

public:
    ForgeMenuScene(std::shared_ptr<GameRouter> gameRouter, cengine::input::Keyboard& keyboard);

    void onEnter() override {}
    void update(cengine::core::Seconds) override {}
    void draw() override;
    void input() override;
    void onExit() override {}
};
