cmake_minimum_required(VERSION 3.10)
project(Menu)

# Указываем путь к заголовочным файлам
set(MENU_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/menu/src/include")

# Тестирование надо включать на самом верхнем уровне, иначе работать не будет
include(CTest)
enable_testing()

# Добавляем подпроекты
add_subdirectory(menu)
add_subdirectory(tests)

