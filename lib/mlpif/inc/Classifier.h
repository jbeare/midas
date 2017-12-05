#pragma once

#include <memory>
#include <sal.h>
#include <string>
#include <vector>

#include <SimpleMatrix.h>

class __declspec(dllexport) Classifier
{
public:
    static std::shared_ptr<Classifier> MakeShared(_In_ uint32_t classes, _In_ uint32_t dimensions);

    static std::shared_ptr<Classifier> MakeShared(_In_ std::string const& name);

    virtual void Train(_In_ std::string const& trainingDataPath, _In_ std::string const& trainingLabelsPath, _In_ bool updatePca) = 0;

    virtual void Train(_In_ SimpleMatrix<double> const& trainingData, _In_ std::vector<uint32_t> const& trainingLabels, _In_ bool updatePca) = 0;

    virtual std::vector<uint32_t> Classify(_In_ std::string const& dataPath) = 0;

    virtual std::vector<uint32_t> Classify(_In_ SimpleMatrix<double> const& data) = 0;

    virtual uint32_t Classify(_In_ std::vector<double> const& data) = 0;

    virtual void Save(_In_ std::string const& name) = 0;

    template<typename Archive>
    void serialize(_Inout_ Archive& ar, _In_ const unsigned int = 0);
};
