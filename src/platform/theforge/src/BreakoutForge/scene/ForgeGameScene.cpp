#include "ForgeGameScene.h"

#include <algorithm>
#include <utility>

#include "breakout/game/GameRouter.h"

#include "ForgeUi.h"

using cengine::input::Key;

namespace {
constexpr float kSpeed = 0.9f; // fracao da tela por segundo
} // namespace

ForgeGameScene::ForgeGameScene(std::shared_ptr<GameRouter> gameRouter, cengine::input::Keyboard& keyboard)
    : m_gameRouter(std::move(gameRouter)), m_keyboard(keyboard)
{
}

void ForgeGameScene::input()
{
    // Edges: so a transicao de estado. O movimento e estado CONTINUO, lido no
    // update — as duas leituras da porta, cada uma no seu lugar.
    if (m_keyboard.readKey().key == Key::Escape)
    {
        m_gameRouter->menu();
    }
}

void ForgeGameScene::update(const cengine::core::Seconds dt)
{
    const float step = kSpeed * static_cast<float>(dt.count());
    m_paddleX = std::clamp(m_paddleX + m_keyboard.heldAxis(Key::Left, Key::Right) * step, 0.08f, 0.92f);
}

void ForgeGameScene::draw()
{
    const float w = forgeui::screenWidth();
    const float h = forgeui::screenHeight();

    // A raquete: um traco de texto, so para o casco ter o que mover.
    forgeui::drawText("=========", m_paddleX * w - 60.0f, h * 0.88f, 28.0f, forgeui::color::kSuccess);

    forgeui::drawText("CASCO (task 01) — o World entra na task 02", 24.0f, 24.0f, 18.0f, forgeui::color::kFaint);
    forgeui::drawHints("SETAS <- -> mover a raquete   ESC voltar ao menu");
}
