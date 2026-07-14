#pragma once

#include <memory>

#include <cengine/core/IScene.hpp>
#include <cengine/core/Time.hpp>
#include <cengine/input/Keyboard.hpp>

#include "ForgeSpriteUi.h"

#include "breakout/game/World.h"

class GameRouter;

// O estado "game": a cena e so a casca — traduz o input da PORTA em comandos do
// World, chama update(dt) com o passo fixo do engine e desenha lendo as
// consultas. Nenhuma regra de jogo mora aqui.
//
// Desenho em SPRITES (forgesprite, do platform-theforge-common): este jogo e
// quem traz o batcher de volta a vida — desde que o spaceinvaders congelou e o
// asteroids foi de wireframe, nenhum jogo vivo o compilava.
class ForgeGameScene final: public cengine::core::IScene
{
    std::shared_ptr<GameRouter> m_gameRouter;
    cengine::input::Keyboard&   m_keyboard;
    brk::World                  m_world;

    /// Projeta um retangulo da arena (800x600) para pixels da tela.
    [[nodiscard]] brk::Aabb toScreen(const brk::Aabb& rect) const;

    /// Desenha uma regiao do atlas ESTICADA sobre um retangulo da arena.
    void drawSprite(const forgesprite::SpriteRegion& region, const brk::Aabb& rect, uint32_t color) const;

public:
    ForgeGameScene(std::shared_ptr<GameRouter> gameRouter, cengine::input::Keyboard& keyboard);

    void onEnter() override {}
    void update(cengine::core::Seconds dt) override;
    void draw() override;
    void input() override;
    void onExit() override {}
};
