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
#include <TimeUtil.h>

namespace fs = std::experimental::filesystem;

class TwsDataBrowser : public DataBrowser
{
public:
    static std::shared_ptr<TwsDataBrowser> MakeShared(fs::path path)
    {
        return std::make_shared<TwsDataBrowser>(path);
    }

// Expects path to point to a directory with a number other directories containing .txt files.
//      \order_155998\
//          \allstocks_20151123\
//              table_aapl.csv
//              table_aig.csv
//              ...
//
// Each text file is in CSV format with the following columns:
//      Timestamp
//      Open
//      High
//      Low
//      Close
//      Volume
    TwsDataBrowser(fs::path path) :
        m_path(path)
    {
        if (!fs::exists(path) || !fs::is_directory(path))
        {
            throw MLibException(E_INVALIDARG);
        }

        /*auto indexPath = path / c_IndexFileName;
        if (fs::exists(indexPath))
        {
            std::ifstream ifs(indexPath.string(), std::ios::binary);
            boost::archive::binary_iarchive ar(ifs);
            Serialize<boost::archive::binary_iarchive>(ar);
            return;
        }*/

        BuildIndex();

        /*std::ofstream ofs(indexPath.string(), std::ios::binary);
        boost::archive::binary_oarchive ar(ofs);
        Serialize<boost::archive::binary_oarchive>(ar);*/
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

    virtual std::vector<Bar> GetBars(std::string symbol, BarResolution resolution, std::time_t start, std::time_t end)
    {
        if (CacheBars(symbol, resolution, start, end))
        {
            BuildIndex();
        }

        if (m_index.find(symbol) == m_index.end())
        {
            throw MLibException(E_INVALIDARG);
        }

        if (resolution != BarResolution::Minute)
        {
            throw MLibException(E_NOTIMPL);
        }

        std::tm startTm{TrimTime(GetTime(start))}, endTm{TrimTime(GetTime(end))};
        std::time_t startDay{mktime(&startTm)}, endDay{mktime(&endTm)};
        std::vector<Bar> bars;

        for (auto const& csv : m_index[symbol])
        {
            if ((csv.first >= startDay) && (csv.first <= endDay))
            {
                if (!csv.second.empty())
                {
                    SimpleMatrix<double> matrix(csv.second);
                    for (uint32_t i = 0; i < matrix.NumRows(); i++)
                    {
                        auto const& row = matrix.Row(i);
                        bars.push_back({symbol, static_cast<std::time_t>(row[0]), resolution, row[1], row[2], row[3], row[4], row[5]});
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
    const std::string c_IndexFileName{"TWSDB_INDEX"};

    void BuildIndex()
    {
        m_index.clear();

        for (auto const& entry : fs::directory_iterator(m_path))
        {
            if (!fs::is_directory(entry.status()))
            {
                continue;
            }

            std::tm time{};
            if (sscanf_s(entry.path().stem().string().c_str(), "allstocks_%4d%2d%2d", &time.tm_year, &time.tm_mon, &time.tm_mday) != 3)
            {
                continue;
            }
            std::time_t timestamp(GetTimestamp(GetTime(time)));

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

                m_index[symbol][timestamp] = subentry.path().string();
            }
        }
    }

    // Returns true if any bars were cached.
    bool CacheBars(std::string symbol, BarResolution resolution, std::time_t start, std::time_t end)
    {
        if (resolution != BarResolution::Minute)
        {
            throw MLibException(E_NOTIMPL);
        }

        std::shared_ptr<Tws> tws;
        bool cached{false};

        std::tm startTm{TrimTime(GetTime(start))}, endTm{TrimTime(GetTime(end))};
        std::time_t startDay{mktime(&startTm)}, endDay{mktime(&endTm)};

        while (startDay <= endDay)
        {
            if ((m_index.find(symbol) == m_index.end()) || (m_index[symbol].find(startDay) == m_index[symbol].end()))
            {
                if (!tws)
                {
                    tws = Tws::MakeShared();
                    THROW_HR_IF_FAILED_BOOL(tws->Connect("127.0.0.1", 7497));
                }

                Contract contract;
                contract.symbol = symbol;
                contract.secType = "STK";
                contract.currency = "USD";
                contract.exchange = "SMART";
                contract.primaryExchange = "ISLAND";

                std::tm queryTm{GetTime(startDay)};
                queryTm.tm_mday++;
                mktime(&queryTm);
                char queryTime[80];
                std::strftime(queryTime, 80, "%Y%m%d %H:%M:%S", &queryTm);

                auto result = tws->RequestHistoricalMarketData(contract, queryTime, "1 D", "1 min", "TRADES", 1, 1, TagValueListSPtr());
                auto data = result->GetResultSync();
                std::vector<Bar> bars;

                for (auto const& row : data)
                {
                    std::tm time{};
                    if (sscanf_s(std::get<0>(row).c_str(), "%4d%2d%2d  %2d:%2d:%2d",
                        &time.tm_year, &time.tm_mon, &time.tm_mday, &time.tm_hour, &time.tm_min, &time.tm_sec) != 6)
                    {
                        continue;
                    }
                    std::time_t timestamp{GetTimestamp(GetTime(time))};

                    if (startDay > timestamp)
                    {
                        // If the bar was from a prior day, then this must be a non-trading day.
                        break;
                    }

                    bars.push_back({symbol, timestamp, resolution, std::get<1>(row), std::get<2>(row), std::get<3>(row), std::get<4>(row), static_cast<double>(std::get<5>(row))});
                }

                char timeStr[80];
                std::strftime(timeStr, 80, "\\allstocks_%Y%m%d\\", &startTm);

                auto path = m_path.string();
                path += timeStr;

                if (!fs::exists(path) || !fs::is_directory(path))
                {
                    fs::create_directory(path);
                }

                path += "table_";
                path += symbol;
                path += ".csv";

                if (!bars.empty())
                {
                    auto matrix = SimpleMatrixFromBarVector(bars);
                    matrix.Save(path);
                }
                else
                {
                    std::ofstream ofs;
                    ofs.open(path);
                    ofs.close();
                }

                cached = true;
            }

            startTm = GetTime(startDay);
            startTm.tm_mday++;
            startDay = mktime(&startTm);
        }

        if (tws)
        {
            tws->Disconnect();
        }

        return cached;
    }

    template<typename Archive>
    void Serialize(Archive& ar)
    {
        ar & m_index;
    }

    // m_index[symbol][date] = path_to_csv
    std::map<std::string, std::map<std::time_t, std::string>> m_index;
    fs::path m_path;
};
