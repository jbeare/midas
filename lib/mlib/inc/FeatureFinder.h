#pragma once

#include <iostream>
#include <sal.h>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

#include <Analyzer.h>
#include <SimpleMatrix.h>
#include <Combination.h>
#include <Classifier.h>

struct FeatureCombination
{
    double Score() const
    {
        auto net = Details.netSpread[1] + Details.netSpread[2] + Details.netSpread[3] + Details.netSpread[4] + Details.netSpread[5] + Details.netSpread[6] + Details.netSpread[7];
        return net - (Details.trades * 0.005);
    }

    void Print() const
    {
        printf("Features: ");
        for (int i = 0; i < Features.size(); i++)
        {
            printf("%u ", Features[i]);
        }
        printf("\n");
        printf("Dimensions: %u\n", Dimensions);
        Details.Print();
    }

    std::vector<uint32_t> Features;
    uint32_t Dimensions;
    Analysis Details;

    bool operator< (const FeatureCombination& f)
    {
        return (Score() > f.Score()) ? true : false;
    }
};

class FeatureFinder
{
public:
    static std::vector<FeatureCombination> FindFeatures(
        _In_ SimpleMatrix<double> const& data,
        _In_ std::vector<uint32_t> const& labels,
        _In_ SimpleMatrix<double> const& testRawData,
        _In_ SimpleMatrix<double> const& testData,
        _In_ std::vector<uint32_t> const& testLabels,
        _In_ uint32_t maxDimensions,
        _In_ uint32_t maxLabels)
    {
        std::vector<uint32_t> v;
        m_specs.clear();
        volatile bool killSwitch{false};

        // Load a vector with each index.
        for (uint32_t i = 0; i < data.NumCols(); i++)
        {
            v.push_back(i);
        }

        // From 1 to max num of features.
        for (uint32_t i = 1; (i <= data.NumCols()) && (i <= maxDimensions); i++)
        {
            // For each combination of 'i' number of features.
            do
            {
                // Create training and test data sets.
                std::vector<double> dv, tv;
                for (uint32_t j = 0; j < data.NumRows(); j++)
                {
                    for (uint32_t k = 0; k < i; k++)
                    {
                        dv.push_back(data.Row(j)[k]);
                    }
                }
                for (uint32_t j = 0; j < testData.NumRows(); j++)
                {
                    for (uint32_t k = 0; k < i; k++)
                    {
                        tv.push_back(testData.Row(j)[k]);
                    }
                }

                SimpleMatrix<double> d(dv, data.NumRows(), i);
                SimpleMatrix<double> t(tv, testData.NumRows(), i);

                // From 1 to max num of dimensions.
                for (uint32_t k = 1; (k <= d.NumCols()) && (k <= maxDimensions); k++)
                {
                    auto f = std::vector<uint32_t>{v.begin(), v.begin() + i};
                    QueueWork([d, labels, testRawData, t, testLabels, k, maxLabels, f]()
                    {
                        _FindFeatures(d, labels, testRawData, t, testLabels, k, maxLabels, f);
                    });
                }

            } while (!killSwitch && next_combination(v.begin(), v.begin() + i, v.end()));
        }

        QueueWork({});

        std::sort(m_specs.begin(), m_specs.end());
        return m_specs;
    }

    static void QueueWork(std::function<void()> work)
    {
        static std::atomic<int> numActiveThreads{};
        static std::vector<std::thread> threads;
        static std::vector<std::function<void()>> workItems;

        if (work)
        {
            workItems.push_back(work);
        }

        if ((!work && !workItems.empty()) || (workItems.size() >= 100))
        {
            threads.push_back(std::thread([](std::vector<std::function<void()>> wi)
            {
                for (auto& w : wi)
                {
                    w();
                }
            }, std::move(workItems)));
            workItems.clear();
        }

        if (!work || (threads.size() >= 24))
        {
            for (auto& th : threads)
            {
                th.join();
            }
            threads.clear();
        }
    }

    static void _FindFeatures(
        _In_ SimpleMatrix<double> const& data,
        _In_ std::vector<uint32_t> const& labels,
        _In_ SimpleMatrix<double> const& testRawData,
        _In_ SimpleMatrix<double> const& testData,
        _In_ std::vector<uint32_t> const& testLabels,
        _In_ uint32_t dimensions,
        _In_ uint32_t labelCount,
        _In_ std::vector<uint32_t> const& features)
    {
        auto c = Classifier::MakeShared(labelCount, dimensions);
        c->Train(data, labels, true);
        auto testResults = c->Classify(testData);
        auto a = Analyzer::Analyze2(testRawData, testLabels, testResults);
        std::lock_guard<std::mutex> lock(m_mutex);
        m_specs.push_back(FeatureCombination{ features, dimensions, a });
    }

private:
    static std::mutex m_mutex;
    static std::vector<FeatureCombination> m_specs;
};

std::mutex FeatureFinder::m_mutex{};
std::vector<FeatureCombination> FeatureFinder::m_specs{};
