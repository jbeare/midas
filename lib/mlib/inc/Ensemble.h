#pragma once

#include <Classifier.h>
#include <vector>
#include <iostream>
#include <fstream>

class Ensemble
{
public:
    static std::shared_ptr<Ensemble> MakeShared(_In_ std::vector<std::shared_ptr<Classifier>> classifiers)
    {
        return std::make_shared<Ensemble>(classifiers);
    }

    Ensemble(_In_ std::vector<std::shared_ptr<Classifier>> classifiers) :
        m_classifiers(classifiers)
    {

    }

    uint32_t Classify(_In_ std::vector<double> const& data)
    {
        uint32_t result{};
        for (auto& classifier : m_classifiers)
        {
            result += classifier->Classify(data);
        }
        return result / static_cast<uint32_t>(m_classifiers.size());
    }

    std::vector<uint32_t> Classify(_In_ SimpleMatrix<double> const& data)
    {
        std::vector<uint32_t> results;
        for (uint32_t i = 0; i < data.NumRows(); i++)
        {
            results.push_back(Classify(data.Row(i).GetVector()));
        }
        return results;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int = 0)
    {
        ar & boost::serialization::make_nvp("Ensemble_m_classifiers", m_classifiers);
    }

private:
    std::vector<std::shared_ptr<Classifier>> m_classifiers;
};
