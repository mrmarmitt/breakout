#include "ForgeSceneFactory.h"

#include <cengine/input/Keyboard.hpp>
#include <cengine/routing/ISceneRepository.hpp>

#include "breakout/game/GameRouter.h"
#include "breakout/game/service/PlaySession.h"

#include "scene/ForgeExitScene.h"
#include "scene/ForgeGameOverScene.h"
#include "scene/ForgeGameScene.h"
#include "scene/ForgeMenuScene.h"
#include "scene/ForgeSplashScene.h"

// As factories rodam LAZY (no primeiro getScene de cada estado) e a cena morre
// ao sair do estado (o router a descarrega no commit) — capturas por VALOR. Dai
// a partida nova nascer zerada de graca.
//
// O teclado da porta e capturado por REFERENCIA (vive no casco, que sobrevive a
// todas as cenas); o PlaySession, o que precisa ATRAVESSAR a troca de cena.
void ForgeSceneFactory::populateForgeScenes(cengine::routing::ISceneRepository& sceneRepository,
                                            const std::shared_ptr<GameRouter>&  gameRouter,
                                            const std::shared_ptr<PlaySession>& session,
                                            cengine::input::Keyboard&           keyboard)
{
    sceneRepository.registerFactory(
        "initial", [gameRouter, &keyboard]() { return std::make_unique<ForgeSplashScene>(gameRouter, keyboard); });
    sceneRepository.registerFactory(
        "menu", [gameRouter, &keyboard]() { return std::make_unique<ForgeMenuScene>(gameRouter, keyboard); });
    sceneRepository.registerFactory("game", [gameRouter, session, &keyboard]() {
        return std::make_unique<ForgeGameScene>(gameRouter, session, keyboard);
    });
    sceneRepository.registerFactory("gameover", [gameRouter, session, &keyboard]() {
        return std::make_unique<ForgeGameOverScene>(gameRouter, session, keyboard);
    });
    sceneRepository.registerFactory("exit", []() { return std::make_unique<ForgeExitScene>(); });
}
