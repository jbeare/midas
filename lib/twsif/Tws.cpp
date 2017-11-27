#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <deque>
#include <vector>
#include <algorithm>
#include <memory>
#include <map>
#include <iostream>
#include <thread>

#define IB_WIN32
#define TWSAPIDLLEXP

#include <CommissionReport.h>
#include <EClientSocket.h>
#include <EReader.h>
#include <EReaderOSSignal.h>
#include <EWrapper.h>
#include <Execution.h>
#include <Order.h>
#include <OrderState.h>

#include "Tws.h"

namespace std
{
    template<typename K, typename V>
    bool key_exists(map<K, V> m, K k)
    {
        return m.find(k) != m.end();
    }
}

class TwsImpl : public Tws, public EWrapper, public std::enable_shared_from_this<TwsImpl>
{
public:
    TwsImpl();
    virtual ~TwsImpl();

    virtual bool Connect(_In_ const std::string host, _In_ uint32_t port, _In_ int32_t clientId = 0);
    virtual void Disconnect();
    virtual bool IsConnected() const;
    virtual void SetConnectOptions(_In_ const std::string& connectOptions);

    int GetNextRequestId();

    virtual std::shared_ptr<AsyncResult<AccountSummary>> RequestAccountSummary(
        _In_ const std::string& groupName,
        _In_ const std::string& tags);

    virtual std::shared_ptr<AsyncResult<void>> RequestMarketData(
        _In_ const Contract& contract,
        _In_ const std::string& genericTicks,
        _In_ bool snapshot,
        _In_ const TagValueListSPtr& mktDataOptions);

    virtual std::shared_ptr<AsyncResult<HistoricalData>> RequestHistoricalMarketData(
        _In_ const Contract& contract,
        _In_ const std::string& endDateTime,
        _In_ const std::string& durationStr,
        _In_ const std::string& barSizeSetting,
        _In_ const std::string& whatToShow,
        _In_ int useRTH,
        _In_ int formatDate,
        _In_ const TagValueListSPtr& chartOptions);

private:
    // TODO: Do access to these need to be guarded? ie. will EWrapper serialize the callbacks?
    AccountSummaryMap m_accountSummaryMap;
    HistoricalDataMap m_historicalDataMap;

    EReaderOSSignal m_osSignal;
    std::unique_ptr<EClientSocket> m_client;
    std::unique_ptr<EReader> m_reader;
    std::thread m_thread;

#pragma region EWrapper
    virtual void tickPrice(TickerId tickerId, TickType field, double price, int canAutoExecute);
    virtual void tickSize(TickerId tickerId, TickType field, int size);
    virtual void tickOptionComputation(TickerId tickerId, TickType tickType, double impliedVol, double delta, double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice);
    virtual void tickGeneric(TickerId tickerId, TickType tickType, double value);
    virtual void tickString(TickerId tickerId, TickType tickType, const std::string& value);
    virtual void tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints, double totalDividends, int holdDays, const std::string& futureLastTradeDate, double dividendImpact, double dividendsToLastTradeDate);
    virtual void orderStatus(OrderId orderId, const std::string& status, double filled, double remaining, double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId, const std::string& whyHeld);
    virtual void openOrder(OrderId orderId, const Contract&, const Order&, const OrderState&);
    virtual void openOrderEnd();
    virtual void winError(const std::string& str, int lastError);
    virtual void connectionClosed();
    virtual void updateAccountValue(const std::string& key, const std::string& val, const std::string& currency, const std::string& accountName);
    virtual void updatePortfolio(const Contract& contract, double position, double marketPrice, double marketValue, double averageCost, double unrealizedPNL, double realizedPNL, const std::string& accountName);
    virtual void updateAccountTime(const std::string& timeStamp);
    virtual void accountDownloadEnd(const std::string& accountName);
    virtual void nextValidId(OrderId orderId);
    virtual void contractDetails(int reqId, const ContractDetails& contractDetails);
    virtual void bondContractDetails(int reqId, const ContractDetails& contractDetails);
    virtual void contractDetailsEnd(int reqId);
    virtual void execDetails(int reqId, const Contract& contract, const Execution& execution);
    virtual void execDetailsEnd(int reqId);
    virtual void error(const int id, const int errorCode, const std::string errorString);
    virtual void updateMktDepth(TickerId id, int position, int operation, int side, double price, int size);
    virtual void updateMktDepthL2(TickerId id, int position, std::string marketMaker, int operation, int side, double price, int size);
    virtual void updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage, const std::string& originExch);
    virtual void managedAccounts(const std::string& accountsList);
    virtual void receiveFA(faDataType pFaDataType, const std::string& cxml);
    virtual void historicalData(TickerId reqId, const std::string& date, double open, double high, double low, double close, int volume, int barCount, double WAP, int hasGaps);
    virtual void scannerParameters(const std::string& xml);
    virtual void scannerData(int reqId, int rank, const ContractDetails& contractDetails, const std::string& distance, const std::string& benchmark, const std::string& projection, const std::string& legsStr);
    virtual void scannerDataEnd(int reqId);
    virtual void realtimeBar(TickerId reqId, long time, double open, double high, double low, double close, long volume, double wap, int count);
    virtual void currentTime(long time);
    virtual void fundamentalData(TickerId reqId, const std::string& data);
    virtual void deltaNeutralValidation(int reqId, const UnderComp& underComp);
    virtual void tickSnapshotEnd(int reqId);
    virtual void marketDataType(TickerId reqId, int marketDataType);
    virtual void commissionReport(const CommissionReport& commissionReport);
    virtual void position(const std::string& account, const Contract& contract, double position, double avgCost);
    virtual void positionEnd();
    virtual void accountSummary(int reqId, const std::string& account, const std::string& tag, const std::string& value, const std::string& currency);
    virtual void accountSummaryEnd(int reqId);
    virtual void verifyMessageAPI(const std::string& apiData);
    virtual void verifyCompleted(bool isSuccessful, const std::string& errorText);
    virtual void displayGroupList(int reqId, const std::string& groups);
    virtual void displayGroupUpdated(int reqId, const std::string& contractInfo);
    virtual void verifyAndAuthMessageAPI(const std::string& apiData, const std::string& xyzChallange);
    virtual void verifyAndAuthCompleted(bool isSuccessful, const std::string& errorText);
    virtual void connectAck();
    virtual void positionMulti(int reqId, const std::string& account, const std::string& modelCode, const Contract& contract, double pos, double avgCost);
    virtual void positionMultiEnd(int reqId);
    virtual void accountUpdateMulti(int reqId, const std::string& account, const std::string& modelCode, const std::string& key, const std::string& value, const std::string& currency);
    virtual void accountUpdateMultiEnd(int reqId);
    virtual void securityDefinitionOptionalParameter(int reqId, const std::string& exchange, int underlyingConId, const std::string& tradingClass, const std::string& multiplier, std::set<std::string> expirations, std::set<double> strikes);
    virtual void securityDefinitionOptionalParameterEnd(int reqId);
    virtual void softDollarTiers(int reqId, const std::vector<SoftDollarTier> &tiers);
#pragma endregion
};

std::shared_ptr<Tws> Tws::MakeShared()
{
    return std::make_shared<TwsImpl>();
}

TwsImpl::TwsImpl() :
    m_osSignal(2000),
    m_client(new EClientSocket(this, &m_osSignal))
{

}

TwsImpl::~TwsImpl()
{
    if (IsConnected())
    {
        Disconnect();
    }
}

_Use_decl_annotations_
bool TwsImpl::Connect(const std::string host, uint32_t port, int32_t clientId)
{
    printf("Connecting to %s:%d clientId:%d\n", !host.empty() ? "127.0.0.1" : host.c_str(), port, clientId);

    bool res = m_client->eConnect(host.c_str(), port, clientId);

    if (res)
    {
        printf("Connected to %s:%d clientId:%d\n", m_client->host().c_str(), m_client->port(), clientId);
        m_reader.reset(new EReader(m_client.get(), &m_osSignal));
        m_reader->start();
        m_thread = std::thread([&]()
        {
            while (IsConnected())
            {
                m_reader->checkClient();
                m_osSignal.waitForSignal();
                m_reader->processMsgs();
            }
        });
    }
    else
    {
        printf("Cannot connect to %s:%d clientId:%d\n", m_client->host().c_str(), m_client->port(), clientId);
    }

    return res;
}

void TwsImpl::Disconnect()
{
    m_client->eDisconnect();
    if (m_thread.joinable())
    {
        m_thread.join();
    }
    printf("Disconnected\n");
}

bool TwsImpl::IsConnected() const
{
    return m_client->isConnected();
}

_Use_decl_annotations_
void TwsImpl::SetConnectOptions(const std::string& connectOptions)
{
    m_client->setConnectOptions(connectOptions);
}

int TwsImpl::GetNextRequestId()
{
    // TODO: What to do when this wraps around?
    static volatile long reqId{0};
    return InterlockedIncrement(&reqId);
}

_Use_decl_annotations_
std::shared_ptr<AsyncResult<TwsImpl::AccountSummary>> TwsImpl::RequestAccountSummary(const std::string& groupName, const std::string& tags)
{
    auto reqId = TwsImpl::GetNextRequestId();

    auto req = AsyncRequest<TwsImpl::AccountSummary>::MakeShared([&]() -> bool
    {
        if (std::key_exists(m_accountSummaryMap, reqId))
        {
            m_client->cancelAccountSummary(reqId);
            m_accountSummaryMap.erase(reqId);
            return true;
        }
        return false;
    });

    m_accountSummaryMap[reqId] = {req, {}};
    m_client->reqAccountSummary(reqId, groupName, tags);

    return req->GetAsyncResult();
}

_Use_decl_annotations_
std::shared_ptr<AsyncResult<void>> TwsImpl::RequestMarketData(
    const Contract& contract,
    const std::string& genericTicks,
    bool snapshot,
    const TagValueListSPtr& mktDataOptions)
{
    auto reqId = TwsImpl::GetNextRequestId();

    m_client->reqMktData(reqId, contract, genericTicks, snapshot, mktDataOptions);

    Sleep(100);

    m_client->cancelMktData(reqId);

    return nullptr;
}

_Use_decl_annotations_
std::shared_ptr<AsyncResult<TwsImpl::HistoricalData>> TwsImpl::RequestHistoricalMarketData(
    const Contract& contract,
    const std::string& endDateTime,
    const std::string& durationStr,
    const std::string& barSizeSetting,
    const std::string& whatToShow,
    int useRTH,
    int formatDate,
    const TagValueListSPtr& chartOptions)
{
    auto reqId = TwsImpl::GetNextRequestId();

    auto req = AsyncRequest<TwsImpl::HistoricalData>::MakeShared([&]() -> bool
    {
        if (std::key_exists(m_historicalDataMap, reqId))
        {
            m_client->cancelHistoricalData(reqId);
            m_historicalDataMap.erase(reqId);
            return true;
        }
        return false;
    });

    m_historicalDataMap[reqId] = {req,{}};

    m_client->reqHistoricalData(reqId, contract, endDateTime, durationStr, barSizeSetting, whatToShow, useRTH, formatDate, chartOptions);

    return req->GetAsyncResult();
}

#pragma region EWrapper
void TwsImpl::connectAck()
{
    printf("Connect ack.\n");

    if (m_client->asyncEConnect())
    {
        m_client->startApi();
    }
}

void TwsImpl::nextValidId(OrderId orderId)
{
    printf("Next Valid Id: %ld\n", orderId);
}

void TwsImpl::currentTime(long time)
{
    printf("Time: %d\n", time);
}

void TwsImpl::error(const int id, const int errorCode, const std::string errorString)
{
    printf("Error. Id: %d, Code: %d, Msg: %s\n", id, errorCode, errorString.c_str());
}

void TwsImpl::tickPrice(TickerId tickerId, TickType field, double price, int canAutoExecute)
{
    printf("Tick Price. Ticker Id: %ld, Field: %d, Price: %g, CanAutoExecute: %d\n", tickerId, (int)field, price, canAutoExecute);
}

void TwsImpl::tickSize(TickerId tickerId, TickType field, int size)
{
    printf("Tick Size. Ticker Id: %ld, Field: %d, Size: %d\n", tickerId, (int)field, size);
}

void TwsImpl::tickOptionComputation(TickerId tickerId, TickType tickType, double impliedVol, double delta,
    double optPrice, double pvDividend,
    double gamma, double vega, double theta, double undPrice)
{
    printf("TickOptionComputation. Ticker Id: %ld, Type: %d, ImpliedVolatility: %g, Delta: %g, OptionPrice: %g, pvDividend: %g, Gamma: %g, Vega: %g, Theta: %g, Underlying Price: %g\n", tickerId, (int)tickType, impliedVol, delta, optPrice, pvDividend, gamma, vega, theta, undPrice);
}

void TwsImpl::tickGeneric(TickerId tickerId, TickType tickType, double value)
{
    printf("Tick Generic. Ticker Id: %ld, Type: %d, Value: %g\n", tickerId, (int)tickType, value);
}

void TwsImpl::tickString(TickerId tickerId, TickType tickType, const std::string& value)
{
    printf("Tick String. Ticker Id: %ld, Type: %d, Value: %s\n", tickerId, (int)tickType, value.c_str());
}

void TwsImpl::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints,
    double totalDividends, int holdDays, const std::string& futureLastTradeDate, double dividendImpact, double dividendsToLastTradeDate)
{
    printf("TickEFP. %ld, Type: %d, BasisPoints: %g, FormattedBasisPoints: %s, Total Dividends: %g, HoldDays: %d, Future Last Trade Date: %s, Dividend Impact: %g, Dividends To Last Trade Date: %g\n", tickerId, (int)tickType, basisPoints, formattedBasisPoints.c_str(), totalDividends, holdDays, futureLastTradeDate.c_str(), dividendImpact, dividendsToLastTradeDate);
}

void TwsImpl::orderStatus(OrderId orderId, const std::string& status, double filled,
    double remaining, double avgFillPrice, int permId, int /*parentId*/,
    double lastFillPrice, int clientId, const std::string& whyHeld)
{
    printf("OrderStatus. Id: %ld, Status: %s, Filled: %g, Remaining: %g, AvgFillPrice: %g, PermId: %d, LastFillPrice: %g, ClientId: %d, WhyHeld: %s\n", orderId, status.c_str(), filled, remaining, avgFillPrice, permId, lastFillPrice, clientId, whyHeld.c_str());
}

void TwsImpl::openOrder(OrderId orderId, const Contract& contract, const Order& order, const OrderState& ostate)
{
    printf("OpenOrder. ID: %ld, %s, %s @ %s: %s, %s, %g, %s\n", orderId, contract.symbol.c_str(), contract.secType.c_str(), contract.exchange.c_str(), order.action.c_str(), order.orderType.c_str(), order.totalQuantity, ostate.status.c_str());
}

void TwsImpl::openOrderEnd()
{
    printf("OpenOrderEnd\n");
}

void TwsImpl::winError(const std::string& /*str*/, int /*lastError*/) {}

void TwsImpl::connectionClosed()
{
    printf("Connection Closed\n");
}

void TwsImpl::updateAccountValue(const std::string& key, const std::string& val,
    const std::string& currency, const std::string& accountName)
{
    printf("UpdateAccountValue. Key: %s, Value: %s, Currency: %s, Account Name: %s\n", key.c_str(), val.c_str(), currency.c_str(), accountName.c_str());
}

void TwsImpl::updatePortfolio(const Contract& contract, double position,
    double marketPrice, double marketValue, double averageCost,
    double unrealizedPNL, double realizedPNL, const std::string& accountName)
{
    printf("UpdatePortfolio. %s, %s @ %s: Position: %g, MarketPrice: %g, MarketValue: %g, AverageCost: %g, UnrealisedPNL: %g, RealisedPNL: %g, AccountName: %s\n", (contract.symbol).c_str(), (contract.secType).c_str(), (contract.primaryExchange).c_str(), position, marketPrice, marketValue, averageCost, unrealizedPNL, realizedPNL, accountName.c_str());
}

void TwsImpl::updateAccountTime(const std::string& timeStamp)
{
    printf("UpdateAccountTime. Time: %s\n", timeStamp.c_str());
}

void TwsImpl::accountDownloadEnd(const std::string& accountName)
{
    printf("Account download finished: %s\n", accountName.c_str());
}

void TwsImpl::contractDetails(int reqId, const ContractDetails& contractDetails)
{
    printf("ContractDetails. ReqId: %d - %s, %s, ConId: %ld @ %s, Trading Hours: %s, Liquidation Hours: %s\n", reqId, contractDetails.summary.symbol.c_str(), contractDetails.summary.secType.c_str(), contractDetails.summary.conId, contractDetails.summary.exchange.c_str(), contractDetails.tradingHours.c_str(), contractDetails.liquidHours.c_str());
}

void TwsImpl::bondContractDetails(int reqId, const ContractDetails& contractDetails)
{
    printf("Bond. ReqId: %d, Symbol: %s, Security Type: %s, Currency: %s, Trading Hours: %s, Liquidation Hours: %s\n", reqId, contractDetails.summary.symbol.c_str(), contractDetails.summary.secType.c_str(), contractDetails.summary.currency.c_str(), contractDetails.tradingHours.c_str(), contractDetails.liquidHours.c_str());
}

void TwsImpl::contractDetailsEnd(int reqId)
{
    printf("ContractDetailsEnd. %d\n", reqId);
}

void TwsImpl::execDetails(int reqId, const Contract& contract, const Execution& execution)
{
    printf("ExecDetails. ReqId: %d - %s, %s, %s - %s, %ld, %g\n", reqId, contract.symbol.c_str(), contract.secType.c_str(), contract.currency.c_str(), execution.execId.c_str(), execution.orderId, execution.shares);
}

void TwsImpl::execDetailsEnd(int reqId)
{
    printf("ExecDetailsEnd. %d\n", reqId);
}

void TwsImpl::updateMktDepth(TickerId id, int position, int operation, int side,
    double price, int size)
{
    printf("UpdateMarketDepth. %ld - Position: %d, Operation: %d, Side: %d, Price: %g, Size: %d\n", id, position, operation, side, price, size);
}

void TwsImpl::updateMktDepthL2(TickerId id, int position, std::string marketMaker, int operation,
    int side, double price, int size)
{
    printf("UpdateMarketDepthL2. %ld - Position: %d, Operation: %d, Side: %d, Price: %g, Size: %d\n", id, position, operation, side, price, size);
}

void TwsImpl::updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage, const std::string& originExch)
{
    printf("News Bulletins. %d - Type: %d, Message: %s, Exchange of Origin: %s\n", msgId, msgType, newsMessage.c_str(), originExch.c_str());
}

void TwsImpl::managedAccounts(const std::string& accountsList)
{
    printf("Account List: %s\n", accountsList.c_str());
}

void TwsImpl::receiveFA(faDataType pFaDataType, const std::string& cxml)
{
    std::cout << "Receiving FA: " << (int)pFaDataType << std::endl << cxml << std::endl;
}

void TwsImpl::historicalData(TickerId reqId, const std::string& date, double open, double high,
    double low, double close, int volume, int barCount, double WAP, int hasGaps)
{
    printf("HistoricalData. ReqId: %ld - Date: %s, Open: %g, High: %g, Low: %g, Close: %g, Volume: %d, Count: %d, WAP: %g, HasGaps: %d\n", reqId, date.c_str(), open, high, low, close, volume, barCount, WAP, hasGaps);

    if (std::key_exists<int, std::pair<std::shared_ptr<AsyncRequest<HistoricalData>>, HistoricalData>>(m_historicalDataMap, reqId))
    {
        if (open > 0)
        {
            m_historicalDataMap[reqId].second.push_back({date, open, high, low, close, volume, barCount, WAP, hasGaps});
        }
        else
        {
            m_historicalDataMap[reqId].first->SetResult(std::move(m_historicalDataMap[reqId].second));
            m_historicalDataMap.erase(reqId);
        }
    }
}

void TwsImpl::scannerParameters(const std::string& xml)
{
    printf("ScannerParameters. %s\n", xml.c_str());
}

void TwsImpl::scannerData(int reqId, int rank, const ContractDetails& contractDetails,
    const std::string& distance, const std::string& benchmark, const std::string& projection,
    const std::string& legsStr)
{
    printf("ScannerData. %d - Rank: %d, Symbol: %s, SecType: %s, Currency: %s, Distance: %s, Benchmark: %s, Projection: %s, Legs String: %s\n", reqId, rank, contractDetails.summary.symbol.c_str(), contractDetails.summary.secType.c_str(), contractDetails.summary.currency.c_str(), distance.c_str(), benchmark.c_str(), projection.c_str(), legsStr.c_str());
}

void TwsImpl::scannerDataEnd(int reqId)
{
    printf("ScannerDataEnd. %d\n", reqId);
}

void TwsImpl::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
    long volume, double wap, int count)
{
    printf("RealTimeBars. %ld - Time: %ld, Open: %g, High: %g, Low: %g, Close: %g, Volume: %ld, Count: %d, WAP: %g\n", reqId, time, open, high, low, close, volume, count, wap);
}

void TwsImpl::fundamentalData(TickerId reqId, const std::string& data)
{
    printf("FundamentalData. ReqId: %ld, %s\n", reqId, data.c_str());
}

void TwsImpl::deltaNeutralValidation(int reqId, const UnderComp& underComp)
{
    printf("DeltaNeutralValidation. %d, ConId: %ld, Delta: %g, Price: %g\n", reqId, underComp.conId, underComp.delta, underComp.price);
}

void TwsImpl::tickSnapshotEnd(int reqId)
{
    printf("TickSnapshotEnd: %d\n", reqId);
}

void TwsImpl::marketDataType(TickerId reqId, int marketDataType)
{
    printf("MarketDataType. ReqId: %ld, Type: %d\n", reqId, marketDataType);
}

void TwsImpl::commissionReport(const CommissionReport& commissionReport)
{
    printf("CommissionReport. %s - %g %s RPNL %g\n", commissionReport.execId.c_str(), commissionReport.commission, commissionReport.currency.c_str(), commissionReport.realizedPNL);
}

void TwsImpl::position(const std::string& account, const Contract& contract, double position, double avgCost)
{
    printf("Position. %s - Symbol: %s, SecType: %s, Currency: %s, Position: %g, Avg Cost: %g\n", account.c_str(), contract.symbol.c_str(), contract.secType.c_str(), contract.currency.c_str(), position, avgCost);
}

void TwsImpl::positionEnd()
{
    printf("PositionEnd\n");
}

void TwsImpl::accountSummary(int reqId, const std::string& account, const std::string& tag, const std::string& value, const std::string& currency)
{
    printf("Acct Summary. ReqId: %d, Account: %s, Tag: %s, Value: %s, Currency: %s\n", reqId, account.c_str(), tag.c_str(), value.c_str(), currency.c_str());

    if (std::key_exists(m_accountSummaryMap, reqId))
    {
        m_accountSummaryMap[reqId].second.push_back({account, tag, value, currency});
    }
}

void TwsImpl::accountSummaryEnd(int reqId)
{
    printf("AccountSummaryEnd. Req Id: %d\n", reqId);

    if (std::key_exists(m_accountSummaryMap, reqId))
    {
        m_accountSummaryMap[reqId].first->SetResult(std::move(m_accountSummaryMap[reqId].second));
        m_accountSummaryMap.erase(reqId);
    }
}

void TwsImpl::verifyMessageAPI(const std::string& apiData)
{
    printf("verifyMessageAPI: %s\b", apiData.c_str());
}

void TwsImpl::verifyCompleted(bool isSuccessful, const std::string& errorText)
{
    printf("verifyCompleted. IsSuccessfule: %d - Error: %s\n", isSuccessful, errorText.c_str());
}

void TwsImpl::verifyAndAuthMessageAPI(const std::string& apiDatai, const std::string& xyzChallenge)
{
    printf("verifyAndAuthMessageAPI: %s %s\n", apiDatai.c_str(), xyzChallenge.c_str());
}

void TwsImpl::verifyAndAuthCompleted(bool isSuccessful, const std::string& errorText)
{
    printf("verifyAndAuthCompleted. IsSuccessful: %d - Error: %s\n", isSuccessful, errorText.c_str());
    
    if (isSuccessful)
    {
        m_client->startApi();
    }
}

void TwsImpl::displayGroupList(int reqId, const std::string& groups)
{
    printf("Display Group List. ReqId: %d, Groups: %s\n", reqId, groups.c_str());
}

void TwsImpl::displayGroupUpdated(int reqId, const std::string& contractInfo)
{
    std::cout << "Display Group Updated. ReqId: " << reqId << ", Contract Info: " << contractInfo << std::endl;
}

void TwsImpl::positionMulti(int reqId, const std::string& account, const std::string& modelCode, const Contract& contract, double pos, double avgCost)
{
    printf("Position Multi. Request: %d, Account: %s, ModelCode: %s, Symbol: %s, SecType: %s, Currency: %s, Position: %g, Avg Cost: %g\n", reqId, account.c_str(), modelCode.c_str(), contract.symbol.c_str(), contract.secType.c_str(), contract.currency.c_str(), pos, avgCost);
}

void TwsImpl::positionMultiEnd(int reqId)
{
    printf("Position Multi End. Request: %d\n", reqId);
}

void TwsImpl::accountUpdateMulti(int reqId, const std::string& account, const std::string& modelCode, const std::string& key, const std::string& value, const std::string& currency)
{
    printf("AccountUpdate Multi. Request: %d, Account: %s, ModelCode: %s, Key, %s, Value: %s, Currency: %s\n", reqId, account.c_str(), modelCode.c_str(), key.c_str(), value.c_str(), currency.c_str());
}

void TwsImpl::accountUpdateMultiEnd(int reqId)
{
    printf("Account Update Multi End. Request: %d\n", reqId);
}

void TwsImpl::securityDefinitionOptionalParameter(int reqId, const std::string& /*exchange*/, int /*underlyingConId*/, const std::string& tradingClass, const std::string& multiplier, std::set<std::string> expirations, std::set<double> strikes)
{
    printf("Security Definition Optional Parameter. Request: %d, Trading Class: %s, Multiplier: %s\n", reqId, tradingClass.c_str(), multiplier.c_str());
}

void TwsImpl::securityDefinitionOptionalParameterEnd(int reqId)
{
    printf("Security Definition Optional Parameter End. Request: %d\n", reqId);
}

void TwsImpl::softDollarTiers(int /*reqId*/, const std::vector<SoftDollarTier> &tiers)
{
    printf("Soft dollar tiers (%d):", (uint32_t)tiers.size());

    for (uint64_t i = 0; i < tiers.size(); i++)
    {
        printf("%s\n", tiers[0].displayName().c_str());
    }
}
#pragma endregion

#if 0
void TwsImpl::tickPrice(TickerId /*tickerId*/, TickType /*field*/, double /*price*/, int /*canAutoExecute*/) {}
void TwsImpl::tickSize(TickerId /*tickerId*/, TickType /*field*/, int /*size*/) {}
void TwsImpl::tickOptionComputation(TickerId /*tickerId*/, TickType /*tickType*/, double /*impliedVol*/, double /*delta*/, double /*optPrice*/, double /*pvDividend*/, double /*gamma*/, double /*vega*/, double /*theta*/, double /*undPrice*/) {}
void TwsImpl::tickGeneric(TickerId /*tickerId*/, TickType /*tickType*/, double /*value*/) {}
void TwsImpl::tickString(TickerId /*tickerId*/, TickType /*tickType*/, const std::string& /*value*/) {}
void TwsImpl::tickEFP(TickerId /*tickerId*/, TickType /*tickType*/, double /*basisPoints*/, const std::string& /*formattedBasisPoints*/, double /*totalDividends*/, int /*holdDays*/, const std::string& /*futureLastTradeDate*/, double /*dividendImpact*/, double /*dividendsToLastTradeDate*/) {}
void TwsImpl::orderStatus(OrderId /*orderId*/, const std::string& /*status*/, double /*filled*/, double /*remaining*/, double /*avgFillPrice*/, int /*permId*/, int /*parentId*/, double /*lastFillPrice*/, int /*clientId*/, const std::string& /*whyHeld*/) {}
void TwsImpl::openOrder(OrderId /*orderId*/, const Contract&, const Order&, const OrderState&) {}
void TwsImpl::openOrderEnd() {}
void TwsImpl::winError(const std::string& /*str*/, int /*lastError*/) {}
void TwsImpl::connectionClosed() {}
void TwsImpl::updateAccountValue(const std::string& /*key*/, const std::string& /*val*/, const std::string& /*currency*/, const std::string& /*accountName*/) {}
void TwsImpl::updatePortfolio(const Contract& /*contract*/, double /*position*/, double /*marketPrice*/, double /*marketValue*/, double /*averageCost*/, double /*unrealizedPNL*/, double /*realizedPNL*/, const std::string& /*accountName*/) {}
void TwsImpl::updateAccountTime(const std::string& /*timeStamp*/) {}
void TwsImpl::accountDownloadEnd(const std::string& /*accountName*/) {}
void TwsImpl::nextValidId(OrderId /*orderId*/) {}
void TwsImpl::contractDetails(int /*reqId*/, const ContractDetails& /*contractDetails*/) {}
void TwsImpl::bondContractDetails(int /*reqId*/, const ContractDetails& /*contractDetails*/) {}
void TwsImpl::contractDetailsEnd(int /*reqId*/) {}
void TwsImpl::execDetails(int /*reqId*/, const Contract& /*contract*/, const Execution& /*execution*/) {}
void TwsImpl::execDetailsEnd(int /*reqId*/) {}
void TwsImpl::error(const int /*id*/, const int /*errorCode*/, const std::string /*errorString*/) {}
void TwsImpl::updateMktDepth(TickerId /*id*/, int /*position*/, int /*operation*/, int /*side*/, double/* price*/, int /*size*/) {}
void TwsImpl::updateMktDepthL2(TickerId /*id*/, int /*position*/, std::string /*marketMaker*/, int /*operation*/, int /*side*/, double /*price*/, int /*size*/) {}
void TwsImpl::updateNewsBulletin(int /*msgId*/, int /*msgType*/, const std::string& /*newsMessage*/, const std::string& /*originExch*/) {}
void TwsImpl::managedAccounts(const std::string& /*accountsList*/) {}
void TwsImpl::receiveFA(faDataType /*pFaDataType*/, const std::string& /*cxml*/) {}
void TwsImpl::historicalData(TickerId /*reqId*/, const std::string& /*date*/, double /*open*/, double /*high*/, double /*low*/, double /*close*/, int /*volume*/, int /*barCount*/, double /*WAP*/, int /*hasGaps*/) {}
void TwsImpl::scannerParameters(const std::string& /*xml*/) {}
void TwsImpl::scannerData(int /*reqId*/, int /*rank*/, const ContractDetails& /*contractDetails*/, const std::string& /*distance*/, const std::string& /*benchmark*/, const std::string& /*projection*/, const std::string& /*legsStr*/) {}
void TwsImpl::scannerDataEnd(int /*reqId*/) {}
void TwsImpl::realtimeBar(TickerId /*reqId*/, long /*time*/, double /*open*/, double /*high*/, double /*low*/, double /*close*/, long /*volume*/, double /*wap*/, int /*count*/) {}
void TwsImpl::currentTime(long /*time*/) {}
void TwsImpl::fundamentalData(TickerId /*reqId*/, const std::string& /*data*/) {}
void TwsImpl::deltaNeutralValidation(int /*reqId*/, const UnderComp& /*underComp*/) {}
void TwsImpl::tickSnapshotEnd(int /*reqId*/) {}
void TwsImpl::marketDataType(TickerId /*reqId*/, int /*marketDataType*/) {}
void TwsImpl::commissionReport(const CommissionReport& /*commissionReport*/) {}
void TwsImpl::position(const std::string& /*account*/, const Contract& /*contract*/, double /*position*/, double /*avgCost*/) {}
void TwsImpl::positionEnd() {}
void TwsImpl::accountSummary(int /*reqId*/, const std::string& /*account*/, const std::string& /*tag*/, const std::string& /*value*/, const std::string& /*curency*/) {}
void TwsImpl::accountSummaryEnd(int /*reqId*/) {}
void TwsImpl::verifyMessageAPI(const std::string& /*apiData*/) {}
void TwsImpl::verifyCompleted(bool /*isSuccessful*/, const std::string& /*errorText*/) {}
void TwsImpl::displayGroupList(int /*reqId*/, const std::string& /*groups*/) {}
void TwsImpl::displayGroupUpdated(int /*reqId*/, const std::string& /*contractInfo*/) {}
void TwsImpl::verifyAndAuthMessageAPI(const std::string& /*apiData*/, const std::string& /*xyzChallange*/) {}
void TwsImpl::verifyAndAuthCompleted(bool /*isSuccessful*/, const std::string& /*errorText*/) {}
void TwsImpl::connectAck() {}
void TwsImpl::positionMulti(int /*reqId*/, const std::string& /*account*/, const std::string& /*modelCode*/, const Contract& /*contract*/, double /*pos*/, double /*avgCost*/) {}
void TwsImpl::positionMultiEnd(int /*reqId*/) {}
void TwsImpl::accountUpdateMulti(int /*reqId*/, const std::string& /*account*/, const std::string& /*modelCode*/, const std::string& /*key*/, const std::string& /*value*/, const std::string& /*currency*/) {}
void TwsImpl::accountUpdateMultiEnd(int /*reqId*/) {}
void TwsImpl::securityDefinitionOptionalParameter(int /*reqId*/, const std::string& /*exchange*/, int /*underlyingConId*/, const std::string& /*tradingClass*/, const std::string& /*multiplier*/, std::set<std::string> /*expirations*/, std::set<double> /*strikes*/) {}
void TwsImpl::securityDefinitionOptionalParameterEnd(int /*reqId*/) {}
void TwsImpl::softDollarTiers(int /*reqId*/, const std::vector<SoftDollarTier>& /*tiers*/) {}
#endif
