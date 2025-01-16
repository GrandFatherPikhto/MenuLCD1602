cmake_minimum_required(VERSION 3.5.0)

project(Menu VERSION 0.1.0 LANGUAGES C)

set(SOURCES 
    main.c
    console.c
    menu.c
    items.c
    )

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)    

# Находим libconfig
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCONFIG REQUIRED libconfig)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR} ${LIBCONFIG_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LIBCONFIG_LIBRARIES})
