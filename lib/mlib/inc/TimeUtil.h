#pragma once

#include <ctime>
#include <MLibException.h>

static std::time_t GetTimestamp(std::tm const& time)
{
    std::tm temp{time};
    return mktime(&temp);
}

static std::tm GetTime(std::time_t const& time)
{
    std::tm t{};
    if (localtime_s(&t, &time))
    {
        throw MLibException(E_INVALIDARG);
    }
    return t;
}

static std::tm GetTime(int year, int month, int day)
{
    std::tm t{0, 0, 0, day, month - 1, year - 1900};
    if (mktime(&t) == -1)
    {
        throw MLibException(E_INVALIDARG);
    }
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    return t;
};

static std::tm GetTime(int year, int month, int day, int hour, int minute, int second)
{
    std::tm t{GetTime(year, month, day)};
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = second;
    return t;
};

// Assume the input has not been run through mktime and does not follow the std::tm rules.
static std::tm GetTime(std::tm const& time)
{
    return GetTime(time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
}

static std::tm TrimTime(std::tm const& time)
{
    std::tm t{time};
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    return t;
}