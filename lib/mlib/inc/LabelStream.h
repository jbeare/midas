#pragma once

#include <mutex>
#include <queue>
#include <vector>
#include <Bar.h>
#include <SimpleMatrix.h>
#include <MLibException.h>

class DefaultLabelPolicy
{
public:
    static constexpr uint32_t LabelCount{8};

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
