#pragma once

#include <ctime>
#include <string>

enum class BarResolution
{
    Second,
    Minute,
    Hour,
    Day
};

struct Bar
{
    std::string Symbol;
    std::time_t Timestamp;
    BarResolution Resolution;
    double Open;
    double High;
    double Low;
    double Close;
    double Volume;
};
