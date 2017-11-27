#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <map>
#include <MLibException.h>
#include <SimpleMatrix.h>
#include <Bar.h>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <DataBrowser.h>
#include <Tws.h>

namespace fs = std::experimental::filesystem;

class TwsDataBrowser : public DataBrowser
{
public:
    static std::shared_ptr<TwsDataBrowser> MakeShared(fs::path path)
    {
        return std::make_shared<TwsDataBrowser>(path);
    }

    // Expects path to point to a directory with a number of .txt files.
    // The name of each txt file is the stock symbol.
    // Each text file is in CSV format with the following columns:
    //      Timestamp
    //      Open
    //      High
    //      Low
    //      Close
    //      Volume
    TwsDataBrowser(fs::path path)
    {
        if (!fs::exists(path) || !fs::is_directory(path))
        {
            throw MLibException(E_INVALIDARG);
        }

        for (auto const& entry : fs::directory_iterator(path))
        {
            if (!fs::is_directory(entry.status()) && entry.path().has_stem() && (entry.path().extension() == ".txt"))
            {
                m_data[entry.path().stem().string()] = std::make_unique<SimpleMatrix<double>>(entry.path().string());
            }
        }
    }

#pragma region DataBrowser
    virtual std::vector<std::string> GetSymbols()
    {
        std::vector<std::string> v;
        for (auto const& data : m_data)
        {
            v.push_back(data.first);
        }
        return v;
    }

    virtual BarResolution GetNativeBarResolution()
    {
        return BarResolution::Minute;
    }

    virtual std::vector<Bar> GetBars(std::string /*symbol*/, BarResolution /*resolution*/, std::time_t /*start*/, std::time_t /*end*/)
    {
        throw MLibException(E_NOTIMPL);
    }

    virtual std::vector<Bar> GetBars(std::string symbol, BarResolution resolution, uint32_t start, uint32_t end)
    {
        if (m_data.find(symbol) == m_data.end())
        {
            throw MLibException(E_INVALIDARG);
        }

        if (end >= m_data[symbol]->NumRows())
        {
            throw MLibException(E_INVALIDARG);
        }

        if (resolution != BarResolution::Minute)
        {
            throw MLibException(E_NOTIMPL);
        }

        std::vector<Bar> b;

        auto const& data = *m_data[symbol].get();
        for (auto i = start; i <= end; i++)
        {
            b.push_back({symbol, static_cast<std::time_t>(data.Row(i)[0]), resolution, data.Row(i)[1], data.Row(i)[2], data.Row(i)[3], data.Row(i)[4], data.Row(i)[5]});
        }

        return b;
    }

    virtual uint32_t GetBarCount(std::string symbol, BarResolution resolution)
    {
        if (m_data.find(symbol) == m_data.end())
        {
            throw MLibException(E_INVALIDARG);
        }

        if (resolution != BarResolution::Minute)
        {
            throw MLibException(E_NOTIMPL);
        }

        return m_data[symbol]->NumRows();
    }

    virtual std::pair<std::time_t, std::time_t> GetBarRange(std::string symbol, BarResolution resolution)
    {
        if (m_data.find(symbol) == m_data.end())
        {
            throw MLibException(E_INVALIDARG);
        }

        if (resolution != BarResolution::Minute)
        {
            throw MLibException(E_NOTIMPL);
        }

        throw MLibException(E_NOTIMPL);
    }
#pragma endregion

private:
    std::map<std::string, std::unique_ptr<SimpleMatrix<double>>> m_data;
};
