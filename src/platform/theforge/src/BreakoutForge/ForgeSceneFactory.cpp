#include "ForgeSceneFactory.h"

#include <cengine/input/Keyboard.hpp>
#include <cengine/routing/ISceneRepository.hpp>

#include "breakout/game/GameRouter.h"

#include "scene/ForgeExitScene.h"
#include "scene/ForgeGameScene.h"
#include "scene/ForgeMenuScene.h"
#include "scene/ForgeSplashScene.h"

// As factories rodam LAZY (no primeiro getScene de cada estado) e a cena morre
// ao sair do estado (o router a descarrega no commit) — capturas por VALOR. O
// teclado da porta e capturado por REFERENCIA: ele vive no casco da plataforma,
// que sobrevive a todas as cenas.
void ForgeSceneFactory::populateForgeScenes(cengine::routing::ISceneRepository& sceneRepository,
                                            const std::shared_ptr<GameRouter>&  gameRouter,
                                            cengine::input::Keyboard&           keyboard)
{
    sceneRepository.registerFactory(
        "initial", [gameRouter, &keyboard]() { return std::make_unique<ForgeSplashScene>(gameRouter, keyboard); });
    sceneRepository.registerFactory(
        "menu", [gameRouter, &keyboard]() { return std::make_unique<ForgeMenuScene>(gameRouter, keyboard); });
    sceneRepository.registerFactory(
        "game", [gameRouter, &keyboard]() { return std::make_unique<ForgeGameScene>(gameRouter, keyboard); });
    sceneRepository.registerFactory("exit", []() { return std::make_unique<ForgeExitScene>(); });
}
