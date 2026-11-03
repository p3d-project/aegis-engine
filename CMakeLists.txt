cmake_minimum_required(VERSION 3.23)

project(aegis_engine LANGUAGES CXX)

option(AEGIS_ENGINE_BUILD_TESTS "Build Aegis Engine tests" ON)

add_library(aegis_engine INTERFACE)
add_library(aegis::engine ALIAS aegis_engine)

file(GLOB_RECURSE AEGIS_ENGINE_PUBLIC_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

target_compile_features(aegis_engine INTERFACE cxx_std_17)

target_include_directories(aegis_engine
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/etl/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs/fpm/include>
)

target_sources(aegis_engine
    INTERFACE
    FILE_SET aegis_engine_headers TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES ${AEGIS_ENGINE_PUBLIC_HEADERS}
)

if(AEGIS_ENGINE_BUILD_TESTS)
    include(CTest)
endif()

if(AEGIS_ENGINE_BUILD_TESTS AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
