#pragma once

#include <cstdint>

// O dominio do Breakout (task 02): raquete, bola e tijolos — em C++ puro, sem
// nenhuma dependencia de plataforma. A cena traduz input em comandos, chama
// update(dt) com o passo fixo do engine e desenha lendo as consultas; os testes
// (CMake, sem The-Forge) dirigem a simulacao do mesmo jeito.
//
// Coordenadas: arena virtual de 800x600, origem no canto superior esquerdo, Y
// crescendo PARA BAIXO (mesma convencao dos irmaos).
//
// COLISAO: retangulo x retangulo, detectada pela `cengine::collision2d` — este
// jogo e o consumidor VIVO que faltava para o AABB da engine (a evidencia que o
// promoveu vinha do spaceinvaders, congelado).
//
// E a bola e um QUADRADO de proposito: no Breakout original ela e um blob
// retangular, nao um circulo. AABB aqui nao e simplificacao, e fidelidade.
//
// A fronteira que este jogo torna nitida: a ENGINE responde "estes dois
// retangulos se tocam?"; o JOGO decide o que isso SIGNIFICA — refletir o vetor,
// matar o tijolo, perder a bola. Reflexao e politica de gameplay, nao geometria.

#include <cengine/collision2d/Shapes.hpp>

namespace brk {

using Aabb = cengine::collision2d::Aabb;
using Vec2 = cengine::collision2d::Vec2;

struct WorldConfig
{
    /// Testes que so querem exercitar a raquete/bola nascem com a arena limpa.
    bool spawnBricks = true;
};

class World
{
public:
    static constexpr float kArenaW = 800.0f;
    static constexpr float kArenaH = 600.0f;

    // --- raquete ---
    static constexpr float kPaddleW = 110.0f;
    static constexpr float kPaddleH = 16.0f;
    static constexpr float kPaddleY = 545.0f;
    static constexpr float kPaddleSpeed = 520.0f; // unidades/s
    static constexpr float kPaddleMargin = 12.0f;

    // --- bola (quadrada, como no arcade) ---
    static constexpr float kBallSize = 12.0f;
    static constexpr float kBallSpeed = 380.0f;

    /// Angulo maximo de saida da raquete, medido da vertical. A bola sai mais
    /// "aberta" quanto mais longe do centro da raquete ela bate — e o que da
    /// CONTROLE ao jogador, e o que separa Breakout de um pinball burro.
    static constexpr float kMaxBounceAngle = 1.0472f; // 60 graus

    // --- tijolos ---
    static constexpr uint32_t kRows = 6;
    static constexpr uint32_t kCols = 12;
    static constexpr uint32_t kBrickCount = kRows * kCols;
    static constexpr float    kBrickW = 60.0f;
    static constexpr float    kBrickH = 20.0f;
    static constexpr float    kBrickGap = 4.0f;
    static constexpr float    kBrickTop = 70.0f;
    static constexpr float    kBrickLeft = (kArenaW - kCols * (kBrickW + kBrickGap) + kBrickGap) * 0.5f;

    explicit World(WorldConfig config = {});

    // --- comandos (fase input da cena) ---

    /// Direcao da raquete neste quadro: -1 esquerda, 0 parada, +1 direita.
    /// Valores fora de [-1, 1] sao saturados.
    void setMoveAxis(float axis);

    /// Solta a bola presa na raquete. Retorna se ela saiu (falso se ja estava
    /// em jogo).
    bool serve();

    // --- simulacao (fase update, dt fixo do engine) ---
    void update(double dt);

    // --- consultas (fase draw e testes) ---
    [[nodiscard]] Aabb paddle() const;
    [[nodiscard]] Aabb ball() const;
    [[nodiscard]] Vec2 ballVelocity() const { return m_ballVel; }

    /// Bola presa na raquete, esperando o saque.
    [[nodiscard]] bool serving() const { return m_serving; }

    /// Quantas bolas cairam pelo fundo. Vidas/pontuacao/fases sao a task 04;
    /// por ora este contador e o que PROVA que a bola pode ser perdida.
    [[nodiscard]] uint32_t ballsLost() const { return m_ballsLost; }

    [[nodiscard]] uint32_t brickCount() const { return kBrickCount; }
    [[nodiscard]] bool     brickAlive(uint32_t index) const;
    [[nodiscard]] Aabb     brickRect(uint32_t index) const;
    [[nodiscard]] uint32_t brickRow(uint32_t index) const { return index / kCols; }
    [[nodiscard]] uint32_t bricksAlive() const { return m_bricksAlive; }

    // --- ganchos de teste (padrao do killInvader do spaceinvaders) ---

    /// Poe a bola exatamente onde o teste quer, ja em jogo (sem saque).
    void placeBall(Vec2 position, Vec2 velocity);

    void killBrick(uint32_t index);
    void clearBricks();

private:
    void updatePaddle(double dt);
    void updateBall(double dt);

    void collideWalls();
    void collideBricks();
    void collidePaddle();

    /// Reflete a bola no retangulo atingido pelo eixo de MENOR penetracao — se
    /// ela entrou mais fundo em X do que em Y, foi pelo lado, entao inverte vx.
    /// A engine so disse que os retangulos se tocam; isto aqui e do jogo.
    void reflectOff(const Aabb& target);

    void resetBallToPaddle();

    WorldConfig m_config;

    float m_paddleX = kArenaW * 0.5f; // centro da raquete
    float m_moveAxis = 0.0f;

    Vec2 m_ballPos = {}; // canto superior esquerdo
    Vec2 m_ballVel = {};
    bool m_serving = true;

    bool     m_brickAlive[kBrickCount] = {};
    uint32_t m_bricksAlive = 0;
    uint32_t m_ballsLost = 0;
};

} // namespace brk
