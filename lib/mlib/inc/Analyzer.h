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

        printf("\n        1     |        2      |        3      |        4      |        5      |        6      |        7\n");
        printf("%6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f\n",
            gainSpread[1], lossSpread[1],
            gainSpread[2], lossSpread[2],
            gainSpread[3], lossSpread[3],
            gainSpread[4], lossSpread[4],
            gainSpread[5], lossSpread[5],
            gainSpread[6], lossSpread[6],
            gainSpread[7], lossSpread[7]);

        printf("Net score: %f\n", netSpread[1] + netSpread[2] + netSpread[3] + netSpread[4] + netSpread[5] + netSpread[6] + netSpread[7]);
        printf("\n");
    }

    void Print2() const
    {
        double gain = gainSpread[1] + gainSpread[2] + gainSpread[3] + gainSpread[4] + gainSpread[5] + gainSpread[6] + gainSpread[7];
        double losss = lossSpread[1] + lossSpread[2] + lossSpread[3] + lossSpread[4] + lossSpread[5] + lossSpread[6] + lossSpread[7];
        double net = netSpread[1] + netSpread[2] + netSpread[3] + netSpread[4] + netSpread[5] + netSpread[6] + netSpread[7];

        printf("Trades: %d Good: %d Bad(+): %d Bad(-): %d Opps: %d\n", trades, goodTrades, gainOnBadTrade, lossOnBadTrade, opportunities);

        printf("\n        1     |        2      |        3      |        4      |        5      |        6      |        7\n");
        printf("%6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f\n",
            gainSpread[1], lossSpread[1],
            gainSpread[2], lossSpread[2],
            gainSpread[3], lossSpread[3],
            gainSpread[4], lossSpread[4],
            gainSpread[5], lossSpread[5],
            gainSpread[6], lossSpread[6],
            gainSpread[7], lossSpread[7]);

        printf("\n        0     |        1      |        2      |        3      |        4      |        5      |        6\n");
        printf("%6d %6d | %6d %6d | %6d %6d | %6d %6d | %6d %6d | %6d %6d | %6d %6d\n",
            resultSpread[0], labelSpread[0],
            resultSpread[1], labelSpread[1],
            resultSpread[2], labelSpread[2],
            resultSpread[3], labelSpread[3],
            resultSpread[4], labelSpread[4],
            resultSpread[5], labelSpread[5],
            resultSpread[6], labelSpread[6]);

        printf("Gain: %f Loss: %f Net %f\n", gain, losss, net);
        printf("Overhead: %f Net:%f\n", trades * 0.005, net - (trades * 0.005));
        printf("Net per trade: %f Net per dollar per trade: %f\n", ((net - (trades * 0.005)) / trades), ((net - (trades * 0.005)) / trades) / price);
        printf("\n");
    }

    double Score()
    {
        double net = netSpread[1] + netSpread[2] + netSpread[3] + netSpread[4] + netSpread[5] + netSpread[6] + netSpread[7];
        return ((net - (trades * 0.005)) / trades) / price;
    }

    template<typename Archive>
    void serialize(_Inout_ Archive& ar, _In_ const unsigned int = 0)
    {
        ar & boost::serialization::make_nvp("Analysis_maxGain", maxGain);
        ar & boost::serialization::make_nvp("Analysis_actGain", actGain);
        ar & boost::serialization::make_nvp("Analysis_labelSpread", labelSpread);
        ar & boost::serialization::make_nvp("Analysis_resultSpread", resultSpread);
        ar & boost::serialization::make_nvp("Analysis_correctSpread", correctSpread);
        ar & boost::serialization::make_nvp("Analysis_underSpread", underSpread);
        ar & boost::serialization::make_nvp("Analysis_overSpread", overSpread);
        ar & boost::serialization::make_nvp("Analysis_samples", samples);
        ar & boost::serialization::make_nvp("Analysis_correct", correct);
        ar & boost::serialization::make_nvp("Analysis_under", under);
        ar & boost::serialization::make_nvp("Analysis_over", over);
        ar & boost::serialization::make_nvp("Analysis_loss", loss);
        ar & boost::serialization::make_nvp("Analysis_trades", trades);
        ar & boost::serialization::make_nvp("Analysis_goodTrades", goodTrades);
        ar & boost::serialization::make_nvp("Analysis_opportunities", opportunities);
        ar & boost::serialization::make_nvp("Analysis_lossOnBadTrade", lossOnBadTrade);
        ar & boost::serialization::make_nvp("Analysis_gainOnBadTrade", gainOnBadTrade);
        ar & boost::serialization::make_nvp("Analysis_maxGainTotal", maxGainTotal);
        ar & boost::serialization::make_nvp("Analysis_actGainTotal", actGainTotal);
        ar & boost::serialization::make_nvp("Analysis_gainSpread", gainSpread);
        ar & boost::serialization::make_nvp("Analysis_lossSpread", lossSpread);
        ar & boost::serialization::make_nvp("Analysis_netSpread", netSpread);
        ar & boost::serialization::make_nvp("Analysis_price", price);
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

    // Analyze2
    double gainSpread[8]{};
    double lossSpread[8]{};
    double netSpread[8]{};
    double price{};
};

class Analyzer
{
public:
    static Analysis Analyze2(const SimpleMatrix<double>& data, const std::vector<uint32_t>& labels, const std::vector<uint32_t>& results)
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
            a.price += d0[1];

            double change = d1[4] - d0[4];
            if (change > 0)
            {
                a.gainSpread[results[i]] += change;
                a.netSpread[results[i]] += change;
            }
            else
            {
                a.lossSpread[results[i]] += change;
                a.netSpread[results[i]] += change;
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

        a.price = a.price / data.NumRows();

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
