#pragma once

#include <memory>
#include <Strategy.h>
#include <DataBrowser.h>
#include <map>
#include <Classifier.h>
#include <FeatureStream.h>
#include <LabelStream.h>
#include <Analyzer.h>
#include <LabelFinder.h>

template<class F = DefaultFeatureSet>
class Simulator
{
public:
    Simulator(std::shared_ptr<DataBrowser> dataBrowser, std::shared_ptr<Strategy> strategy) :
        m_dataBrowser(dataBrowser),
        m_strategy(strategy)
    {

    }

    void Run(std::time_t start, std::time_t end)
    {
        ExtractDataBrowserBars(start, end);
        TrainClassifier();
        Simulate();
    }

private:
    void ExtractDataBrowserBars(std::time_t start, std::time_t end)
    {
        for (auto const& symbol : m_dataBrowser->GetSymbols())
        {
            m_barMap[symbol].first = {m_dataBrowser->GetBars(symbol, BarResolution::Minute, start, end)};
            m_barMap[symbol].second = m_barMap[symbol].first.begin();
        }
    }

    void TrainClassifier()
    {
        for (auto const& bars : m_barMap)
        {
            LabelFinder labelFinder;

            auto configs = labelFinder.FindLabelConfig(bars.second.first, bars.second.first, 5, 2);

            FeatureStream<F> fs;
            std::vector<FeatureSet> f;
            fs << bars.second.first;
            fs >> f;

            LabelStream ls(configs[0].Config);
            std::vector<BarLabel> l;
            ls << bars.second.first;
            ls >> l;

            auto data = AlignTimestamps(bars.second.first, f, l);
            auto raw = SimpleMatrixFromBarVector(std::get<0>(data));
            auto features = SimpleMatrixFromFeatureSetVector(std::get<1>(data));
            auto labels = VectorFromLabelVector(std::get<2>(data));

            m_classifierMap[bars.first] = Classifier::MakeShared(ls.LabelCount(), m_strategy->Dimensions());
            m_classifierMap[bars.first]->Train(features, labels, true);

            std::vector<uint32_t> results = m_classifierMap[bars.first]->Classify(features);
            std::cout << bars.first << std::endl;
            Analyzer::Analyze2(raw, labels, results).Print2();
        }
    }

    std::time_t GetLeastTimestamp()
    {
        std::time_t timestamp{INT32_MAX};

        for (auto const& bars : m_barMap)
        {
            if ((bars.second.second != bars.second.first.end()) && (bars.second.second->Timestamp < timestamp))
            {
                timestamp = bars.second.second->Timestamp;
            }
        }

        return timestamp;
    }

    std::map<std::string, Bar> GetNextTimeslice()
    {
        auto timestamp = GetLeastTimestamp();
        std::map<std::string, Bar> timeslice;

        for (auto& bars : m_barMap)
        {
            if ((bars.second.second != bars.second.first.end()) && (bars.second.second->Timestamp == timestamp))
            {
                timeslice[bars.first] = *bars.second.second;
                bars.second.second++;
            }
        }

        return timeslice;
    }

    void Simulate()
    {
        std::map<std::string, FeatureStream<F>> streamMap;
        auto timeslice = GetNextTimeslice();
        while (!timeslice.empty())
        {
            bool tradeExecuted{false};

            for (auto& bar : timeslice)
            {
                streamMap[bar.first] << std::vector<Bar>{bar.second};
            }

            for (auto& stream : streamMap)
            {
                std::vector<FeatureSet> featureSet;
                stream.second >> featureSet;

                if (!featureSet.empty())
                {
                    auto c = m_classifierMap[stream.first]->Classify(featureSet.back().Features);
                    if (c)
                    {
                        tradeExecuted = true;
                    }
                    printf("%4s:%2u  ", stream.first.c_str(), c);
                }
            }

            printf("****:%2d\n", tradeExecuted ? 1 : 0);
            timeslice = GetNextTimeslice();
        }
    }

    std::shared_ptr<DataBrowser> m_dataBrowser;
    std::shared_ptr<Strategy> m_strategy;
    std::map<std::string, std::pair<std::vector<Bar>, std::vector<Bar>::iterator>> m_barMap;
    std::map<std::string, std::shared_ptr<Classifier>> m_classifierMap;
};
