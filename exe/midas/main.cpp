#include <stdlib.h>
#include <stdio.h>
#include <map>

#include <Classifier.h>
#include <SimpleMatrix.h>
#include <FeatureFinder.h>
#include <Analyzer.h>

SimpleMatrix<double>
    aapl_data("AAPL_observations.csv"),
    ibm_data("IBM_observations.csv"),
    intc_data("INTC_observations.csv");
SimpleMatrix<uint32_t>
    aapl_labels("AAPL_labels.csv"),
    ibm_labels("IBM_labels.csv"),
    intc_labels("INTC_labels.csv");
SimpleMatrix<double>
    aapl("AAPL_raw.csv"),
    ibm("IBM_raw.csv"),
    intc("INTC_raw.csv");

void DoClassifyStuff()
{
    std::vector<uint32_t> aapl_results, ibm_results, intc_results;

    {
        auto c = Classifier::MakeShared(8, 5);
        c->Train(aapl_data, aapl_labels.GetVector(), true);
        aapl_results = c->Classify(aapl_data);
    }

    {
        auto c = Classifier::MakeShared(8, 5);
        c->Train(ibm_data, ibm_labels.GetVector(), true);
        ibm_results = c->Classify(ibm_data);
    }

    {
        auto c = Classifier::MakeShared(8, 5);
        c->Train(intc_data, intc_labels.GetVector(), true);
        intc_results = c->Classify(intc_data);
    }

    Analyzer::Analyze(aapl, aapl_labels.GetVector(), aapl_results).Print();
    Analyzer::Analyze(ibm, ibm_labels.GetVector(), ibm_results).Print();
    Analyzer::Analyze(intc, intc_labels.GetVector(), intc_results).Print();
}

void DoFeatureFindingStuff()
{
    std::vector<uint32_t> aapl_results, ibm_results, intc_results;

    //auto specs = FeatureFinder::FindFeatures(aapl_data, aapl_labels.GetVector(), ibm, ibm_data, ibm_labels.GetVector(), aapl_data.NumCols());
    auto specs = FeatureFinder::FindFeatures(aapl_data, aapl_labels.GetVector(), aapl, aapl_data, aapl_labels.GetVector(), aapl_data.NumCols());

    std::map<uint32_t, uint32_t> featureMap;

    for (int i = 0; i < (specs.size() / 10); i++)
    {
        specs[i].Print();

        for (auto& feature : specs[i].Features)
        {
            featureMap[feature]++;
        }
    }

    for (auto& pair : featureMap)
    {
        printf("%d : %d\n", pair.first, pair.second);
    }
}

int main(int /*argc*/, char** /*argv*/)
{
    while (1)
    {
        DoClassifyStuff();
        DoFeatureFindingStuff();

        system("pause");
    }
}
