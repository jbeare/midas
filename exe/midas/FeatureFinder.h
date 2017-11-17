#pragma once

#include <sal.h>
#include <vector>
#include <mutex>
#include <thread>

#include <SimpleMatrix.h>
#include <Combination.h>

class FeatureFinder
{
public:
    struct FeatureSpec
    {
        double Gain{};
        std::vector<uint32_t> Features;
        uint32_t Dimensions;

        bool operator<(const FeatureSpec& f)
        {
            return (Gain > f.Gain) ? true : false;
        }
    };

#if 0
    static std::vector<FeatureSpec> FindFeatures(
        _In_ const arma::mat& data,
        _In_ const arma::Row<size_t>& labels,
        _In_ const arma::mat& testRawData,
        _In_ const arma::mat& testData,
        _In_ const arma::Row<size_t>& testLabels,
        _In_ uint32_t maxDimensions)
    {
        std::vector<std::thread> threads;
        std::vector<uint32_t> v;
        m_specs.clear();

        for (uint32_t i = 0; i < data.n_rows; i++)
        {
            v.push_back(i);
        }

        for (uint32_t i = 1; (i <= data.n_rows) && (i <= maxDimensions); i++)
        {
            do
            {
                arma::mat d, t;
                for (uint32_t j = 0; j < i; j++)
                {
                    d.insert_rows(d.n_rows, data.row(v[j]));
                    t.insert_rows(t.n_rows, testData.row(v[j]));
                }

                for (uint32_t k = 1; (k <= d.n_rows) && (k <= maxDimensions); k++)
                {
                    /*printf("d=%d ", k);
                    for (auto& e : v)
                    {
                        std::cout << e << " ";
                    }
                    std::cout << endl;*/

                    if (threads.size() >= 4)
                    {
                        for (auto& th : threads)
                        {
                            th.join();
                        }
                        threads.clear();
                    }

                    threads.push_back(std::thread(_FindFeatures, d, labels, testRawData, t, testLabels, k, std::vector<uint32_t>{v.begin(), v.begin() + i}));

                    //Midas::Classifier c(d, labels, k, 8);
                    //auto results = c.Classify(d);
                    //s.push_back(FeatureSpec{ Analyze(rawData, labels, results), v, k });
                }

            } while (next_combination(v.begin(), v.begin() + i, v.end()));
        }

        for (auto& th : threads)
        {
            th.join();
        }

        std::sort(m_specs.begin(), m_specs.end());
        return m_specs;
    }

    static void _FindFeatures(
        _In_ const arma::mat& data,
        _In_ const arma::Row<size_t>& labels,
        _In_ const arma::mat& testRawData,
        _In_ const arma::mat& testData,
        _In_ const arma::Row<size_t>& testLabels,
        _In_ uint32_t dimensions,
        _In_ std::vector<uint32_t> features)
    {
        Midas::Classifier c(data, labels, dimensions, 8);
        auto testResults = c.Classify(testData);
        auto score = Analyze(testRawData, testLabels, testResults);
        if (score > 65)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_specs.push_back(FeatureSpec{ score, features, dimensions });
        }
    }
#endif

    static double Analyze(const SimpleMatrix<double>& data, const std::vector<uint32_t>& labels, const std::vector<uint32_t>& results)
    {
        if ((data.NumRows() != labels.size()) || (results.size() != labels.size()))
        {
            printf("Data error!\n");
            return 0;
        }

        double maxGain[8] = {};
        double actGain[8] = {};
        int labelSpread[8]{};
        int resultSpread[8]{};
        int correctSpread[8]{};
        int underSpread[8]{};
        int overSpread[8]{};
        int correct{};
        int under{};
        int over{};
        double loss{};
        int trades{};
        int goodTrades{};
        int opportunities{};
        int lossOnBadTrade{};
        int gainOnBadTrade{};

        for (uint32_t i = 1; i < data.NumRows() - 1; i++)
        {
            auto d0 = data.Row(i);
            auto d1 = data.Row(i + 1);
            labelSpread[labels[i]]++;
            resultSpread[results[i]]++;
            if (labels[i] > 0)
            {
                opportunities++;
            }

            if (labels[i] == results[i]) // We classified the data correctly.
            {
                // Gain profit equal to the classification.
                if (results[i] > 0)
                {
                    actGain[results[i]]++;
                    trades++;
                    goodTrades++;
                }
                correct++;
                correctSpread[results[i]]++;
            }
            else if (labels[i] > results[i]) // We underestimated the classification.
            {
                // Gain profit equal to the under-classification.
                if (results[i] > 0)
                {
                    actGain[results[i]]++;
                    trades++;
                    goodTrades++;
                }
                under++;
                underSpread[labels[i] - results[i]]++;
            }
            else // We overestimated the classification.
            {
                // We gain/lose.
                if (results[i] > 0)
                {
                    loss += d1[4] - d0[4];
                    trades++;
                    if ((d1[4] - d0[4]) > 0)
                    {
                        gainOnBadTrade++;
                    }
                    else
                    {
                        lossOnBadTrade++;
                    }
                }
                over++;
                overSpread[results[i] - labels[i]]++;
            }
        }

        maxGain[0] = labelSpread[0] * 0;
        maxGain[1] = labelSpread[1] * 0.005;
        maxGain[2] = labelSpread[2] * 0.01;
        maxGain[3] = labelSpread[3] * 0.05;
        maxGain[4] = labelSpread[4] * 0.10;
        maxGain[5] = labelSpread[5] * 0.15;
        maxGain[6] = labelSpread[6] * 0.20;
        maxGain[7] = labelSpread[7] * 0.25;
        double maxGainTotal = maxGain[0] + maxGain[1] + maxGain[2] + maxGain[3] + maxGain[4] + maxGain[5] + maxGain[6] + maxGain[7];

        actGain[0] = actGain[0] * 0;
        actGain[1] = actGain[1] * 0.005;
        actGain[2] = actGain[2] * 0.01;
        actGain[3] = actGain[3] * 0.05;
        actGain[4] = actGain[4] * 0.10;
        actGain[5] = actGain[5] * 0.15;
        actGain[6] = actGain[6] * 0.20;
        actGain[7] = actGain[7] * 0.25;
        double actGainTotal = actGain[0] + actGain[1] + actGain[2] + actGain[3] + actGain[4] + actGain[5] + actGain[6] + actGain[7];

        // Accuracy (spot on, over, under)
        // Oppourtunities taken (taken chances/total chances, correct changes/total chances)
        // Gainz (actual gain / max gain)

        double c = correct;
        uint64_t r = (uint64_t)data.NumRows();
        printf("Accuracy +/-0:%.f/%llu(%.2f) +/-1:%.f/%llu(%.2f) +/-2:%.f/%llu(%.2f) +/-3:%.f/%llu(%.2f) +/-4:%.f/%llu(%.2f)\n",
            c, r, c / r,
            c + underSpread[1] + overSpread[1], r, (c + underSpread[1] + overSpread[1]) / r,
            c + underSpread[1] + overSpread[1] + underSpread[2] + overSpread[2], r, (c + underSpread[1] + overSpread[1] + underSpread[2] + overSpread[2]) / r,
            c + underSpread[1] + overSpread[1] + underSpread[2] + overSpread[2] + underSpread[3] + overSpread[3], r, (c + underSpread[1] + overSpread[1] + underSpread[2] + overSpread[2] + underSpread[3] + overSpread[3]) / r,
            c + underSpread[1] + overSpread[1] + underSpread[2] + overSpread[2] + underSpread[3] + overSpread[3] + underSpread[4] + overSpread[4], r, (c + underSpread[1] + overSpread[1] + underSpread[2] + overSpread[2] + underSpread[3] + overSpread[3] + underSpread[4] + overSpread[4]) / r);

        printf("Gain %.2f/%.2f           Gain/Loss on over-classification %.2f\n", actGainTotal, maxGainTotal, loss);
        printf("Trade/Opp %d/%d Good/Opp %d/%d Bad Gain/Loss %d/%d\n",
            trades, opportunities, goodTrades, opportunities, gainOnBadTrade, lossOnBadTrade);

        printf("     .005     |      .01      |      .05      |      .10      |      .15      |      .20      |      .25\n");
        printf("%6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f\n\n",
            actGain[1], maxGain[1],
            actGain[2], maxGain[2],
            actGain[3], maxGain[3],
            actGain[4], maxGain[4],
            actGain[5], maxGain[5],
            actGain[6], maxGain[6],
            actGain[7], maxGain[7]);

        return actGainTotal;
    }

private:
    static std::mutex m_mutex;
    static std::vector<FeatureSpec> m_specs;
};

std::mutex FeatureFinder::m_mutex{};
std::vector<FeatureFinder::FeatureSpec> FeatureFinder::m_specs{};
