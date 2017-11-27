#pragma once

#include <string>
#include <vector>
#include <Bar.h>

class DataBrowser
{
public:
    virtual std::vector<std::string> GetSymbols() = 0;

    virtual BarResolution GetNativeBarResolution() = 0;

    virtual std::vector<Bar> GetBars(std::string symbol, BarResolution resolution, std::time_t start, std::time_t end) = 0;

    virtual std::vector<Bar> GetBars(std::string symbol, BarResolution resolution, uint32_t start, uint32_t end) = 0;

    virtual uint32_t GetBarCount(std::string symbol, BarResolution resolution) = 0;

    virtual std::pair<std::time_t, std::time_t> GetBarRange(std::string symbol, BarResolution resolution) = 0;

    std::vector<Bar> GetBar(std::string symbol, BarResolution resolution, std::time_t index)
    {
        return GetBars(symbol, resolution, index, index);
    }

    std::vector<Bar> GetBar(std::string symbol, BarResolution resolution, uint32_t index)
    {
        return GetBars(symbol, resolution, index, index);
    }
};
