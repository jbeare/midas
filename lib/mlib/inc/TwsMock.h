#pragma once

#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include <AsyncRequest.h>
#include <Tws.h>
#include <TwsDataBrowser.h>

#include <Contract.h>

class TwsMock : public Tws
{
public:
    static std::shared_ptr<Tws> MakeShared(
        _In_ std::shared_ptr<TwsDataBrowser> dataBrowser,
        _In_ std::vector<std::string> symbols,
        _In_ std::time_t start,
        _In_ std::time_t end)
    {
        return std::make_shared<TwsMock>(dataBrowser, symbols, start, end);
    }

    TwsMock(
        _In_ std::shared_ptr<TwsDataBrowser> dataBrowser,
        _In_ std::vector<std::string> symbols,
        _In_ std::time_t start,
        _In_ std::time_t end) :
        m_dataBrowser(dataBrowser),
        m_symbols(symbols),
        m_start(start),
        m_end(end),
        m_current(start)
    {

    }

    ~TwsMock()
    {

    }
    
#pragma region Tws
    virtual bool Connect(_In_ const std::string /*host*/, _In_ uint32_t /*port*/, _In_ int32_t /*clientId*/ = 0)
    {
        m_connected = true;
        return m_connected;
    }

    virtual void Disconnect()
    {
        m_connected = false;
    }

    virtual bool IsConnected() const
    {
        return m_connected;
    }

    virtual void SetConnectOptions(_In_ const std::string& /*connectOptions*/)
    {

    }

    virtual std::shared_ptr<AsyncResult<AccountSummary>> RequestAccountSummary(
        _In_ const std::string& /*groupName*/,
        _In_ const std::string& /*tags*/)
    {
        throw MLibException(E_NOTIMPL);
    }

    virtual std::shared_ptr<AsyncResult<void>> RequestMarketData(
        _In_ const Contract& /*contract*/,
        _In_ const std::string& /*genericTicks*/,
        _In_ bool /*snapshot*/,
        _In_ const TagValueListSPtr& /*mktDataOptions*/)
    {
        throw MLibException(E_NOTIMPL);
    }

    virtual std::shared_ptr<AsyncResult<HistoricalData>> RequestHistoricalMarketData(
        _In_ const Contract& /*contract*/,
        _In_ const std::string& /*endDateTime*/,
        _In_ const std::string& /*durationStr*/,
        _In_ const std::string& /*barSizeSetting*/,
        _In_ const std::string& /*whatToShow*/,
        _In_ int /*useRTH*/,
        _In_ int /*formatDate*/,
        _In_ const TagValueListSPtr& /*chartOptions*/)
    {
        throw MLibException(E_NOTIMPL);
    }
#pragma endregion

    std::vector<Bar> GetNextBars()
    {
        std::vector<Bar> bars;
        for (auto const& symbol : m_symbols)
        {
            std::vector<Bar> bar;
            while (bar.empty() && (m_current <= m_end))
            {
                bar = m_dataBrowser->GetBar(symbol, BarResolution::Minute, m_current);
                if (bar.empty())
                {
                    m_current += 60;
                }
            }
            bars.insert(bars.end(), bar.begin(), bar.end());
        }
        m_current += 60;
        return bars;
    }

private:
    std::shared_ptr<TwsDataBrowser> m_dataBrowser;
    std::vector<std::string> m_symbols;
    std::time_t m_start;
    std::time_t m_end;
    std::time_t m_current;
    bool m_connected{false};
};
