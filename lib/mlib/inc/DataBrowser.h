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

namespace fs = std::experimental::filesystem;

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

class GoogleDataBrowser : public DataBrowser
{
public:
    static std::shared_ptr<GoogleDataBrowser> MakeShared(fs::path path)
    {
        return std::make_shared<GoogleDataBrowser>(path);
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
    GoogleDataBrowser(fs::path path)
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

class QuantQuoteDataBrowser : public DataBrowser
{
public:
    static std::shared_ptr<QuantQuoteDataBrowser> MakeShared(fs::path path)
    {
        return std::make_shared<QuantQuoteDataBrowser>(path);
    }

    // Expects path to point to a directory with a number other directories containing .txt files.
    //      \order_155998\
    //          \allstocks_20151123\
    //              table_aapl.csv
    //              table_aig.csv
    //              ...
    //
    // Each text file is in CSV format with the following columns:
    //      Date            : yyyymmdd
    //      Time            : hhmm (military time, EST)
    //                      : This is the time of the start of the interval.
    //                      : ie. 900 is between 9:00:00am and 9:00:59am inclusive
    //      Open
    //      High
    //      Low
    //      Close
    //      Volume
    //      Split Factor
    //      Earnings
    //      Dividends
    //      Extrapolation
    QuantQuoteDataBrowser(fs::path path)
    {
        if (!fs::exists(path) || !fs::is_directory(path))
        {
            throw MLibException(E_INVALIDARG);
        }

        auto indexPath = path / c_IndexFileName;
        if (fs::exists(indexPath))
        {
            std::ifstream ifs(indexPath.string(), std::ios::binary);
            boost::archive::binary_iarchive ar(ifs);
            Serialize<boost::archive::binary_iarchive>(ar);
            return;
        }

        for (auto const& entry : fs::directory_iterator(path))
        {
            if (!fs::is_directory(entry.status()))
            {
                continue;
            }

            std::tm date{};
            if (sscanf_s(entry.path().stem().string().c_str(), "allstocks_%4d%2d%2d", &date.tm_year, &date.tm_mon, &date.tm_mday) != 3)
            {
                continue;
            }

            date.tm_year -= 1900;
            date.tm_mon -= 1;

            auto dateKey = mktime(&date);

            for (auto const& subentry : fs::directory_iterator(entry.path()))
            {
                if (fs::is_directory(subentry.status()))
                {
                    continue;
                }
                
                if (subentry.path().extension() != ".csv")
                {
                    continue;
                }

                char symbol[5];
                if (sscanf_s(subentry.path().stem().string().c_str(), "table_%s", symbol, static_cast<uint32_t>(sizeof(symbol))) != 1)
                {
                    continue;
                }

                m_index[symbol][dateKey] = subentry.path().string();
            }
        }

        std::ofstream ofs(indexPath.string(), std::ios::binary);
        boost::archive::binary_oarchive ar(ofs);
        Serialize<boost::archive::binary_oarchive>(ar);
    }

#pragma region DataBrowser
    virtual std::vector<std::string> GetSymbols()
    {
        throw MLibException(E_NOTIMPL);
    }

    virtual BarResolution GetNativeBarResolution()
    {
        return BarResolution::Minute;
    }

    virtual std::vector<Bar> GetBars(std::string /*symbol*/, BarResolution /*resolution*/, std::time_t /*start*/, std::time_t /*end*/)
    {
        throw MLibException(E_NOTIMPL);
    }

    virtual std::vector<Bar> GetBars(std::string /*symbol*/, BarResolution /*resolution*/, uint32_t /*start*/, uint32_t /*end*/)
    {
        throw MLibException(E_NOTIMPL);
    }

    virtual uint32_t GetBarCount(std::string /*symbol*/, BarResolution /*resolution*/)
    {
        throw MLibException(E_NOTIMPL);
    }

    virtual std::pair<std::time_t, std::time_t> GetBarRange(std::string /*symbol*/, BarResolution /*resolution*/)
    {
        throw MLibException(E_NOTIMPL);
    }
#pragma endregion

private:
    const std::string c_IndexFileName{"QQDB_INDEX"};

    template<typename Archive>
    void Serialize(Archive& ar)
    {
        ar & m_index;
    }

    // m_index[symbol][date] = path_to_csv
    std::map<std::string, std::map<std::time_t, std::string>> m_index;
};
