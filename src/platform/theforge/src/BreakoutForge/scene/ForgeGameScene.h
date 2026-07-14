#pragma once

#include <memory>

#include <cengine/core/IScene.hpp>
#include <cengine/core/Time.hpp>
#include <cengine/input/Keyboard.hpp>

class GameRouter;

// Placeholder do gameplay (estado "game") — degrau do casco (task 01): uma
// raquete de texto anda com as setas SEGURADAS, validando o ESTADO CONTINUO da
// porta de input (`isHeld`/`heldAxis`) no mesmo update de dt fixo que o World
// vai usar.
//
// O World de verdade (raquete + bola + tijolos, colisao AABB pela
// cengine::collision2d) entra na task 02.
class ForgeGameScene final: public cengine::core::IScene
{
    std::shared_ptr<GameRouter> m_gameRouter;
    cengine::input::Keyboard&   m_keyboard;

    float m_paddleX = 0.5f; // posicao normalizada (0..1)

public:
    ForgeGameScene(std::shared_ptr<GameRouter> gameRouter, cengine::input::Keyboard& keyboard);

    void onEnter() override {}
    void update(cengine::core::Seconds dt) override;
    void draw() override;
    void input() override;
    void onExit() override {}
};
