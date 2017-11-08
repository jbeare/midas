#pragma once

#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include <AsyncRequest.h>

class EClientSocket;

class TwsInterface
{
public:
    static std::shared_ptr<TwsInterface> MakeShared();

    virtual bool Connect(_In_ const std::string host, _In_ uint32_t port, _In_ int32_t clientId = 0) = 0;
    virtual void Disconnect() const = 0;
    virtual bool IsConnected() const = 0;
    virtual void SetConnectOptions(_In_ const std::string& connectOptions) = 0;

    typedef std::vector<std::tuple<std::string, std::string, std::string, std::string>> AccountSummary;
    typedef std::map<int, std::pair<std::shared_ptr<AsyncRequest<AccountSummary>>, AccountSummary>> AccountSummaryMap;
    virtual std::shared_ptr<AsyncResult<AccountSummary>> RequestAccountSummary(_In_ const std::string& groupName, _In_ const std::string& tags) = 0;
};
