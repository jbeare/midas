#pragma once

#include <queue>
#include <vector>
#include <Bar.h>
#include <SimpleMatrix.h>

class DefaultFeatureSet
{
public:
    static constexpr uint32_t HistoryDepth{6};

    std::vector<double> Extract(Bar const& bar)
    {
        m_queue.push(bar);

        if (m_queue.size() > HistoryDepth)
        {
            m_queue.pop();
        }
        else if (m_queue.size() < HistoryDepth)
        {
            return std::vector<double>();
        }
        else
        {
            m_first = bar;
        }

        return {
            GetTimeOfDay(m_queue.back().Timestamp),
            GetDelta(m_queue.back().Close, m_queue.back().Open),
            GetDelta(m_queue.back().High, m_queue.back().Low),
            GetDelta(m_queue.front().High, m_queue.front().Low),
            GetDelta(m_queue.back().Open, m_first.Open),
            GetDelta(m_queue.back().Close, m_first.Close)};
    }

    uint32_t GetNumFeatures()
    {
        return 6;
    }

private:
    double GetTimeOfDay(std::time_t timestamp)
    {
        std::tm t;
        localtime_s(&t, &timestamp);
        return (static_cast<double>(t.tm_sec) + (t.tm_min * 60) + (t.tm_hour * 3600)) / 86400;
    }

    double GetDelta(double x, double y)
    {
        return (((x - y) / y) / 2) + .5;
    }

    std::queue<Bar> m_queue;
    Bar m_first;
};

class DefaultLabelPolicy
{
public:
    std::vector<uint32_t> Label(Bar const& bar, Bar const& bar_next)
    {
        auto delta = bar_next.High - bar.Close;

        if (delta > 0.25)
        {
            return {7};
        }
        else if(delta > 0.20)
        {
            return {6};
        }
        else if(delta > 0.15)
        {
            return {5};
        }
        else if(delta > 0.10)
        {
            return {4};
        }
        else if(delta > 0.05)
        {
            return {3};
        }
        else if(delta > 0.01)
        {
            return {2};
        }
        else if(delta > 0.005)
        {
            return {1};
        }
        else
        {
            return {0};
        }
    }
};

class FeatureExtractor
{
public:
    template<class FS = DefaultFeatureSet, class LP = DefaultLabelPolicy>
    static std::tuple<SimpleMatrix<double>, SimpleMatrix<double>, std::vector<uint32_t>> Extract(std::vector<Bar> const& bars)
    {
        std::vector<double> dataVector;
        std::vector<double> featureVector;
        std::vector<uint32_t> labelVector;
        FS fs;
        LP lp;

        for (int i = 0; i < bars.size() - 1; i++)
        {
            auto const& bar = bars[i];
            auto const& bar_next = bars[i + 1];

            std::vector<double> b = fs.Extract(bar);
            std::vector<uint32_t> l = lp.Label(bar, bar_next);

            if (b.size() && l.size())
            {
                dataVector.push_back(static_cast<double>(bar.Timestamp));
                dataVector.push_back(bar.Open);
                dataVector.push_back(bar.High);
                dataVector.push_back(bar.Low);
                dataVector.push_back(bar.Close);
                dataVector.push_back(bar.Volume);
                featureVector.insert(featureVector.end(), b.begin(), b.end());
                labelVector.push_back(l[0]);
            }
        }

        return {
            SimpleMatrix<double>(dataVector, (static_cast<uint32_t>(dataVector.size()) / 6), 6),
            SimpleMatrix<double>(featureVector, (static_cast<uint32_t>(featureVector.size()) / fs.GetNumFeatures()), fs.GetNumFeatures()),
            labelVector};
    }
};
