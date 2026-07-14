#include "ForgeGameScene.h"

#include <cstdio>
#include <string>
#include <utility>

#include "breakout/game/GameRouter.h"
#include "breakout/game/service/PlaySession.h"

#include "../BreakoutSprites.h"
#include "ForgeSpriteUi.h"
#include "ForgeUi.h"

using cengine::input::Key;

namespace {

// Cor por linha de tijolo (as de cima valem mais — a pontuacao entra na task
// 04). O atlas e branco: aqui a cor E o tint.
constexpr uint32_t kRowColors[] = {
    forgeui::color::kAccent, forgeui::color::kValue, forgeui::color::kSuccess,
    forgeui::color::kText,   forgeui::color::kDim,   forgeui::color::kFaint,
};

} // namespace

ForgeGameScene::ForgeGameScene(std::shared_ptr<GameRouter> gameRouter, std::shared_ptr<PlaySession> session,
                               cengine::input::Keyboard& keyboard)
    : m_gameRouter(std::move(gameRouter)), m_session(std::move(session)), m_keyboard(keyboard)
{
}

brk::Aabb ForgeGameScene::toScreen(const brk::Aabb& rect) const
{
    // A projecao arena -> tela estica X e Y de forma DIFERENTE (800x600 numa
    // janela 1280x720) — e por isso que o sprite precisa de um retangulo de
    // destino, e nao de uma escala uniforme.
    const float sx = forgeui::screenWidth() / brk::World::kArenaW;
    const float sy = forgeui::screenHeight() / brk::World::kArenaH;

    return { rect.x * sx, rect.y * sy, rect.w * sx, rect.h * sy };
}

void ForgeGameScene::drawSprite(const forgesprite::SpriteRegion& region, const brk::Aabb& rect,
                                const uint32_t color) const
{
    const brk::Aabb screen = toScreen(rect);
    forgesprite::drawSpriteRect(region, screen.x, screen.y, screen.w, screen.h, color);
}

void ForgeGameScene::input()
{
    // Edges: transicao de estado e o saque. Movimento e estado CONTINUO, lido no
    // update — as duas leituras da porta, cada uma no seu lugar.
    switch (m_keyboard.readKey().key)
    {
    case Key::Escape:
        m_gameRouter->menu();
        break;
    case Key::Char: // o espaco chega como Char ' ' pela fila (WM_CHAR)
        m_world.serve();
        break;
    default:
        break;
    }
}

void ForgeGameScene::update(const cengine::core::Seconds dt)
{
    m_world.setMoveAxis(m_keyboard.heldAxis(Key::Left, Key::Right));
    m_world.update(dt.count());

    // Acabaram as vidas: o World so CONSTATA; quem decide o que a derrota
    // significa e o fluxo. Publica o resultado (o World morre com a cena) e
    // roteia.
    if (m_world.outcome() == brk::World::Outcome::GameOver)
    {
        m_session->setResult(m_world.score(), m_world.level());
        m_gameRouter->gameOver();
    }
}

void ForgeGameScene::draw()
{
    // Sprites primeiro, texto depois: o drawText da flush no lote pendente, e o
    // HUD fica por cima do jogo.
    for (uint32_t i = 0; i < m_world.brickCount(); ++i)
    {
        if (m_world.brickAlive(i))
        {
            drawSprite(sprites::kBrick, m_world.brickRect(i), kRowColors[m_world.brickRow(i)]);
        }
    }

    drawSprite(sprites::kPaddle, m_world.paddle(), forgeui::color::kSuccess);
    drawSprite(sprites::kBall, m_world.ball(), forgeui::color::kText);

    // HUD: pontos a esquerda, vidas em raquetes, fase a direita.
    char score[32] = {};
    std::snprintf(score, sizeof(score), "%04d", m_world.score());
    forgeui::drawText(score, 24.0f, 20.0f, 24.0f, forgeui::color::kValue);

    std::string paddles;
    for (int i = 0; i < m_world.lives(); ++i)
    {
        paddles += "= ";
    }
    forgeui::drawText(paddles, 24.0f, 50.0f, 22.0f, forgeui::color::kSuccess);

    char level[32] = {};
    std::snprintf(level, sizeof(level), "FASE %u", m_world.level());
    forgeui::drawText(level, forgeui::screenWidth() - forgeui::textWidth(level, 18.0f) - 24.0f, 24.0f, 18.0f,
                      forgeui::color::kDim);

    if (m_world.serving())
    {
        forgeui::drawTextCentered("ESPACO para sacar", forgeui::screenHeight() * 0.60f, 24.0f, forgeui::color::kAccent);
    }

    forgeui::drawHints("SETAS <- -> mover   ESPACO sacar   ESC voltar ao menu");
}
