#pragma once

#include <ctime>
#include <string>

enum class BarResolution
{
    Second,
    Minute,
    Hour,
    Day
};

struct BarBase
{
    std::string Symbol{};
    std::time_t Timestamp{};
    BarResolution Resolution{};
};

struct Bar : BarBase
{
    Bar() {}

    Bar(std::string symbol, std::time_t timestamp, BarResolution resolution, double open, double high, double low, double close, double volume) :
        BarBase{symbol, timestamp, resolution},
        Open(open),
        High(high),
        Low(low),
        Close(close),
        Volume(volume)
    {

    }

    double Open{};
    double High{};
    double Low{};
    double Close{};
    double Volume{};
};

struct FeatureSet : BarBase
{
    FeatureSet(std::string symbol, std::time_t timestamp, BarResolution resolution, std::vector<double> features) :
        BarBase{symbol, timestamp, resolution},
        Features(features)
    {
    
    }

    std::vector<double> Features{};
};

struct BarLabel : BarBase
{
    BarLabel(std::string symbol, std::time_t timestamp, BarResolution resolution, uint32_t label) :
        BarBase{symbol, timestamp, resolution},
        Label(label)
    {

    }

    uint32_t Label{};
};

SimpleMatrix<double> SimpleMatrixFromBarVector(std::vector<Bar> const& bars)
{
    std::vector<double> dataVector;

    for (auto const& bar : bars)
    {
        dataVector.push_back(static_cast<double>(bar.Timestamp));
        dataVector.push_back(bar.Open);
        dataVector.push_back(bar.High);
        dataVector.push_back(bar.Low);
        dataVector.push_back(bar.Close);
        dataVector.push_back(bar.Volume);
    }

    return SimpleMatrix<double>(dataVector, static_cast<uint32_t>(bars.size()), static_cast<uint32_t>(dataVector.size() / bars.size()));
}

SimpleMatrix<double> SimpleMatrixFromFeatureSetVector(std::vector<FeatureSet> const& featureSets)
{
    std::vector<double> dataVector;

    for (auto const& featureSet : featureSets)
    {
        dataVector.insert(dataVector.end(), featureSet.Features.begin(), featureSet.Features.end());
    }

    return SimpleMatrix<double>(dataVector, static_cast<uint32_t>(featureSets.size()), static_cast<uint32_t>(dataVector.size() / featureSets.size()));
}

std::vector<uint32_t> VectorFromLabelVector(std::vector<BarLabel> const& labels)
{
    std::vector<uint32_t> dataVector;

    for (auto const& label : labels)
    {
        dataVector.push_back(label.Label);
    }

    return dataVector;
}

std::tuple<std::vector<Bar>, std::vector<FeatureSet>, std::vector<BarLabel>> AlignTimestamps(std::vector<Bar> const& bars, std::vector<FeatureSet> const& featureSets, std::vector<BarLabel> const& labels)
{
    std::vector<Bar> alignedBars;
    std::vector<FeatureSet> alignedFeatureSets;
    std::vector<BarLabel> alignedLabels;

    auto iBar = bars.begin();
    auto iFeature = featureSets.begin();
    auto iLabel = labels.begin();

    while (iBar != bars.end())
    {
        while ((iFeature != featureSets.end()) && (iFeature->Timestamp < iBar->Timestamp))
        {
            iFeature++;
        }

        while ((iLabel != labels.end()) && (iLabel->Timestamp < iBar->Timestamp))
        {
            iLabel++;
        }

        if ((iFeature == featureSets.end()) || (iLabel == labels.end()))
        {
            break;
        }

        if((iBar->Timestamp == iFeature->Timestamp) && (iBar->Timestamp == iLabel->Timestamp))
        {
            alignedBars.push_back(*iBar);
            alignedFeatureSets.push_back(*iFeature);
            alignedLabels.push_back(*iLabel);
        }

        iBar++;
    }

    return {alignedBars, alignedFeatureSets, alignedLabels};
}