# Breakout — dominio C++ puro + testes (task 02).
#
# Mesmo desenho dos irmaos: a lib do jogo (dominio + maquina de estados) compila
# sem NENHUMA plataforma; a plataforma The-Forge builda a parte via MSBuild
# (src/platform/theforge/PC_VS2019). A cengine (via FetchContent) exige
# CMake >= 3.23.
cmake_minimum_required(VERSION 3.23)

project(Breakout)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BREAKOUT_BUILD_TESTS "Build the test suite" ON)

# -----------------------------
# Dependencia: cengine 0.8.0
#
# - routing:     GameRouter herda de FlowRouter, estados implementam IState
# - collision2d: a deteccao AABB. Este jogo e o consumidor VIVO que faltava —
#                a evidencia que promoveu o AABB vinha do spaceinvaders, que
#                esta congelado (ADR 0003).
# - input:       o vocabulario de teclado (a porta que este jogo valida)
# -----------------------------
include(FetchContent)

FetchContent_Declare(
  cengine
  GIT_REPOSITORY https://github.com/cengine-dev/cengine.git
  GIT_TAG 0.8.0
)

# Sem a suite de testes da cengine (evita fetch do GoogleTest dela).
set(CENGINE_BUILD_TESTS OFF)
FetchContent_MakeAvailable(cengine)

# -----------------------------
# breakout_lib — o dominio do jogo, linkavel pela suite de testes.
# -----------------------------
file(GLOB_RECURSE GAME_SOURCES
        "src/breakout/*.cpp" "src/breakout/*.h"
)

add_library(breakout_lib STATIC ${GAME_SOURCES})
target_include_directories(breakout_lib PUBLIC src)
target_link_libraries(breakout_lib PUBLIC
    cengine::core
    cengine::routing
    cengine::collision2d
)

# -----------------------------
# Testes (GoogleTest via CTest)
# -----------------------------
if(BREAKOUT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
