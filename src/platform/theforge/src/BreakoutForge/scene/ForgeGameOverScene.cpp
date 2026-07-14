#include "ForgeGameOverScene.h"

#include <cctype>
#include <cmath>
#include <cstdio>
#include <utility>

#include "breakout/game/GameRouter.h"
#include "breakout/game/Record.h"
#include "breakout/game/service/PlaySession.h"
#include "breakout/game/service/RecordService.h"

#include "ForgeUi.h"

using cengine::input::Key;
using cengine::input::KeyEvent;

ForgeGameOverScene::ForgeGameOverScene(std::shared_ptr<GameRouter> gameRouter, std::shared_ptr<PlaySession> session,
                                       std::shared_ptr<brk::RecordService> records, cengine::input::Keyboard& keyboard)
    : m_gameRouter(std::move(gameRouter)), m_session(std::move(session)), m_records(std::move(records)),
      m_keyboard(keyboard)
{
}

void ForgeGameOverScene::onEnter()
{
    // "Isto e recorde?" e uma pergunta de ATIVACAO, nao de construcao: depende do
    // placar que a cena do jogo acabou de publicar E da tabela como ela esta
    // agora. O onEnter e o gancho que a cengine garante rodar antes do primeiro
    // update/draw da visita.
    m_isNewRecord = m_records->isNewRecord(m_session->score());
    m_name.clear();
}

void ForgeGameOverScene::update(const cengine::core::Seconds dt) { m_elapsed += dt.count(); }

void ForgeGameOverScene::saveRecord()
{
    m_records->addRecord(brk::Record{ m_name, m_session->score(), m_session->level(), brk::nowAsString() });
    m_gameRouter->records();
}

void ForgeGameOverScene::draw()
{
    const float h = forgeui::screenHeight();

    forgeui::drawTextCentered("G A M E   O V E R", h * 0.20f, 44.0f, forgeui::color::kTitle);

    char score[48] = {};
    std::snprintf(score, sizeof(score), "%04d", m_session->score());
    forgeui::drawTextCentered(score, h * 0.38f, 52.0f, forgeui::color::kValue);

    char level[48] = {};
    std::snprintf(level, sizeof(level), "chegou ate a fase %u", m_session->level());
    forgeui::drawTextCentered(level, h * 0.50f, 20.0f, forgeui::color::kDim);

    if (m_isNewRecord)
    {
        forgeui::drawTextCentered("NOVO RECORDE!", h * 0.62f, 28.0f, forgeui::color::kAccent);

        const std::string typed = m_name + (std::fmod(m_elapsed, 0.8) < 0.4 ? "_" : " ");
        forgeui::drawTextCentered(typed, h * 0.72f, 34.0f, forgeui::color::kText);

        forgeui::drawHints("Digite seu nome   ENTER gravar   BACKSPACE apagar");
        return;
    }

    if (std::fmod(m_elapsed, 1.2) < 0.7)
    {
        forgeui::drawTextCentered("ENTER para jogar de novo", h * 0.68f, 24.0f, forgeui::color::kAccent);
    }

    forgeui::drawHints("ENTER jogar de novo   ESC voltar ao menu");
}

void ForgeGameOverScene::input()
{
    const KeyEvent event = m_keyboard.readKey();

    if (!m_isNewRecord)
    {
        switch (event.key)
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
        return;
    }

    // Entrada do nome do recordista.
    switch (event.key)
    {
    case Key::Enter:
        saveRecord(); // nome vazio nao trava: o RecordService chama de ANONIMO
        break;
    case Key::Backspace:
        if (!m_name.empty())
        {
            m_name.pop_back();
        }
        break;
    case Key::Char:
        if (m_name.size() < brk::RecordService::kMaxNameLength &&
            std::isalnum(static_cast<unsigned char>(event.character)))
        {
            m_name += static_cast<char>(std::toupper(static_cast<unsigned char>(event.character)));
        }
        break;
    default:
        break;
    }
}
