set(AEGIS_GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../libs/googletest" CACHE PATH "Path to vendored GoogleTest for aegis_engine")

if(NOT EXISTS ${AEGIS_GTEST_DIR}/CMakeLists.txt)
    message(FATAL_ERROR "Vendored GoogleTest not found at ${AEGIS_GTEST_DIR}")
endif()

add_subdirectory(${AEGIS_GTEST_DIR} ${CMAKE_BINARY_DIR}/_deps/googletest EXCLUDE_FROM_ALL)

file(GLOB AEGIS_TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

if(NOT CMAKE_CROSSCOMPILING)
    add_executable(aegis_test
        ${AEGIS_TEST_SOURCES}
    )

    target_link_libraries(aegis_test PRIVATE
        aegis_engine
        $<$<TARGET_EXISTS:GTest::gtest_main>:GTest::gtest_main>
        $<$<TARGET_EXISTS:gtest_main>:gtest_main>
    )

    include(GoogleTest)
    gtest_discover_tests(aegis_test)
endif()
