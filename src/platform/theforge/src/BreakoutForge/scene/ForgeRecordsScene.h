#pragma once

#include <memory>

#include <cengine/core/IScene.hpp>
#include <cengine/core/Time.hpp>
#include <cengine/input/Keyboard.hpp>

class GameRouter;

namespace brk {
class RecordService;
}

// O estado "records": a tabela do top-10, lida do RecordService. Alcancavel pelo
// menu e logo depois de gravar um recorde novo.
class ForgeRecordsScene final: public cengine::core::IScene
{
    std::shared_ptr<GameRouter>         m_gameRouter;
    std::shared_ptr<brk::RecordService> m_records;
    cengine::input::Keyboard&           m_keyboard;

public:
    ForgeRecordsScene(std::shared_ptr<GameRouter> gameRouter, std::shared_ptr<brk::RecordService> records,
                      cengine::input::Keyboard& keyboard);

    void onEnter() override {}
    void update(cengine::core::Seconds) override {}
    void draw() override;
    void input() override;
    void onExit() override {}
};
