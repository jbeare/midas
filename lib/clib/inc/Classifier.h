#pragma once

#include <memory>
#include <sal.h>
#include <string>
#include <vector>

class __declspec(dllexport) Classifier
{
public:
    static std::shared_ptr<Classifier> MakeShared(_In_ uint32_t classes, _In_ uint32_t dimensions);

    static std::shared_ptr<Classifier> MakeShared(_In_ std::string name);

    virtual void Train(_In_ std::string trainingDataPath, _In_ std::string trainingLabelsPath, _In_ bool updatePca) = 0;

    virtual uint32_t Classify(_In_ std::vector<double> data) = 0;

    virtual void Load(_In_ std::string name) = 0;

    virtual void Store(_In_ std::string name) = 0;
};
