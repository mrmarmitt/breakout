#include "ForgeGameOverScene.h"

#include <cmath>
#include <cstdio>
#include <utility>

#include "breakout/game/GameRouter.h"
#include "breakout/game/service/PlaySession.h"

#include "ForgeUi.h"

using cengine::input::Key;

ForgeGameOverScene::ForgeGameOverScene(std::shared_ptr<GameRouter> gameRouter, std::shared_ptr<PlaySession> session,
                                       cengine::input::Keyboard& keyboard)
    : m_gameRouter(std::move(gameRouter)), m_session(std::move(session)), m_keyboard(keyboard)
{
}

void ForgeGameOverScene::update(const cengine::core::Seconds dt) { m_elapsed += dt.count(); }

void ForgeGameOverScene::draw()
{
    const float h = forgeui::screenHeight();

    forgeui::drawTextCentered("G A M E   O V E R", h * 0.24f, 44.0f, forgeui::color::kTitle);

    char score[48] = {};
    std::snprintf(score, sizeof(score), "%04d", m_session->score());
    forgeui::drawTextCentered(score, h * 0.44f, 52.0f, forgeui::color::kValue);

    char level[48] = {};
    std::snprintf(level, sizeof(level), "chegou ate a fase %u", m_session->level());
    forgeui::drawTextCentered(level, h * 0.56f, 20.0f, forgeui::color::kDim);

    if (std::fmod(m_elapsed, 1.2) < 0.7)
    {
        forgeui::drawTextCentered("ENTER para jogar de novo", h * 0.70f, 24.0f, forgeui::color::kAccent);
    }

    forgeui::drawHints("ENTER jogar de novo   ESC voltar ao menu");
}

void ForgeGameOverScene::input()
{
    switch (m_keyboard.readKey().key)
    {
    case Key::Enter:
        m_gameRouter->game(); // cena recriada pela factory: partida zerada
        break;
    case Key::Escape:
        m_gameRouter->menu();
        break;
    default:
        break;
    }
}
