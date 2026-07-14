// main_theforge.cpp — entry point da plataforma The-Forge em MODO BIBLIOTECA +
// composition root do jogo (mesmo padrao dos irmaos): o main vive fora da
// plataforma e do pacote do jogo, e e onde todas as instancias e injecoes
// principais acontecem.
//
//   main()
//     -> initMemAlloc/initFileSystem/initLog   subsistemas de processo (na
//                                              ordem do WindowsMain)
//     -> composicao: repositorio de cenas -> router (estado inicial = splash)
//        -> GameRouter (FlowRouter da cengine + vocabulario) -> factories de
//        cena, que recebem o TECLADO DA PORTA (cengine::input::Keyboard)
//     -> EngineManager::owned(TheForgeWindowManager{desc}, GameManager)
//     -> engine.start()   loop da cengine: window.update() -> fases do jogo
//                         (input -> update(dt fixo) 0..N -> render) ->
//                         window.present(); shouldExit() -> cleanup()
//
// Novidade deste jogo: as CENAS consomem `cengine::input::Keyboard&` injetado
// (task 20 da cengine), em vez das funcoes globais do forgeui. A instancia e a
// do casco — quem CAPTURA (WndProc traduzindo VK_*) continua sendo a
// plataforma, e e ela que empurra para dentro do teclado da porta.
//
// Este main NAO e um alvo CMake: compila no BreakoutForge.vcxproj (MSBuild),
// que reutiliza a cadeia de build do The-Forge — dai os subsistemas de processo
// e os exports do Agility SDK aqui dentro.

// cengine + jogo — C++ puro, ANTES dos headers do The-Forge (IMemory.h por ultimo).
#include <cengine/core/EngineManager.hpp>
#include <cengine/routing/GameManager.hpp>
#include <cengine/routing/RouterInMemory.hpp>
#include <cengine/routing/SceneRepository.hpp>

#include "breakout/game/GameRouter.h"
#include "breakout/game/service/PlaySession.h"
#include "breakout/game/service/RecordService.h"
#include "breakout/game/service/repository/FileRecordRepository.h"
#include "breakout/game/state/StateGame.h"

#include "platform/theforge/src/BreakoutForge/ForgeSceneFactory.h"
#include "platform/theforge/src/BreakoutForge/audio/AudioPlayer.h"

// platform-theforge-common (checkout irmao; include path no vcxproj)
#include "ForgeUi.h"
#include "TheForgeWindowManager.h"

#include <memory>

#include "Common_3/OS/Interfaces/IOperatingSystem.h" // UINT dos exports do Agility
#include "Common_3/Utilities/Interfaces/IFileSystem.h"
#include "Common_3/Utilities/Interfaces/ILog.h"

#include "Common_3/Utilities/Interfaces/IMemory.h" // deve ser o ultimo include

// O DEFINE_APPLICATION_MAIN exportava estes simbolos para ativar o Agility SDK
// do D3D12 (D3D12Core.dll copiada para a pasta do exe). Sem IApp, exportamos por
// conta propria — D3D12_AGILITY_SDK_VERSION vem do TF_Shared.props.
extern "C"
{
    __declspec(dllexport) extern const UINT  D3D12SDKVersion = D3D12_AGILITY_SDK_VERSION;
    __declspec(dllexport) extern const char* D3D12SDKPath = "";
}

int main()
{
    constexpr const char* kAppName = "BreakoutForge";

    // Subsistemas de processo na ordem do WindowsMain; o resto (janela, GPU,
    // fontes) e responsabilidade do TheForgeWindowManager, dentro da cengine.
    if (!initMemAlloc(kAppName))
        return EXIT_FAILURE;

    FileSystemInitDesc fsDesc = {};
    fsDesc.pAppName = kAppName;
    if (!initFileSystem(&fsDesc))
        return EXIT_FAILURE;

    initLog(kAppName, DEFAULT_LOG_LEVEL);

    {
        // composicao do jogo: quem conhece o grafo inteiro (dominio + cenas +
        // engine) e so este arquivo
        auto sceneRepository = std::make_unique<cengine::routing::SceneRepository>();
        cengine::routing::ISceneRepository& sceneRepositoryRef = *sceneRepository;

        const auto router =
            std::make_shared<cengine::routing::RouterInMemory>(std::move(sceneRepository), std::make_unique<InitialSG>());

        const auto gameRouter = std::make_shared<GameRouter>(router);

        // O resultado da partida atravessa a troca de cena (o World morre com a
        // cena do jogo); o PlaySession e quem sobrevive para o gameOver ler.
        const auto session = std::make_shared<PlaySession>();

        // Audio: XAudio2 com os sons sintetizados na inicializacao (nenhum
        // arquivo de audio no repo). Fica NO JOGO — nem a cengine nem o common
        // teriam evidencia para uma ponte de audio hoje (ADR 0002: nenhum outro
        // jogo do ecossistema tem som). Sem placa de som, o jogo roda mudo.
        AudioPlayer audio;
        if (!audio.init())
        {
            // O AudioPlayer nao loga (os headers do XAudio2 e do The-Forge brigam
            // por _WIN32_WINNT) — ele devolve o HRESULT, e quem loga e aqui.
            LOGF(eWARNING, "[breakout] sem audio: o jogo roda mudo (hr=0x%08X)", audio.lastError());
        }

        // Recordes: politica do jogo (ADR 0002 da cengine — nunca sobe para a
        // engine). O TSV fica ao lado do exe, junto com o gpu.cfg e as fontes.
        const auto records =
            std::make_shared<brk::RecordService>(std::make_shared<brk::FileRecordRepository>("records.tsv"));

        // O teclado da PORTA (cengine::input): o casco captura e empurra, as
        // cenas leem. Vive no ForgeUi (sobrevive a todas as cenas).
        ForgeSceneFactory::populateForgeScenes(sceneRepositoryRef, gameRouter, session, records, forgeui::keyboard(),
                                               audio);

        // Casco do common: fonte do The-Forge + o batcher de SPRITES ligado (o
        // atlas do jogo, gerado por tools/make-atlas-dds.ps1 e resolvido pelo
        // RD_TEXTURES do PathStatement). Este jogo e quem traz o forgesprite de
        // volta a vida — desde que o spaceinvaders congelou e o asteroids foi de
        // wireframe, nenhum jogo vivo o compilava.
        //
        // O batcher de LINHAS fica desligado: aqui nao ha wireframe.
        TheForgeWindowDesc windowDesc = {};
        windowDesc.appName = kAppName;
        windowDesc.width = 1280;
        windowDesc.height = 720;
        windowDesc.fontPath = "TitilliumText/TitilliumText-Bold.otf";
        windowDesc.sprites.atlasPath = "atlas.dds";
        windowDesc.lines.enabled = false;

        // Modo PROPRIO por construcao: a cengine dirige o loop e o The-Forge
        // entra como biblioteca atras do IWindowManager.
        auto engine = cengine::core::EngineManager::owned(
            std::make_unique<TheForgeWindowManager>(windowDesc),
            std::make_unique<cengine::routing::GameManager>(router));

        engine.start(); // bloqueia ate o jogo rotear para "exit"; o cleanup()
                        // (jogo + janela/GPU) roda no fim do start()
    }

    LOGF(eINFO, "[breakout] loop da cengine encerrado");

    exitLog();
    exitFileSystem();
    exitMemAlloc();
    return 0;
}
