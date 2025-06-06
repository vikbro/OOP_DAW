cmake_minimum_required(VERSION 3.25)
project(daw)

set(CMAKE_CXX_STANDARD 17)

add_executable(daw main.cpp Audio.cpp Silence.cpp FileAudio.cpp AudioFactory.cpp Utils.cpp Utils.hpp Effects/EffectOpeation.hpp Effects/AmplifyEffect.cpp Effects/AmplifyEffect.hpp Effects/FadeInOperation.cpp Effects/FadeInOperation.hpp Effect.hpp Generators/Generator.cpp Generators/Generator.hpp Track.cpp Track.hpp Effect.cpp)
