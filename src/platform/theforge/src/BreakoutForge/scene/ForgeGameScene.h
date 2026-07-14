#pragma once

#include <memory>

#include <cengine/core/IScene.hpp>
#include <cengine/core/Time.hpp>
#include <cengine/input/Keyboard.hpp>

#include "breakout/game/World.h"

class GameRouter;

// O estado "game": a cena e so a casca — traduz o input da PORTA em comandos do
// World, chama update(dt) com o passo fixo do engine e desenha lendo as
// consultas. Nenhuma regra de jogo mora aqui.
//
// Desenho ainda por TEXTO (os sprites entram na task 03, que e quando este jogo
// traz o forgesprite de volta a vida).
class ForgeGameScene final: public cengine::core::IScene
{
    std::shared_ptr<GameRouter> m_gameRouter;
    cengine::input::Keyboard&   m_keyboard;
    brk::World                  m_world;

    /// Projeta um retangulo da arena (800x600) para pixels da tela.
    [[nodiscard]] brk::Aabb toScreen(const brk::Aabb& rect) const;

public:
    ForgeGameScene(std::shared_ptr<GameRouter> gameRouter, cengine::input::Keyboard& keyboard);

    void onEnter() override {}
    void update(cengine::core::Seconds dt) override;
    void draw() override;
    void input() override;
    void onExit() override {}
};
