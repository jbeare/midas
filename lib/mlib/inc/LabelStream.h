#pragma once

#include <mutex>
#include <vector>
#include <Bar.h>
#include <MLibException.h>

typedef std::vector<double> LabelConfig;

class LabelPolicy
{
public:
    LabelPolicy(LabelConfig const& config) :
        m_config(config)
    {
        if (m_config.empty())
        {
            throw MLibException(E_INVALIDARG);
        }
        std::sort(m_config.rbegin(), m_config.rend());
    }

    virtual std::vector<uint32_t> Label(Bar const& bar, Bar const& bar_next)
    {
        if ((bar.Resolution != BarResolution::Minute) || (bar.Resolution != bar_next.Resolution))
        {
            throw MLibException(E_NOTIMPL);
        }

        if ((bar_next.Timestamp - bar.Timestamp) != 60)
        {
            return {};
        }

        auto delta = (bar_next.Close - bar.Close) / bar.Close;

        for (uint32_t i = 0; i < static_cast<uint32_t>(m_config.size()); i++)
        {
            if (delta >= m_config[i])
            {
                return {static_cast<uint32_t>(m_config.size()) - i};
            }
        }

        return {0};
    }

    virtual uint32_t LabelCount()
    {
        return static_cast<uint32_t>(m_config.size() + 1);
    }

private:
    LabelConfig m_config;
};

class LabelStream
{
public:
    LabelStream(LabelConfig const& config = {0.0001}) :
        m_labelPolicy(config)
    {

    }

    uint32_t LabelCount()
    {
        return m_labelPolicy.LabelCount();
    }

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
    _Guarded_by_(m_mutex) LabelPolicy m_labelPolicy;
    _Guarded_by_(m_mutex) std::vector<BarLabel> m_labels;
};
