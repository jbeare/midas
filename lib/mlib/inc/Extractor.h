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
        if ((bar.Resolution != BarResolution::Minute) || (bar.Resolution != bar_next.Resolution))
        {
            throw MLibException(E_NOTIMPL);
        }

        // TODO: This should give us better data but its making the outcomes worse.
        //if ((bar_next.Timestamp - bar.Timestamp) != 60)
        //{
        //    return std::vector<uint32_t>();
        //}

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

class Extractor
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

template<class L = DefaultLabelPolicy>
class LabelStream
{
public:
    void operator<< (_In_ std::vector<Bar> const& bars)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (int i = 0; i < bars.size() - 1; i++)
        {
            auto const& bar = bars[i];
            auto const& bar_next = bars[i + 1];

            std::vector<uint32_t> label = m_labelPolicy.Label(bar, bar_next);
            if (label.size())
            {
                m_labels.push_back(
                {
                    bar.Symbol,
                    bar.Timestamp,
                    bar.Resolution,
                    label[0]
                });
            }
        }
    }

    void operator>> (_Out_ std::vector<BarLabel>& labels)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        labels.insert(labels.end(), m_labels.begin(), m_labels.end());
        m_labels.clear();
    }

private:
    std::mutex m_mutex;
    _Guarded_by_(m_mutex) L m_labelPolicy;
    _Guarded_by_(m_mutex) std::vector<BarLabel> m_labels;
};
