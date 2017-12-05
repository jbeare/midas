#pragma once

#include <iostream>
#include <memory>
#include <sal.h>
#include <Classifier.h>
#include <Tws.h>
#include <thread>
#include <UniqueHandle.h>
#include <Analyzer.h>
#include <TwsMock.h>
#include <FeatureStream.h>

class Midas
{
public:
    typedef std::map<std::string, std::pair<std::shared_ptr<Classifier>, Analysis>> ClassifierMap;
    static std::shared_ptr<Midas> MakeShared(_In_ ClassifierMap classifierMap, _In_ std::shared_ptr<Tws> tws)
    {
        return std::make_shared<Midas>(classifierMap, tws);
    }

    Midas(_In_ ClassifierMap classifierMap, _In_ std::shared_ptr<Tws> tws) :
        m_classifierMap(classifierMap),
        m_tws(tws)
    {

    }

    ~Midas()
    {

    }

    void Trade()
    {
        /*printf("Trading, press any key to stop...\n");
        std::thread t(&Midas::_Trade, this);
        std::getchar();
        m_halt.Set();
        t.join();*/

        std::vector<Bar> bars;
        std::map<std::string, Bar> barMap;
        std::string position;
        double net{};
        double buyPrice{};
        do
        {
            bars = std::dynamic_pointer_cast<TwsMock>(m_tws)->GetNextBars();
            for (auto const& bar : bars)
            {
                barMap[bar.Symbol] = bar;
            }

            if (!position.empty())
            {
                double sellPrice = barMap[position].Close;
                double shareCount = (1000 / buyPrice);
                double netProfit = (sellPrice - buyPrice) * shareCount;
                printf("Selling %.2f shares of %s at %.2f for net profit of %.4f\n", shareCount, position.c_str(), sellPrice, netProfit);
                net += netProfit;
            }

            std::map<std::string, uint32_t> results;
            for (auto& bar : bars)
            {
                auto classifier = m_classifierMap.find(bar.Symbol);
                if (classifier == m_classifierMap.end())
                {
                    continue;
                }

                auto& fs = m_featureStreamMap[bar.Symbol];
                std::vector<FeatureSet> f;
                fs << std::vector<Bar>{bar};
                fs >> f;

                if (f.empty())
                {
                    continue;
                }

                results[bar.Symbol] = classifier->second.first->Classify(f[0].Features);
                printf("%s: %u  ", bar.Symbol.c_str(), results[bar.Symbol]);
            }
            if (!bars.empty())
            {
                printf("\n");
            }

            std::string symbol;
            double score{};
            for (auto const& result : results)
            {
                if ((result.second > 0) && (m_classifierMap[result.first].second.Score() > score))
                {
                    score = m_classifierMap[result.first].second.Score();
                    symbol = result.first;
                }
            }

            if (!symbol.empty())
            {
                position = symbol;
                buyPrice = barMap[symbol].Close;
                double shareCount = (1000 / buyPrice);
                printf("Buying %.2f shares of %s at %.2f\n", shareCount, position.c_str(), buyPrice);
            }
            else
            {
                position = "";
                buyPrice = 0;
                printf("No trade\n");
            }
        } while (!bars.empty());

        printf("Total net change: %f\n", net);
    }

private:
    void _Trade()
    {
        HANDLE handles[2]{m_halt.Get(), m_wake.Get()};

        switch (WaitForMultipleObjects(ARRAYSIZE(handles), handles, false, INFINITE))
        {
        case WAIT_OBJECT_0: // m_halt
            // Clear all positions.
            return;
        case WAIT_OBJECT_0 + 1: // m_wake
            // Get latest bar.
            // Generate features.
            // Classify.
            // Trade?
            break;
        default:
            throw MLibException(E_UNEXPECTED);
        }
    }

    ClassifierMap m_classifierMap;
    std::map<std::string, FeatureStream<>> m_featureStreamMap;
    std::shared_ptr<Tws> m_tws;
    UniqueEvent m_halt{nullptr, true, false, nullptr};
    UniqueEvent m_wake{nullptr, false, false, nullptr};
};
