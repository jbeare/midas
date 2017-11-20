#include <stdlib.h>
#include <stdio.h>
#include <map>

#include <Simulator.h>
#include <Classifier.h>
#include <SimpleMatrix.h>
#include <FeatureFinder.h>
#include <Analyzer.h>
#include <DataBrowser.h>
#include <FeatureStream.h>
#include <LabelStream.h>

/*SimpleMatrix<double>
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
}*/

void DoDataBrowserStuff()
{
    auto db = GoogleDataBrowser::MakeShared("D:\\codestore\\stockmarket\\Stocks");

    /*for (auto const& symbol : db->GetSymbols())
    {
        std::cout << symbol << std::endl;
        auto bars = db->GetBars(symbol, BarResolution::Minute, 0u, db->GetBarCount(symbol, BarResolution::Minute) - 1);
        FeatureStream<> fs;
        std::vector<FeatureSet> f;
        fs << bars;
        fs >> f;

        LabelStream<> ls;
        std::vector<BarLabel> l;
        ls << bars;
        ls >> l;

        auto data = AlignTimestamps(bars, f, l);
        auto raw = SimpleMatrixFromBarVector(std::get<0>(data));
        auto features = SimpleMatrixFromFeatureSetVector(std::get<1>(data));
        auto labels = VectorFromLabelVector(std::get<2>(data));

        std::vector<uint32_t> results;
        auto c = Classifier::MakeShared(8, 5);
        c->Train(features, labels, true);
        results = c->Classify(features);
        Analyzer::Analyze(raw, labels, results).Print();
    }*/


    Simulator<> s(db, std::make_shared<DefaultStrategy>());
    s.Run();
}

void DoQuantQuoteStuff()
{
    auto db = QuantQuoteDataBrowser::MakeShared("D:\\codestore\\stockmarket\\RAW_DATA_DO_NOT_FUCK_UP\\order_155998");
}

int main(int /*argc*/, char** /*argv*/)
{
    while (1)
    {
        //DoClassifyStuff();
        //DoFeatureFindingStuff();
        //DoDataBrowserStuff();
        DoQuantQuoteStuff();

        system("pause");
    }
}
