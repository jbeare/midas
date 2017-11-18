#pragma once

#include <vector>

#include <MLibException.h>
#include <SimpleMatrix.h>

struct Analysis
{
    void Print() const
    {
        double c = correct;
        uint64_t r = samples;
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
        printf("%6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f\n",
            actGain[1], maxGain[1],
            actGain[2], maxGain[2],
            actGain[3], maxGain[3],
            actGain[4], maxGain[4],
            actGain[5], maxGain[5],
            actGain[6], maxGain[6],
            actGain[7], maxGain[7]);

        printf("\n");
    }

    double maxGain[8]{};
    double actGain[8]{};
    int labelSpread[8]{};
    int resultSpread[8]{};
    int correctSpread[8]{};
    int underSpread[8]{};
    int overSpread[8]{};
    uint64_t samples{};
    int correct{};
    int under{};
    int over{};
    double loss{};
    int trades{};
    int goodTrades{};
    int opportunities{};
    int lossOnBadTrade{};
    int gainOnBadTrade{};
    double maxGainTotal{};
    double actGainTotal{};
};

class Analyzer
{
public:
    static Analysis Analyze(const SimpleMatrix<double>& data, const std::vector<uint32_t>& labels, const std::vector<uint32_t>& results)
    {
        if ((data.NumRows() != labels.size()) || (results.size() != labels.size()))
        {
            throw MLibException(E_INVALIDARG);
        }

        Analysis a;

        a.samples = data.NumRows();

        for (uint32_t i = 1; i < data.NumRows() - 1; i++)
        {
            auto d0 = data.Row(i);
            auto d1 = data.Row(i + 1);
            a.labelSpread[labels[i]]++;
            a.resultSpread[results[i]]++;
            if (labels[i] > 0)
            {
                a.opportunities++;
            }

            if (labels[i] == results[i]) // We classified the data correctly.
            {
                // Gain profit equal to the classification.
                if (results[i] > 0)
                {
                    a.actGain[results[i]]++;
                    a.trades++;
                    a.goodTrades++;
                }
                a.correct++;
                a.correctSpread[results[i]]++;
            }
            else if (labels[i] > results[i]) // We underestimated the classification.
            {
                // Gain profit equal to the under-classification.
                if (results[i] > 0)
                {
                    a.actGain[results[i]]++;
                    a.trades++;
                    a.goodTrades++;
                }
                a.under++;
                a.underSpread[labels[i] - results[i]]++;
            }
            else // We overestimated the classification.
            {
                // We gain/lose.
                if (results[i] > 0)
                {
                    a.loss += d1[4] - d0[4];
                    a.trades++;
                    if ((d1[4] - d0[4]) > 0)
                    {
                        a.gainOnBadTrade++;
                    }
                    else
                    {
                        a.lossOnBadTrade++;
                    }
                }
                a.over++;
                a.overSpread[results[i] - labels[i]]++;
            }
        }

        a.maxGain[0] = a.labelSpread[0] * 0;
        a.maxGain[1] = a.labelSpread[1] * 0.005;
        a.maxGain[2] = a.labelSpread[2] * 0.01;
        a.maxGain[3] = a.labelSpread[3] * 0.05;
        a.maxGain[4] = a.labelSpread[4] * 0.10;
        a.maxGain[5] = a.labelSpread[5] * 0.15;
        a.maxGain[6] = a.labelSpread[6] * 0.20;
        a.maxGain[7] = a.labelSpread[7] * 0.25;
        a.maxGainTotal = a.maxGain[0] + a.maxGain[1] + a.maxGain[2] + a.maxGain[3] + a.maxGain[4] + a.maxGain[5] + a.maxGain[6] + a.maxGain[7];

        a.actGain[0] = a.actGain[0] * 0;
        a.actGain[1] = a.actGain[1] * 0.005;
        a.actGain[2] = a.actGain[2] * 0.01;
        a.actGain[3] = a.actGain[3] * 0.05;
        a.actGain[4] = a.actGain[4] * 0.10;
        a.actGain[5] = a.actGain[5] * 0.15;
        a.actGain[6] = a.actGain[6] * 0.20;
        a.actGain[7] = a.actGain[7] * 0.25;
        a.actGainTotal = a.actGain[0] + a.actGain[1] + a.actGain[2] + a.actGain[3] + a.actGain[4] + a.actGain[5] + a.actGain[6] + a.actGain[7];

        return a;
    }
};
