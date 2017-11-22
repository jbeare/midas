#pragma once

#include <map>
#include <vector>
#include <Bar.h>
#include <MLibException.h>

typedef std::map<uint32_t, double> TrainingSetComposition;

class TrainingSetGenerator
{
public:
    static std::pair<std::vector<FeatureSet>, std::vector<BarLabel>> Generate(TrainingSetComposition const& composition, std::vector<FeatureSet> const& featureSets, std::vector<BarLabel> const& labels)
    {
        /*double totalComp{};

        for (auto const& comp : composition)
        {
            totalComp += comp.second;
        }
        if (totalComp != 1)
        {
            throw MLibException(E_INVALIDARG);
        }*/

        if (featureSets.size() != labels.size())
        {
            throw MLibException(E_INVALIDARG);
        }

        std::map<uint32_t, std::vector<FeatureSet>> featureMap;
        std::map<uint32_t, std::vector<BarLabel>> labelMap;

        for (int i = 0; i < labels.size(); i++)
        {
            featureMap[labels[i].Label].push_back(featureSets[i]);
            labelMap[labels[i].Label].push_back(labels[i]);
        }

        // Should we randomize the vectors here?

        double targetSize{static_cast<double>(featureSets.size())};
        std::vector<FeatureSet> f;
        std::vector<BarLabel> l;

        for (auto const& comp : composition)
        {
            uint32_t targetLabelCount{static_cast<uint32_t>(targetSize * comp.second)};
            while (!featureMap[comp.first].empty() && (targetLabelCount > 0))
            {
                uint32_t copySize = min(static_cast<uint32_t>(featureMap[comp.first].size()), targetLabelCount);
                f.insert(f.end(), featureMap[comp.first].begin(), featureMap[comp.first].begin() + copySize);
                l.insert(l.end(), labelMap[comp.first].begin(), labelMap[comp.first].begin() + copySize);
                targetLabelCount -= copySize;
            }
        }

        return {f, l};
    }
};
