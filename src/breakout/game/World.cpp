#include "World.h"

#include <algorithm>
#include <cmath>

#include <cengine/collision2d/Intersects.hpp>

namespace brk {

using cengine::collision2d::intersects;

World::World(const WorldConfig config): m_config(config)
{
    if (m_config.spawnBricks)
    {
        for (uint32_t i = 0; i < kBrickCount; ++i)
        {
            m_brickAlive[i] = true;
        }
        m_bricksAlive = kBrickCount;
    }

    resetBallToPaddle();
}

// -----------------------------------------------------------------------------
// comandos
// -----------------------------------------------------------------------------

void World::setMoveAxis(const float axis)
{
    m_moveAxis = std::clamp(axis, -1.0f, 1.0f);
}

bool World::serve()
{
    if (!m_serving)
    {
        return false;
    }

    m_serving = false;
    m_ballVel = { 0.0f, -kBallSpeed }; // saque reto para cima
    return true;
}

// -----------------------------------------------------------------------------
// simulacao
// -----------------------------------------------------------------------------

void World::update(const double dt)
{
    if (dt <= 0.0)
    {
        return;
    }

    updatePaddle(dt);
    updateBall(dt);

    // A ENGINE detecta; o JOGO resolve. Cada colisao abaixo pergunta
    // `intersects(...)` e decide sozinha o que a batida significa.
    collideWalls();
    collideBricks();
    collidePaddle();
}

void World::updatePaddle(const double dt)
{
    const auto step = static_cast<float>(dt);

    const float half = kPaddleW * 0.5f;
    m_paddleX = std::clamp(m_paddleX + m_moveAxis * kPaddleSpeed * step, kPaddleMargin + half,
                           kArenaW - kPaddleMargin - half);
}

void World::updateBall(const double dt)
{
    if (m_serving)
    {
        resetBallToPaddle(); // presa: acompanha a raquete
        return;
    }

    const auto step = static_cast<float>(dt);
    m_ballPos.x += m_ballVel.x * step;
    m_ballPos.y += m_ballVel.y * step;
}

// -----------------------------------------------------------------------------
// colisao — a engine responde "se tocam?"; o significado e daqui
// -----------------------------------------------------------------------------

void World::collideWalls()
{
    if (m_serving)
    {
        return;
    }

    if (m_ballPos.x < 0.0f)
    {
        m_ballPos.x = 0.0f;
        m_ballVel.x = -m_ballVel.x;
    }
    else if (m_ballPos.x + kBallSize > kArenaW)
    {
        m_ballPos.x = kArenaW - kBallSize;
        m_ballVel.x = -m_ballVel.x;
    }

    if (m_ballPos.y < 0.0f)
    {
        m_ballPos.y = 0.0f;
        m_ballVel.y = -m_ballVel.y;
    }

    // Fundo NAO reflete: e por ali que a bola se perde. E a unica parede que o
    // jogo trata como derrota, e nao como geometria.
    if (m_ballPos.y > kArenaH)
    {
        ++m_ballsLost;
        resetBallToPaddle();
        m_serving = true;
    }
}

void World::collideBricks()
{
    if (m_serving)
    {
        return;
    }

    const Aabb ballRect = ball();

    for (uint32_t i = 0; i < kBrickCount; ++i)
    {
        if (!m_brickAlive[i] || !intersects(ballRect, brickRect(i)))
        {
            continue;
        }

        reflectOff(brickRect(i));
        killBrick(i);

        // Um tijolo por quadro: com dois tijolos vizinhos atingidos no mesmo
        // passo, refletir duas vezes devolveria a bola para dentro da parede.
        return;
    }
}

void World::collidePaddle()
{
    if (m_serving || m_ballVel.y < 0.0f) // subindo: ja saiu da raquete
    {
        return;
    }

    const Aabb paddleRect = paddle();
    if (!intersects(ball(), paddleRect))
    {
        return;
    }

    // O CONTROLE do jogo mora aqui: o angulo de saida depende de ONDE a bola
    // bateu na raquete. Centro devolve reto; ponta devolve aberto. Isso e
    // politica de gameplay — a engine so disse que os retangulos se tocam.
    const float paddleCenter = paddleRect.x + paddleRect.w * 0.5f;
    const float ballCenter = m_ballPos.x + kBallSize * 0.5f;
    const float offset = std::clamp((ballCenter - paddleCenter) / (paddleRect.w * 0.5f), -1.0f, 1.0f);

    const float angle = offset * kMaxBounceAngle;

    m_ballVel.x = std::sin(angle) * kBallSpeed;
    m_ballVel.y = -std::cos(angle) * kBallSpeed; // sempre para cima

    m_ballPos.y = paddleRect.y - kBallSize; // desencosta, para nao grudar
}

void World::reflectOff(const Aabb& target)
{
    const Aabb b = ball();

    // Quanto a bola entrou em cada eixo. O eixo de MENOR penetracao e por onde
    // ela chegou: entrou 2 em Y e 18 em X? Veio de cima.
    const float overlapX = std::min(b.x + b.w, target.x + target.w) - std::max(b.x, target.x);
    const float overlapY = std::min(b.y + b.h, target.y + target.h) - std::max(b.y, target.y);

    if (overlapX < overlapY)
    {
        m_ballVel.x = -m_ballVel.x;
        // empurra para fora, senao ela colide de novo no proximo quadro
        m_ballPos.x += m_ballVel.x > 0.0f ? overlapX : -overlapX;
    }
    else
    {
        m_ballVel.y = -m_ballVel.y;
        m_ballPos.y += m_ballVel.y > 0.0f ? overlapY : -overlapY;
    }
}

void World::resetBallToPaddle()
{
    m_ballPos = { m_paddleX - kBallSize * 0.5f, kPaddleY - kBallSize };
    m_ballVel = {};
}

// -----------------------------------------------------------------------------
// consultas
// -----------------------------------------------------------------------------

Aabb World::paddle() const
{
    return { m_paddleX - kPaddleW * 0.5f, kPaddleY, kPaddleW, kPaddleH };
}

Aabb World::ball() const
{
    return { m_ballPos.x, m_ballPos.y, kBallSize, kBallSize };
}

bool World::brickAlive(const uint32_t index) const
{
    return index < kBrickCount && m_brickAlive[index];
}

Aabb World::brickRect(const uint32_t index) const
{
    const uint32_t row = index / kCols;
    const uint32_t col = index % kCols;

    return { kBrickLeft + static_cast<float>(col) * (kBrickW + kBrickGap),
             kBrickTop + static_cast<float>(row) * (kBrickH + kBrickGap), kBrickW, kBrickH };
}

// -----------------------------------------------------------------------------
// ganchos de teste
// -----------------------------------------------------------------------------

void World::placeBall(const Vec2 position, const Vec2 velocity)
{
    m_ballPos = position;
    m_ballVel = velocity;
    m_serving = false;
}

void World::killBrick(const uint32_t index)
{
    if (index >= kBrickCount || !m_brickAlive[index])
    {
        return;
    }

    m_brickAlive[index] = false;
    --m_bricksAlive;
}

void World::clearBricks()
{
    for (uint32_t i = 0; i < kBrickCount; ++i)
    {
        m_brickAlive[i] = false;
    }
    m_bricksAlive = 0;
}

} // namespace brk
