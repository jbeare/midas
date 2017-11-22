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

typedef uint64_t Timestamp;

struct Time
{
    Time() {}

    Time(uint8_t minutes, uint8_t hours, uint8_t day, uint8_t month, uint32_t year) :
        Minutes(minutes),
        Hours(hours),
        Day(day),
        Month(month),
        Year(year) {}

    Time(Timestamp timestamp)
    {
        Minutes = timestamp & 0xFF;
        Hours = (timestamp >> (1 * 8)) & 0xFF;
        Day = (timestamp >> (2 * 8)) & 0xFF;
        Month = (timestamp >> (3 * 8)) & 0xFF;
        Year = (timestamp >> (4 * 8)) & 0xFFFFFFFF;
    }

    uint8_t Minutes{0};
    uint8_t Hours{0};
    uint8_t Day{1};
    uint8_t Month{1};
    uint32_t Year{1900};

    Timestamp GetTimeStamp()
    {
        Timestamp timestamp{};
        timestamp += (Timestamp)Minutes;
        timestamp += (Timestamp)Hours << (1 * 8);
        timestamp += (Timestamp)Day << (2 * 8);
        timestamp += (Timestamp)Month << (3 * 8);
        timestamp += (Timestamp)Year << (4 * 8);
        return timestamp;
    }
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

            Time ts{};
            if (sscanf_s(entry.path().stem().string().c_str(), "allstocks_%4u%2hhu%2hhu", &ts.Year, &ts.Month, &ts.Day) != 3)
            {
                continue;
            }

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

                m_index[symbol][ts.GetTimeStamp()] = subentry.path().string();
            }
        }

        std::ofstream ofs(indexPath.string(), std::ios::binary);
        boost::archive::binary_oarchive ar(ofs);
        Serialize<boost::archive::binary_oarchive>(ar);
    }

#pragma region DataBrowser
    virtual std::vector<std::string> GetSymbols()
    {
        std::vector<std::string> v;
        for (auto const& data : m_index)
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

    virtual std::vector<Bar> GetBars(std::string symbol, BarResolution resolution, Timestamp start, Timestamp end)
    {
        if (m_index.find(symbol) == m_index.end())
        {
            throw MLibException(E_INVALIDARG);
        }

        if (resolution != BarResolution::Minute)
        {
            throw MLibException(E_NOTIMPL);
        }

        Time startTime(start), endTime(end);
        Time startDay(start), endDay(end);
        startDay.Hours = 0;
        startDay.Minutes = 0;
        endDay.Hours = 0;
        endDay.Minutes = 0;
        std::vector<Bar> bars;

        for (auto const& csv : m_index[symbol])
        {
            if ((csv.first >= startDay.GetTimeStamp()) && (csv.first <= endDay.GetTimeStamp()))
            {
                SimpleMatrix<double> matrix(csv.second);
                for (uint32_t i = 0; i < matrix.NumRows(); i++)
                {
                    auto const& row = matrix.Row(i);
                    uint8_t minutes = static_cast<uint8_t>(static_cast<uint16_t>(row[1]) % 100);
                    uint8_t hours = static_cast<uint8_t>((static_cast<uint16_t>(row[1]) - minutes) / 100);
                    Time time{csv.first};
                    time.Minutes = minutes;
                    time.Hours = hours;
                    Timestamp timestamp{time.GetTimeStamp()};

                    if ((timestamp >= start) && (timestamp <= end))
                    {
                        if ((row[1] >= 930) && (row[1] < 1600))
                        {
                            bars.push_back({symbol, static_cast<std::time_t>(timestamp), resolution, row[2], row[3], row[4], row[5], row[6]});
                        }
                    }
                }
            }
        }

        return std::move(bars);
    }

    virtual std::vector<Bar> GetBars(std::string /*symbol*/, BarResolution /*resolution*/, uint32_t /*start*/, uint32_t /*end*/)
    {
        throw MLibException(E_NOTIMPL);
    }

    virtual uint32_t GetBarCount(std::string /*symbol*/, BarResolution /*resolution*/)
    {
        throw MLibException(E_NOTIMPL);
    }

    virtual std::pair<std::time_t, std::time_t> GetBarRange(std::string symbol, BarResolution resolution)
    {
        if (m_index.find(symbol) == m_index.end())
        {
            throw MLibException(E_INVALIDARG);
        }

        if (resolution != BarResolution::Minute)
        {
            throw MLibException(E_NOTIMPL);
        }

        return std::pair<std::time_t, std::time_t>(m_index[symbol].begin()->first, (--m_index[symbol].end())->first + 86400);
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
    std::map<std::string, std::map<Timestamp, std::string>> m_index;
};
