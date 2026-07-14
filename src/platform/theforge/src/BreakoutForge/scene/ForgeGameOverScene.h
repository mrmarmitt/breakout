#pragma once

#include <memory>
#include <string>

#include <cengine/core/IScene.hpp>
#include <cengine/core/Time.hpp>
#include <cengine/input/Keyboard.hpp>

class GameRouter;
class PlaySession;

namespace brk {
class RecordService;
}

// O estado "gameover": mostra o resultado da partida que acabou (lido do
// PlaySession — a cena do jogo, e o World dela, ja morreram) e, se a pontuacao
// entrou no top-N, pede o nome ali mesmo antes de gravar.
//
//   recorde     -> digita o nome, ENTER grava e mostra a tabela
//   sem recorde -> ENTER joga de novo, ESC volta ao menu
class ForgeGameOverScene final: public cengine::core::IScene
{
    std::shared_ptr<GameRouter>         m_gameRouter;
    std::shared_ptr<PlaySession>        m_session;
    std::shared_ptr<brk::RecordService> m_records;
    cengine::input::Keyboard&           m_keyboard;

    bool        m_isNewRecord = false; // decidido no onEnter, quando o placar ja existe
    std::string m_name;

    double m_elapsed = 0.0;

    void saveRecord();

public:
    ForgeGameOverScene(std::shared_ptr<GameRouter> gameRouter, std::shared_ptr<PlaySession> session,
                       std::shared_ptr<brk::RecordService> records, cengine::input::Keyboard& keyboard);

    void onEnter() override;
    void update(cengine::core::Seconds dt) override;
    void draw() override;
    void input() override;
    void onExit() override {}
};
