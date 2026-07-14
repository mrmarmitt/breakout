#include <gtest/gtest.h>

#include <cmath>

#include "breakout/game/World.h"

// A simulacao do Breakout. Os testes dirigem o World do mesmo jeito que a cena:
// comandos -> update(dt fixo) -> consultas. Nada de plataforma aqui.

namespace {

constexpr double kStep = 1.0 / 60.0; // o passo fixo do engine

void advance(brk::World& world, const double seconds)
{
    for (double elapsed = 0.0; elapsed < seconds; elapsed += kStep)
    {
        world.update(kStep);
    }
}

// Arena limpa: os testes de raquete/bola/parede nao querem tijolo no caminho.
brk::World emptyArena()
{
    return brk::World{ brk::WorldConfig{ .spawnBricks = false } };
}

float centerX(const brk::Aabb& r) { return r.x + r.w * 0.5f; }
float centerY(const brk::Aabb& r) { return r.y + r.h * 0.5f; }

} // namespace

// =============================================================================
// A raquete
// =============================================================================

TEST(WorldTest, PaddleStartsCenteredAtTheBottom)
{
    const brk::World world = emptyArena();

    EXPECT_FLOAT_EQ(centerX(world.paddle()), brk::World::kArenaW * 0.5f);
    EXPECT_FLOAT_EQ(world.paddle().y, brk::World::kPaddleY);
}

TEST(WorldTest, PaddleMovesWithTheAxis)
{
    brk::World world = emptyArena();

    const float before = centerX(world.paddle());

    world.setMoveAxis(1.0f);
    advance(world, 0.25);
    EXPECT_GT(centerX(world.paddle()), before);

    world.setMoveAxis(-1.0f);
    advance(world, 0.5);
    EXPECT_LT(centerX(world.paddle()), before);
}

TEST(WorldTest, PaddleStopsAtTheWalls)
{
    brk::World world = emptyArena();

    world.setMoveAxis(-1.0f);
    advance(world, 5.0); // muito alem do necessario

    EXPECT_GE(world.paddle().x, brk::World::kPaddleMargin - 1e-3f);

    world.setMoveAxis(1.0f);
    advance(world, 5.0);

    EXPECT_LE(world.paddle().x + world.paddle().w, brk::World::kArenaW - brk::World::kPaddleMargin + 1e-3f);
}

TEST(WorldTest, MoveAxisIsClamped)
{
    brk::World fast = emptyArena();
    brk::World normal = emptyArena();

    fast.setMoveAxis(50.0f); // input maluco de uma cena
    normal.setMoveAxis(1.0f);

    advance(fast, 0.25);
    advance(normal, 0.25);

    EXPECT_FLOAT_EQ(centerX(fast.paddle()), centerX(normal.paddle()));
}

// =============================================================================
// O saque
// =============================================================================

TEST(WorldTest, BallStartsStuckToThePaddle)
{
    brk::World world = emptyArena();

    EXPECT_TRUE(world.serving());

    // Presa: acompanha a raquete enquanto ela anda.
    world.setMoveAxis(1.0f);
    advance(world, 0.3);

    EXPECT_NEAR(centerX(world.ball()), centerX(world.paddle()), 1.0f);
    EXPECT_TRUE(world.serving());
}

TEST(WorldTest, ServeLaunchesTheBallUpwards)
{
    brk::World world = emptyArena();

    ASSERT_TRUE(world.serve());
    EXPECT_FALSE(world.serving());
    EXPECT_LT(world.ballVelocity().y, 0.0f) << "sobe (Y cresce para baixo)";

    const float before = world.ball().y;
    advance(world, 0.2);
    EXPECT_LT(world.ball().y, before);
}

TEST(WorldTest, ServingTwiceDoesNothing)
{
    brk::World world = emptyArena();

    EXPECT_TRUE(world.serve());
    EXPECT_FALSE(world.serve()) << "a bola ja esta em jogo";
}

// =============================================================================
// As paredes
// =============================================================================

TEST(WorldTest, BallBouncesOffTheSideWalls)
{
    brk::World world = emptyArena();

    world.placeBall({ 10.0f, 300.0f }, { -brk::World::kBallSpeed, 0.0f }); // rumo a esquerda

    advance(world, 0.5);

    EXPECT_GT(world.ballVelocity().x, 0.0f) << "voltou";
    EXPECT_GE(world.ball().x, 0.0f) << "e nao atravessou a parede";
}

TEST(WorldTest, BallBouncesOffTheCeiling)
{
    brk::World world = emptyArena();

    world.placeBall({ 400.0f, 10.0f }, { 0.0f, -brk::World::kBallSpeed });

    advance(world, 0.5);

    EXPECT_GT(world.ballVelocity().y, 0.0f) << "voltou para baixo";
    EXPECT_GE(world.ball().y, 0.0f);
}

TEST(WorldTest, BallFallingThroughTheBottomIsLost)
{
    brk::World world = emptyArena();

    ASSERT_EQ(world.ballsLost(), 0u);

    world.placeBall({ 400.0f, brk::World::kArenaH - 20.0f }, { 0.0f, brk::World::kBallSpeed });

    advance(world, 1.0);

    EXPECT_EQ(world.ballsLost(), 1u) << "o fundo NAO e parede: e por ali que a bola se perde";
    EXPECT_TRUE(world.serving()) << "e a proxima bola volta presa a raquete";
}

// =============================================================================
// A raquete rebate — e aqui mora o CONTROLE do jogo
// =============================================================================

namespace {

// Poe a bola caindo bem em cima de um ponto X da raquete, e roda ate o rebote.
brk::World ballFallingOnPaddleAt(const float ballCenterX)
{
    brk::World world = emptyArena();

    world.placeBall({ ballCenterX - brk::World::kBallSize * 0.5f, brk::World::kPaddleY - brk::World::kBallSize - 4.0f },
                    { 0.0f, brk::World::kBallSpeed });

    for (int i = 0; i < 30 && world.ballVelocity().y > 0.0f; ++i)
    {
        world.update(kStep);
    }

    return world;
}

} // namespace

TEST(WorldTest, BallBouncesOffThePaddleUpwards)
{
    const brk::World world = ballFallingOnPaddleAt(brk::World::kArenaW * 0.5f);

    EXPECT_LT(world.ballVelocity().y, 0.0f) << "sobe depois de bater";
}

TEST(WorldTest, HittingThePaddleCenterReturnsTheBallStraight)
{
    const brk::World world = ballFallingOnPaddleAt(brk::World::kArenaW * 0.5f);

    EXPECT_NEAR(world.ballVelocity().x, 0.0f, 1.0f) << "centro devolve reto";
}

TEST(WorldTest, HittingThePaddleEdgeReturnsTheBallSideways)
{
    // A ponta DIREITA joga a bola para a direita; a esquerda, para a esquerda.
    // E isto que da controle ao jogador — sem isso o jogo vira sorteio.
    const brk::World right = ballFallingOnPaddleAt(brk::World::kArenaW * 0.5f + brk::World::kPaddleW * 0.45f);
    const brk::World left = ballFallingOnPaddleAt(brk::World::kArenaW * 0.5f - brk::World::kPaddleW * 0.45f);

    EXPECT_GT(right.ballVelocity().x, 100.0f);
    EXPECT_LT(left.ballVelocity().x, -100.0f);
    EXPECT_LT(right.ballVelocity().y, 0.0f) << "sempre para cima, mesmo na ponta";
    EXPECT_LT(left.ballVelocity().y, 0.0f);
}

TEST(WorldTest, PaddleBounceKeepsTheBallSpeedConstant)
{
    const brk::World world = ballFallingOnPaddleAt(brk::World::kArenaW * 0.5f + brk::World::kPaddleW * 0.4f);

    const brk::Vec2 v = world.ballVelocity();
    const float     speed = std::sqrt(v.x * v.x + v.y * v.y);

    // O angulo muda; a velocidade nao. Senao o jogo acelera sozinho na ponta.
    EXPECT_NEAR(speed, brk::World::kBallSpeed, 1.0f);
}

// =============================================================================
// Os tijolos
// =============================================================================

TEST(WorldTest, AllBricksStartAlive)
{
    const brk::World world;

    EXPECT_EQ(world.bricksAlive(), brk::World::kBrickCount);
    EXPECT_TRUE(world.brickAlive(0));
    EXPECT_TRUE(world.brickAlive(brk::World::kBrickCount - 1));
}

TEST(WorldTest, BricksAreLaidOutInAGridInsideTheArena)
{
    const brk::World world;

    for (uint32_t i = 0; i < world.brickCount(); ++i)
    {
        const brk::Aabb brick = world.brickRect(i);

        EXPECT_GE(brick.x, 0.0f);
        EXPECT_LE(brick.x + brick.w, brk::World::kArenaW);
        EXPECT_GE(brick.y, 0.0f);
        EXPECT_LT(brick.y + brick.h, brk::World::kPaddleY) << "nenhum tijolo nasce em cima da raquete";
    }
}

namespace {

// Roda ate a bola quebrar UM tijolo (ou desistir). Nao vale usar um advance()
// longo aqui: a bola ricocheteia e vai quebrando o resto da parede — o que e o
// comportamento certo do jogo, mas mede outra coisa.
void advanceUntilABrickBreaks(brk::World& world)
{
    const uint32_t before = world.bricksAlive();
    for (int i = 0; i < 60 && world.bricksAlive() == before; ++i)
    {
        world.update(kStep);
    }
}

// Tijolo da PONTA ESQUERDA de uma linha do meio: sem vizinho a esquerda, entao
// a bola pode chegar pelo lado sem encostar em outro tijolo antes.
constexpr uint32_t kLeftmostBrickOfRow1 = brk::World::kCols;

// Tijolo da linha do TOPO: nao ha nada acima dele.
constexpr uint32_t kTopRowBrick = 5;

} // namespace

TEST(WorldTest, BallHittingABrickKillsIt)
{
    brk::World world;

    const brk::Aabb brick = world.brickRect(kTopRowBrick);

    // Bola subindo, logo abaixo do tijolo.
    world.placeBall({ centerX(brick) - brk::World::kBallSize * 0.5f, brick.y + brick.h + 2.0f },
                    { 0.0f, -brk::World::kBallSpeed });

    advanceUntilABrickBreaks(world);

    EXPECT_FALSE(world.brickAlive(kTopRowBrick));
    EXPECT_EQ(world.bricksAlive(), brk::World::kBrickCount - 1);
}

TEST(WorldTest, BallBouncesDownOffABrickHitFromBelow)
{
    brk::World world;

    const brk::Aabb brick = world.brickRect(kTopRowBrick);

    world.placeBall({ centerX(brick) - brk::World::kBallSize * 0.5f, brick.y + brick.h + 2.0f },
                    { 0.0f, -brk::World::kBallSpeed });

    advanceUntilABrickBreaks(world);

    // Bateu por baixo: volta para baixo (o eixo de menor penetracao foi o Y).
    EXPECT_GT(world.ballVelocity().y, 0.0f);
}

TEST(WorldTest, BallBouncesSidewaysOffABrickHitFromTheSide)
{
    brk::World world;

    const brk::Aabb brick = world.brickRect(kLeftmostBrickOfRow1);

    // Bola vindo da ESQUERDA, na altura do meio do tijolo (e da margem livre da
    // arena, entao ela nao encosta em nenhum vizinho antes).
    world.placeBall({ brick.x - brk::World::kBallSize - 8.0f, centerY(brick) - brk::World::kBallSize * 0.5f },
                    { brk::World::kBallSpeed, 0.0f });

    advanceUntilABrickBreaks(world);

    EXPECT_FALSE(world.brickAlive(kLeftmostBrickOfRow1));
    EXPECT_LT(world.ballVelocity().x, 0.0f) << "veio pelo lado: inverte X, nao Y";
    EXPECT_FLOAT_EQ(world.ballVelocity().y, 0.0f);
}

TEST(WorldTest, DeadBricksDoNotCollide)
{
    brk::World world;

    // Abre um CORREDOR: a coluna inteira. Um buraco de um tijolo so nao serve
    // de teste — o vao entre as linhas e de 4 e a bola tem 12, entao ela nunca
    // "flutua" entre duas fileiras: encostar numa e encostar na de baixo.
    constexpr uint32_t kColumn = 5;
    for (uint32_t row = 0; row < brk::World::kRows; ++row)
    {
        world.killBrick(row * brk::World::kCols + kColumn);
    }

    const brk::Aabb brick = world.brickRect(kColumn);
    const uint32_t  before = world.bricksAlive();

    // Sobe pelo corredor, vinda de baixo de toda a parede.
    world.placeBall({ centerX(brick) - brk::World::kBallSize * 0.5f, 400.0f }, { 0.0f, -brk::World::kBallSpeed });

    advance(world, 0.8);

    EXPECT_EQ(world.bricksAlive(), before) << "tijolo morto nao colide: a bola atravessa o corredor";
}

TEST(WorldTest, OnlyOneBrickDiesPerFrame)
{
    brk::World world;

    // Bola encaixada no CANTO entre dois tijolos vizinhos, tocando os dois no
    // mesmo quadro. Sem o corte de "um por quadro", ela refletiria duas vezes e
    // sairia empurrada para dentro da parede.
    const brk::Aabb brick = world.brickRect(20);
    world.placeBall({ brick.x + brick.w - 2.0f, brick.y + brick.h - 2.0f }, { 0.0f, -brk::World::kBallSpeed });

    const uint32_t before = world.bricksAlive();
    world.update(kStep);

    EXPECT_EQ(world.bricksAlive(), before - 1) << "no maximo (e exatamente) um tijolo neste quadro";
}

TEST(WorldTest, ClearingTheArenaLeavesNoBricks)
{
    brk::World world;

    world.clearBricks();

    EXPECT_EQ(world.bricksAlive(), 0u);
    EXPECT_FALSE(world.brickAlive(0));
}

TEST(WorldTest, NonPositiveDtDoesNothing)
{
    brk::World world = emptyArena();

    world.setMoveAxis(1.0f);
    world.update(0.0);
    world.update(-1.0);

    EXPECT_FLOAT_EQ(centerX(world.paddle()), brk::World::kArenaW * 0.5f);
}
