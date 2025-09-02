#pragma once
#include <gmock/gmock.h>
#include "rotary_encoder.h"

class MockRotary {
public:
    MOCK_METHOD(void, rotaryCallback, (int current), ());
    MOCK_METHOD(void, buttonCallback, (), ());
    MOCK_METHOD(void, longButtonCallback, (), ());
};

extern MockRotary* g_mockRotary;
