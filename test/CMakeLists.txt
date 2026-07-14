# Configuracao dos testes (mesmo padrao dos irmaos e da cengine)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

add_executable(breakout_tests
    "${CMAKE_CURRENT_SOURCE_DIR}/breakout/game/WorldTest.cpp"
)

target_link_libraries(breakout_tests
  PRIVATE
  breakout_lib
  gtest_main
)

include(GoogleTest)

gtest_discover_tests(breakout_tests)
