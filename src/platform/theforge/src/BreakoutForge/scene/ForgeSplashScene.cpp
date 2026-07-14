#include "ForgeSplashScene.h"

#include <cmath>
#include <utility>

#include "breakout/game/GameRouter.h"

#include "ForgeUi.h"

using cengine::input::Key;

ForgeSplashScene::ForgeSplashScene(std::shared_ptr<GameRouter> gameRouter, cengine::input::Keyboard& keyboard)
    : m_gameRouter(std::move(gameRouter)), m_keyboard(keyboard)
{
}

void ForgeSplashScene::update(const cengine::core::Seconds dt) { m_elapsed += dt.count(); }

void ForgeSplashScene::draw()
{
    const float h = forgeui::screenHeight();

    forgeui::drawTextCentered("B R E A K O U T", h * 0.30f, 64.0f, forgeui::color::kTitle);
    forgeui::drawTextCentered("cengine + platform-theforge-common", h * 0.42f, 20.0f, forgeui::color::kDim);

    // piscar dirigido pelo tempo de simulacao (update com dt fixo)
    if (std::fmod(m_elapsed, 1.2) < 0.8)
    {
        forgeui::drawTextCentered("Pressione ENTER para comecar", h * 0.66f, 24.0f, forgeui::color::kValue);
    }

    forgeui::drawHints("ENTER comecar");
}

void ForgeSplashScene::input()
{
    switch (m_keyboard.readKey().key)
    {
    case Key::Enter:
    case Key::Escape:
        m_gameRouter->menu(); // sair so existe a partir do menu
        break;
    default:
        break;
    }
}
