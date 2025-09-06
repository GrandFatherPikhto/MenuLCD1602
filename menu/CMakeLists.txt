cmake_minimum_required(VERSION 3.5.0)

project(Menu VERSION 0.1.0 LANGUAGES C)

set(PROJECT_NAME menu)

set(EXEC_NAME ${PROJECT_NAME}_exec)
set(LIB_NAME ${PROJECT_NAME}_lib)

# Находим libconfig
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCONFIG REQUIRED libconfig)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

set(LIB_SOURCES 
    src/menu.c
    src/config.c
    # src/console.c
    # src/print.c
    src/current.c
    src/rotary_encoder.c
    src/lcd1602.c
    src/strategy_default.c
    src/strategy_submenu.c
    src/strategy_action.c
    src/strategy_toggle.c
    src/strategy_numeric.c
    )

set(EXEC_SOURCES
    ${LIB_SOURCES}
    src/main.c
)

set(INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
)

add_executable(${EXEC_NAME} 
    ${EXEC_SOURCES} 
    ${INCLUDES})

add_library(
    ${LIB_NAME} 
    STATIC 
    ${LIB_SOURCES} 
    ${INCLUDES})

target_link_libraries(${EXEC_NAME} PRIVATE
    ${LIB_NAME}
    ${LIBCONFIG_LIBRARIES}
    SDL2::SDL2 
    SDL2_ttf::SDL2_ttf
)

target_link_libraries(${LIB_NAME} PRIVATE
    ${LIBCONFIG_LIBRARIES}
)

target_include_directories(${EXEC_NAME} PRIVATE 
    ${INCLUDE_DIRECTORIES}
    ${LIBCONFIG_INCLUDES}
    ${SDL2_INCLUDE_DIRS}
)

target_include_directories(${LIB_NAME} PRIVATE
    ${INCLUDE_DIRECTORIES}
)

set_target_properties(${LIB_NAME} PROPERTIES
    VERSION 1.0
    SOVERSION 1
)

set_target_properties(${EXEC_NAME} PROPERTIES
    VERSION 1.0
    SOVERSION 1
)
