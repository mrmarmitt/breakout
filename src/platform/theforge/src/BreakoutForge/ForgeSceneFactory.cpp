#include "ForgeSceneFactory.h"

#include <cengine/input/Keyboard.hpp>
#include <cengine/routing/ISceneRepository.hpp>

#include "audio/AudioPlayer.h"
#include "breakout/game/GameRouter.h"
#include "breakout/game/service/PlaySession.h"
#include "breakout/game/service/RecordService.h"

#include "scene/ForgeExitScene.h"
#include "scene/ForgeGameOverScene.h"
#include "scene/ForgeGameScene.h"
#include "scene/ForgeMenuScene.h"
#include "scene/ForgeRecordsScene.h"
#include "scene/ForgeSplashScene.h"

// As factories rodam LAZY (no primeiro getScene de cada estado) e a cena morre ao
// sair do estado (o router a descarrega no commit) — capturas por VALOR. Dai a
// partida nova nascer zerada de graca.
//
// O que precisa ATRAVESSAR a troca de cena vive fora delas: o PlaySession (o
// resultado da ultima partida), o RecordService (a tabela), o teclado da porta e
// o audio (que vivem no casco, e por isso vao por REFERENCIA).
void ForgeSceneFactory::populateForgeScenes(cengine::routing::ISceneRepository&        sceneRepository,
                                            const std::shared_ptr<GameRouter>&         gameRouter,
                                            const std::shared_ptr<PlaySession>&        session,
                                            const std::shared_ptr<brk::RecordService>& records,
                                            cengine::input::Keyboard&                  keyboard,
                                            AudioPlayer&                               audio)
{
    sceneRepository.registerFactory(
        "initial", [gameRouter, &keyboard]() { return std::make_unique<ForgeSplashScene>(gameRouter, keyboard); });
    sceneRepository.registerFactory(
        "menu", [gameRouter, &keyboard]() { return std::make_unique<ForgeMenuScene>(gameRouter, keyboard); });
    sceneRepository.registerFactory("game", [gameRouter, session, &keyboard, &audio]() {
        return std::make_unique<ForgeGameScene>(gameRouter, session, keyboard, audio);
    });
    sceneRepository.registerFactory("gameover", [gameRouter, session, records, &keyboard]() {
        return std::make_unique<ForgeGameOverScene>(gameRouter, session, records, keyboard);
    });
    sceneRepository.registerFactory("records", [gameRouter, records, &keyboard]() {
        return std::make_unique<ForgeRecordsScene>(gameRouter, records, keyboard);
    });
    sceneRepository.registerFactory("exit", []() { return std::make_unique<ForgeExitScene>(); });
}
