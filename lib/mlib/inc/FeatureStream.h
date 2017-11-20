#pragma once

#include <mutex>
#include <queue>
#include <vector>
#include <Bar.h>
#include <SimpleMatrix.h>
#include <MLibException.h>

class DefaultFeatureSet
{
public:
    static constexpr uint32_t HistoryDepth{6};
    static constexpr uint32_t FeatureCount{6};

    DefaultFeatureSet() {}

    std::vector<double> Extract(Bar const& bar)
    {
        if (bar.Resolution != BarResolution::Minute)
        {
            throw MLibException(E_NOTIMPL);
        }

        // TODO: This should give us better data but its making the outcomes worse.
        //if (!m_queue.empty() && ((bar.Timestamp - m_queue.back().Timestamp) != 60))
        //{
        //    m_queue = std::queue<Bar>();
        //}

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

template<class F = DefaultFeatureSet>
class FeatureStream
{
public:
    FeatureStream() {}

    void operator<< (_In_ std::vector<Bar> const& bars)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto const& bar : bars)
        {
            std::vector<double> features = m_featureExtractor.Extract(bar);
            if (features.size())
            {
                m_featureSets.push_back(
                {
                    bar.Symbol,
                    bar.Timestamp,
                    bar.Resolution,
                    std::move(features)
                });
            }
        }
    }

    void operator>> (_Out_ std::vector<FeatureSet>& features)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        features.insert(features.end(), m_featureSets.begin(), m_featureSets.end());
        m_featureSets.clear();
    }

private:
    F m_featureExtractor;
    _Guarded_by_(m_mutex) std::mutex m_mutex;
    _Guarded_by_(m_mutex) std::vector<FeatureSet> m_featureSets;
};
