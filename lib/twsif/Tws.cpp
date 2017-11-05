#include "Tws.h"

#include <EClientSocket.h>

Tws::Tws() :
    m_osSignal(2000),
    m_client(new EClientSocket(this, &m_osSignal))
{

}

Tws::~Tws()
{

}

bool Tws::Connect(const std::string /*host*/, uint32_t /*port*/, int32_t /*clientId*/)
{
    return true;
}

void Tws::Disconnect() const
{

}

bool Tws::IsConnected() const
{
    return true;
}

void Tws::tickPrice(TickerId /*tickerId*/, TickType /*field*/, double /*price*/, int /*canAutoExecute*/) {}
void Tws::tickSize(TickerId /*tickerId*/, TickType /*field*/, int /*size*/) {}
void Tws::tickOptionComputation(TickerId /*tickerId*/, TickType /*tickType*/, double /*impliedVol*/, double /*delta*/, double /*optPrice*/, double /*pvDividend*/, double /*gamma*/, double /*vega*/, double /*theta*/, double /*undPrice*/) {}
void Tws::tickGeneric(TickerId /*tickerId*/, TickType /*tickType*/, double /*value*/) {}
void Tws::tickString(TickerId /*tickerId*/, TickType /*tickType*/, const std::string& /*value*/) {}
void Tws::tickEFP(TickerId /*tickerId*/, TickType /*tickType*/, double /*basisPoints*/, const std::string& /*formattedBasisPoints*/, double /*totalDividends*/, int /*holdDays*/, const std::string& /*futureLastTradeDate*/, double /*dividendImpact*/, double /*dividendsToLastTradeDate*/) {}
void Tws::orderStatus(OrderId /*orderId*/, const std::string& /*status*/, double /*filled*/, double /*remaining*/, double /*avgFillPrice*/, int /*permId*/, int /*parentId*/, double /*lastFillPrice*/, int /*clientId*/, const std::string& /*whyHeld*/) {}
void Tws::openOrder(OrderId /*orderId*/, const Contract&, const Order&, const OrderState&) {}
void Tws::openOrderEnd() {}
void Tws::winError(const std::string& /*str*/, int /*lastError*/) {}
void Tws::connectionClosed() {}
void Tws::updateAccountValue(const std::string& /*key*/, const std::string& /*val*/, const std::string& /*currency*/, const std::string& /*accountName*/) {}
void Tws::updatePortfolio(const Contract& /*contract*/, double /*position*/, double /*marketPrice*/, double /*marketValue*/, double /*averageCost*/, double /*unrealizedPNL*/, double /*realizedPNL*/, const std::string& /*accountName*/) {}
void Tws::updateAccountTime(const std::string& /*timeStamp*/) {}
void Tws::accountDownloadEnd(const std::string& /*accountName*/) {}
void Tws::nextValidId(OrderId /*orderId*/) {}
void Tws::contractDetails(int /*reqId*/, const ContractDetails& /*contractDetails*/) {}
void Tws::bondContractDetails(int /*reqId*/, const ContractDetails& /*contractDetails*/) {}
void Tws::contractDetailsEnd(int /*reqId*/) {}
void Tws::execDetails(int /*reqId*/, const Contract& /*contract*/, const Execution& /*execution*/) {}
void Tws::execDetailsEnd(int /*reqId*/) {}
void Tws::error(const int /*id*/, const int /*errorCode*/, const std::string /*errorString*/) {}
void Tws::updateMktDepth(TickerId /*id*/, int /*position*/, int /*operation*/, int /*side*/, double/* price*/, int /*size*/) {}
void Tws::updateMktDepthL2(TickerId /*id*/, int /*position*/, std::string /*marketMaker*/, int /*operation*/, int /*side*/, double /*price*/, int /*size*/) {}
void Tws::updateNewsBulletin(int /*msgId*/, int /*msgType*/, const std::string& /*newsMessage*/, const std::string& /*originExch*/) {}
void Tws::managedAccounts(const std::string& /*accountsList*/) {}
void Tws::receiveFA(faDataType /*pFaDataType*/, const std::string& /*cxml*/) {}
void Tws::historicalData(TickerId /*reqId*/, const std::string& /*date*/, double /*open*/, double /*high*/, double /*low*/, double /*close*/, int /*volume*/, int /*barCount*/, double /*WAP*/, int /*hasGaps*/) {}
void Tws::scannerParameters(const std::string& /*xml*/) {}
void Tws::scannerData(int /*reqId*/, int /*rank*/, const ContractDetails& /*contractDetails*/, const std::string& /*distance*/, const std::string& /*benchmark*/, const std::string& /*projection*/, const std::string& /*legsStr*/) {}
void Tws::scannerDataEnd(int /*reqId*/) {}
void Tws::realtimeBar(TickerId /*reqId*/, long /*time*/, double /*open*/, double /*high*/, double /*low*/, double /*close*/, long /*volume*/, double /*wap*/, int /*count*/) {}
void Tws::currentTime(long /*time*/) {}
void Tws::fundamentalData(TickerId /*reqId*/, const std::string& /*data*/) {}
void Tws::deltaNeutralValidation(int /*reqId*/, const UnderComp& /*underComp*/) {}
void Tws::tickSnapshotEnd(int /*reqId*/) {}
void Tws::marketDataType(TickerId /*reqId*/, int /*marketDataType*/) {}
void Tws::commissionReport(const CommissionReport& /*commissionReport*/) {}
void Tws::position(const std::string& /*account*/, const Contract& /*contract*/, double /*position*/, double /*avgCost*/) {}
void Tws::positionEnd() {}
void Tws::accountSummary(int /*reqId*/, const std::string& /*account*/, const std::string& /*tag*/, const std::string& /*value*/, const std::string& /*curency*/) {}
void Tws::accountSummaryEnd(int /*reqId*/) {}
void Tws::verifyMessageAPI(const std::string& /*apiData*/) {}
void Tws::verifyCompleted(bool /*isSuccessful*/, const std::string& /*errorText*/) {}
void Tws::displayGroupList(int /*reqId*/, const std::string& /*groups*/) {}
void Tws::displayGroupUpdated(int /*reqId*/, const std::string& /*contractInfo*/) {}
void Tws::verifyAndAuthMessageAPI(const std::string& /*apiData*/, const std::string& /*xyzChallange*/) {}
void Tws::verifyAndAuthCompleted(bool /*isSuccessful*/, const std::string& /*errorText*/) {}
void Tws::connectAck() {}
void Tws::positionMulti(int /*reqId*/, const std::string& /*account*/, const std::string& /*modelCode*/, const Contract& /*contract*/, double /*pos*/, double /*avgCost*/) {}
void Tws::positionMultiEnd(int /*reqId*/) {}
void Tws::accountUpdateMulti(int /*reqId*/, const std::string& /*account*/, const std::string& /*modelCode*/, const std::string& /*key*/, const std::string& /*value*/, const std::string& /*currency*/) {}
void Tws::accountUpdateMultiEnd(int /*reqId*/) {}
void Tws::securityDefinitionOptionalParameter(int /*reqId*/, const std::string& /*exchange*/, int /*underlyingConId*/, const std::string& /*tradingClass*/, const std::string& /*multiplier*/, std::set<std::string> /*expirations*/, std::set<double> /*strikes*/) {}
void Tws::securityDefinitionOptionalParameterEnd(int /*reqId*/) {}
void Tws::softDollarTiers(int /*reqId*/, const std::vector<SoftDollarTier>& /*tiers*/) {}
