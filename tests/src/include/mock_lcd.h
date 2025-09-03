#pragma once

#include <gmock/gmock.h>
#include "lcd1602.h"

class MockLcd {
public:
    MOCK_METHOD(void, printMenu, (const char*, const char*), ());
};

extern MockLcd* g_mockLcd;

// Заглушка для printMenu
void printMenu(const char* str1, const char* str2);