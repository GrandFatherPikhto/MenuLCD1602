cmake_minimum_required(VERSION 3.5.0)

set(CMAKE_C_STANDARD 99) # Enable c99 standard
set(CMAKE_CXX_STANDARD 23) # Enable cpp standard

set(PROJECT_NAME menu_test)
set(TEST_COMMAND ${CMAKE_BINARY_DIR}/tests/${PROJECT_NAME})

project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES C CXX)

# find_package(PkgConfig)
# pkg_search_module(GTEST REQUIRED gtest_main)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1 # Or a newer stable release tag
)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PRIVATE 
        # src/tests.cc
        #src/config_test.cc
        #src/integration_test.cc
        src/menu_test.cc
        # src/rotary_test.cc
    )
target_link_libraries(${PROJECT_NAME}
    PRIVATE ${GTEST_LDFLAGS}
        menu_lib
        GTest::gtest_main
        GTest::gmock
    )
target_compile_options(${PROJECT_NAME} PRIVATE ${GTEST_CFLAGS})

get_property(MENU_DIRECTORY GLOBAL PROPERTY MENU_DIRECTORY)

target_include_directories(${PROJECT_NAME}
    PRIVATE 
        ${MENU_DIRECTORY}/src/include 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include 
        ${MENU_INCLUDE_DIR}
    )

# add_test(NAME TestMerged COMMAND ${TEST_COMMAND})
# add_test(NAME TestDirect COMMAND ${TEST_COMMAND})
# add_test(NAME TestPWMSequence COMMAND ${TEST_COMMAND})
# add_test(NAME ConfigTest COMMAND ${TEST_COMMAND})

message(STATUS "Команда тестирования: ${PROJECT_NAME}: ${TEST_COMMAND}")
message(STATUS "Текущая директория: ${PROJECT_NAME}: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Menu include dir: ${PROJECT_NAME}: ${MENU_INCLUDE_DIR}")

# Добавляем отдельные тесты для каждого файла
add_test(NAME ConfigTests COMMAND ${PROJECT_NAME} --gtest_filter="ConfigTest*")
add_test(NAME MenuTests COMMAND ${PROJECT_NAME} --gtest_filter="MenuTest*")
add_test(NAME RotaryTests COMMAND ${PROJECT_NAME} --gtest_filter="RotaryTest*")
add_test(NAME IntegrationTests COMMAND ${PROJECT_NAME} --gtest_filter="IntegrationTest*")

# Добавляем общий тест для запуска всех тестов
add_test(NAME AllTests COMMAND ${PROJECT_NAME})

# Устанавливаем свойства тестов
# set_tests_properties(ConfigTests PROPERTIES LABELS "unit;config")
# set_tests_properties(MenuTests PROPERTIES LABELS "unit;menu")
# set_tests_properties(RotaryTests PROPERTIES LABELS "unit;rotary")
# set_tests_properties(IntegrationTests PROPERTIES LABELS "integration")
# set_tests_properties(AllTests PROPERTIES LABELS "all")

# Для автоматического обнаружения GoogleTest тестов
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}
    EXTRA_ARGS "--gtest_output=xml:${CMAKE_BINARY_DIR}/test_results.xml"
    TEST_PREFIX "GTEST_"
    TEST_SUFFIX ""
    PROPERTIES LABELS "gtest_auto"
)