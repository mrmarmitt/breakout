#include "ForgeGameScene.h"

#include <cstdio>
#include <string>
#include <utility>

#include "breakout/game/GameRouter.h"

#include "ForgeUi.h"

using cengine::input::Key;

namespace {

// Cor por linha de tijolo (as de cima valem mais — a pontuacao entra na task 04).
constexpr uint32_t kRowColors[] = {
    forgeui::color::kAccent,  forgeui::color::kValue,  forgeui::color::kSuccess,
    forgeui::color::kSuccess, forgeui::color::kText,   forgeui::color::kDim,
};

// Enquanto nao ha sprite, um retangulo e desenhado como uma FILEIRA DE '#'
// dimensionada para o tamanho dele. Feio de proposito: o desenho de verdade e
// a task 03.
void drawBlock(const brk::Aabb& r, const uint32_t color)
{
    const float fontSize = r.h;
    const float glyphW = forgeui::textWidth("#", fontSize);
    const auto  count = static_cast<int>(r.w / (glyphW > 0.0f ? glyphW : 1.0f));

    std::string block;
    for (int i = 0; i < count; ++i)
    {
        block += "#";
    }

    forgeui::drawText(block, r.x, r.y, fontSize, color);
}

} // namespace

ForgeGameScene::ForgeGameScene(std::shared_ptr<GameRouter> gameRouter, cengine::input::Keyboard& keyboard)
    : m_gameRouter(std::move(gameRouter)), m_keyboard(keyboard)
{
}

brk::Aabb ForgeGameScene::toScreen(const brk::Aabb& rect) const
{
    const float sx = forgeui::screenWidth() / brk::World::kArenaW;
    const float sy = forgeui::screenHeight() / brk::World::kArenaH;

    return { rect.x * sx, rect.y * sy, rect.w * sx, rect.h * sy };
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
}

void ForgeGameScene::draw()
{
    for (uint32_t i = 0; i < m_world.brickCount(); ++i)
    {
        if (m_world.brickAlive(i))
        {
            drawBlock(toScreen(m_world.brickRect(i)), kRowColors[m_world.brickRow(i)]);
        }
    }

    drawBlock(toScreen(m_world.paddle()), forgeui::color::kSuccess);
    drawBlock(toScreen(m_world.ball()), forgeui::color::kText);

    char hud[64] = {};
    std::snprintf(hud, sizeof(hud), "TIJOLOS %u   BOLAS PERDIDAS %u", m_world.bricksAlive(), m_world.ballsLost());
    forgeui::drawText(hud, 24.0f, 24.0f, 18.0f, forgeui::color::kDim);

    if (m_world.serving())
    {
        forgeui::drawTextCentered("ESPACO para sacar", forgeui::screenHeight() * 0.60f, 24.0f, forgeui::color::kAccent);
    }

    forgeui::drawHints("SETAS <- -> mover   ESPACO sacar   ESC voltar ao menu");
}
