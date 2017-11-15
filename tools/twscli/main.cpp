#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <Tws.h>

int main()
{
    auto tws = TwsInterface::MakeShared();

    while (1)
    {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);
        std::vector<std::string> args;
        boost::split(args, line, boost::is_any_of(" "), boost::token_compress_on);

        if (args[0] == "Connect")
        {
            tws->Connect("127.0.0.1", 7497);
            Sleep(100);
        }
        else if (args[0] == "Disconnect")
        {
            tws->Disconnect();
        }
        else if (args[0] == "RequestAccountSummary")
        {
            std::string group = "All";
            std::string tags = "AccountType,NetLiquidation,TotalCashValue";
            auto result = tws->RequestAccountSummary(group, tags);
            result->WaitForResult(1000);
        }
        else if (args[0] == "RequestHistoricalMarketData")
        {
            Contract contract;
            contract.symbol = "EUR";
            contract.secType = "CASH";
            contract.currency = "USD";
            contract.exchange = "IDEALPRO";

            std::time_t rawtime;
            std::tm* timeinfo;
            char queryTime[80];

            std::time(&rawtime);
            timeinfo = localtime(&rawtime);
            std::strftime(queryTime, 80, "%Y%m%d %H:%M:%S", timeinfo);

            tws->RequestHistoricalMarketData(contract, queryTime, "5 M", "1 min", "TRADES", 1, 1, TagValueListSPtr());
        }
        else if ((args[0] == "exit") || (args[0] == "quit"))
        {
            break;
        }
        else if (args[0] == "")
        {
            continue;
        }
        else
        {
            std::cout << "Command not recognized." << std::endl;
        }
    }

    return 0;
}
