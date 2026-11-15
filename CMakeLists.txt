cmake_minimum_required(VERSION 3.23)
project(aegis_engine LANGUAGES CXX)

# Add libraries
add_subdirectory(libs/entt EXCLUDE_FROM_ALL)
add_subdirectory(libs/flatbuffers EXCLUDE_FROM_ALL)

add_library(aegis_engine_lib INTERFACE)
add_library(aegis::engine ALIAS aegis_engine_lib)

# Add header libraries
target_compile_features(aegis_engine_lib INTERFACE cxx_std_20)
target_include_directories(aegis_engine_lib INTERFACE
    libs/etl/include
    libs/fpm/include
)
target_link_libraries(aegis_engine_lib INTERFACE
    EnTT::EnTT
    FlatBuffers::FlatBuffers
)

add_executable(aegis_engine source/main.cpp)
target_link_libraries(aegis_engine PRIVATE aegis::engine)
