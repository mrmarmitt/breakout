#include "ForgeGameScene.h"

#include <cmath>
#include <cstdio>
#include <string>
#include <utility>

#include "breakout/game/GameRouter.h"
#include "breakout/game/service/PlaySession.h"

#include "../BreakoutSprites.h"
#include "ForgeSpriteUi.h"
#include "ForgeUi.h"

using cengine::input::Key;
using cengine::input::KeyEvent;

namespace {

// Cor por linha de tijolo (as de cima valem mais — a pontuacao entra na task
// 04). O atlas e branco: aqui a cor E o tint.
constexpr uint32_t kRowColors[] = {
    forgeui::color::kAccent, forgeui::color::kValue, forgeui::color::kSuccess,
    forgeui::color::kText,   forgeui::color::kDim,   forgeui::color::kFaint,
};

} // namespace

ForgeGameScene::ForgeGameScene(std::shared_ptr<GameRouter> gameRouter, std::shared_ptr<PlaySession> session,
                               cengine::input::Keyboard& keyboard, AudioPlayer& audio)
    : m_gameRouter(std::move(gameRouter)), m_session(std::move(session)), m_keyboard(keyboard), m_audio(audio)
{
}

void ForgeGameScene::playSounds(const brk::Events& events) const
{
    // Os EVENTOS sao contados, nao booleanos, entao dois tijolos no mesmo quadro
    // tocam dois blips — e o jogo soa como o que aconteceu, nao como uma media.
    for (uint32_t i = 0; i < events.brickBreaks; ++i)
    {
        m_audio.play(Sound::BrickBreak);
    }
    for (uint32_t i = 0; i < events.paddleHits; ++i)
    {
        m_audio.play(Sound::PaddleHit);
    }
    for (uint32_t i = 0; i < events.wallBounces; ++i)
    {
        m_audio.play(Sound::WallBounce);
    }

    if (events.lifeLost)
    {
        m_audio.play(Sound::LifeLost);
    }
    if (events.levelUp)
    {
        m_audio.play(Sound::LevelUp);
    }
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
    const KeyEvent event = m_keyboard.readKey();

    // PAUSADO: o overlay come o input. A cena de baixo (esta mesma) nao ve nada
    // — e a politica "blocksInputBelow" do desenho de scene stack, aqui em uma
    // linha, porque a camada de baixo somos nos.
    if (m_paused)
    {
        switch (event.key)
        {
        case Key::Enter:
            m_paused = false; // continuar
            break;
        case Key::Escape:
            m_gameRouter->menu(); // abandonar a partida
            break;
        default:
            break;
        }
        return;
    }

    // Edges: pausa e saque. Movimento e estado CONTINUO, lido no update — as
    // duas leituras da porta, cada uma no seu lugar.
    switch (event.key)
    {
    case Key::Escape:
        m_paused = true;
        m_pausedElapsed = 0.0;
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
    if (m_paused)
    {
        // O World NAO e atualizado: a partida congela exatamente onde estava,
        // com a bola no ar. O relogio do overlay corre a parte, so para piscar.
        m_pausedElapsed += dt.count();
        return;
    }

    m_world.setMoveAxis(m_keyboard.heldAxis(Key::Left, Key::Right));
    m_world.update(dt.count());

    // O World RELATA o que aconteceu; a cena decide como isso SOA.
    playSounds(m_world.events());

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

    if (m_paused)
    {
        drawPauseOverlay();
        return; // as dicas da pausa substituem as do jogo
    }

    if (m_world.serving())
    {
        forgeui::drawTextCentered("ESPACO para sacar", forgeui::screenHeight() * 0.60f, 24.0f, forgeui::color::kAccent);
    }

    forgeui::drawHints("SETAS <- -> mover   ESPACO sacar   ESC pausar");
}

void ForgeGameScene::drawPauseOverlay() const
{
    const float w = forgeui::screenWidth();
    const float h = forgeui::screenHeight();

    // O veu: o proprio sprite da bola (solido no atlas) esticado sobre a tela
    // inteira, com tint PRETO e alpha parcial. O jogo continua visivel atras —
    // "drawsBelow", de graca, porque desenhamos por cima em vez de trocar de
    // cena.
    constexpr uint32_t kVeil = 0xC0000000; // ABGR: preto com ~75% de alpha
    forgesprite::drawSpriteRect(sprites::kBall, 0.0f, 0.0f, w, h, kVeil);

    forgeui::drawTextCentered("P A U S A", h * 0.40f, 44.0f, forgeui::color::kTitle);

    if (std::fmod(m_pausedElapsed, 1.2) < 0.7)
    {
        forgeui::drawTextCentered("ENTER para continuar", h * 0.54f, 24.0f, forgeui::color::kAccent);
    }

    forgeui::drawHints("ENTER continuar   ESC abandonar a partida");
}
