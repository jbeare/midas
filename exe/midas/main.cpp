#include <stdlib.h>
#include <stdio.h>
#include <map>

#include <Simulator.h>
#include <Classifier.h>
#include <SimpleMatrix.h>
#include <FeatureFinder.h>
#include <Analyzer.h>
#include <GoogleDataBrowser.h>
#include <QuantQuoteDataBrowser.h>
#include <TwsDataBrowser.h>
#include <FeatureStream.h>
#include <LabelStream.h>
#include <TrainingSetGenerator.h>

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

    //auto specs = FeatureFinder::FindFeatures(aapl_data, aapl_labels.GetVector(), ibm, ibm_data, ibm_labels.GetVector(), aapl_data.NumCols(), 8);
    auto specs = FeatureFinder::FindFeatures(aapl_data, aapl_labels.GetVector(), aapl, aapl_data, aapl_labels.GetVector(), aapl_data.NumCols(), 8);

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

    //for (auto const& symbol : db->GetSymbols())
    {
        std::string symbol = "XXX";
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
        Analyzer::Analyze2(raw, labels, results).Print();
    }

    //Simulator<> s(db, std::make_shared<DefaultStrategy>());
    //s.Run();
}

void DoQuantQuoteStuff()
{
    auto db = QuantQuoteDataBrowser::MakeShared("D:\\codestore\\stockmarket\\RAW_DATA_DO_NOT_FUCK_UP\\order_155998");

    //for (auto const& symbol : db->GetSymbols())
    {
        std::string symbol = "aapl";
        std::cout << symbol << std::endl;
        auto range = db->GetBarRange(symbol, BarResolution::Minute);
        auto bars = db->GetBars(symbol, BarResolution::Minute, Time{0, 0, 1, 11, 2017}.GetTimeStamp(), Time{0, 0, 7, 11, 2017}.GetTimeStamp());

        if (bars.empty())
        {
            return;
        }

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
        Analyzer::Analyze2(raw, labels, results).Print();
    }
}

void DoQQFeatureFindingStuff()
{
    auto db = QuantQuoteDataBrowser::MakeShared("D:\\codestore\\stockmarket\\RAW_DATA_DO_NOT_FUCK_UP\\order_155998");

    std::string symbol = "ibm";
    std::cout << symbol << std::endl;
    auto range = db->GetBarRange(symbol, BarResolution::Minute);
    auto bars = db->GetBars(symbol, BarResolution::Minute, Time{0, 0, 1, 10, 2017}.GetTimeStamp(), Time{0, 0, 1, 11, 2017}.GetTimeStamp());

    if (bars.empty())
    {
        return;
    }

    FeatureStream<LargeFeatureSet2> fs;
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

    auto specs = FeatureFinder::FindFeatures(features, labels, raw, features, labels, 8, 8);

    std::map<uint32_t, uint32_t> featureMap;

    specs.front().Print();

    for (int i = 0; i < (specs.size() / 10); i++)
    {
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

void DoQQTrainingGeneratorStuff()
{
    auto db = QuantQuoteDataBrowser::MakeShared("D:\\codestore\\stockmarket\\RAW_DATA_DO_NOT_FUCK_UP\\order_155998");

    //for (auto const& symbol : db->GetSymbols())
    {
        std::string symbol = "ibm";
        std::cout << symbol << std::endl;
        auto range = db->GetBarRange(symbol, BarResolution::Minute);
        auto bars = db->GetBars(symbol, BarResolution::Minute, Time{0, 0, 1, 7, 2015}.GetTimeStamp(), Time{0, 0, 1, 8, 2015}.GetTimeStamp());

        if (bars.empty())
        {
            return;
        }

        FeatureStream<> fs;
        std::vector<FeatureSet> f;
        fs << bars;
        fs >> f;

        LabelStream<> ls;
        std::vector<BarLabel> l;
        ls << bars;
        ls >> l;

        auto data = AlignTimestamps(bars, f, l);

        TrainingSetComposition comp{{0, .3},{1, .2},{2, .6},{3, .3},{4, .02},{5, .01},{6, .01},{7, .01}};
        auto training = TrainingSetGenerator::Generate(comp, std::get<1>(data), std::get<2>(data));
        auto trainingFeatures = SimpleMatrixFromFeatureSetVector(training.first);
        auto trainingLabels = VectorFromLabelVector(training.second);

        auto raw = SimpleMatrixFromBarVector(std::get<0>(data));
        auto features = SimpleMatrixFromFeatureSetVector(std::get<1>(data));
        auto labels = VectorFromLabelVector(std::get<2>(data));

        /*{
            std::vector<uint32_t> results;
            auto c = Classifier::MakeShared(SimpleLabelPolicy::LabelCount, 5);
            c->Train(trainingFeatures, trainingLabels, true);
            //c->Train(features, labels, true);
            results = c->Classify(features);
            Analyzer::Analyze2(raw, labels, results).Print();
        }*/

        {
            auto specs = FeatureFinder::FindFeatures(trainingFeatures, trainingLabels, raw, features, labels, features.NumCols(), 8);

            std::map<uint32_t, uint32_t> featureMap;

            specs.front().Print();

            for (int i = 0; i < (specs.size() / 10); i++)
            {
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
    }
}

void DoTwsDataBrowserStuff()
{
    auto db = TwsDataBrowser::MakeShared("D:\\codestore\\stockmarket\\TwsStockData");

    //int tm_mday;  // day of the month - [1, 31]
    //int tm_mon;   // months since January - [0, 11]
    //int tm_year;  // years since 1900
    std::tm start{0, 0, 0, 20, 9, 117};
    std::tm end{0, 0, 0, 1, 10, 117};
    auto bars = db->GetBars("MSFT", BarResolution::Minute, mktime(&start), mktime(&end));
}

int main(int /*argc*/, char** /*argv*/)
{
    while (1)
    {
        //DoClassifyStuff();
        //DoFeatureFindingStuff();
        //DoDataBrowserStuff();
        //DoQuantQuoteStuff();
        //DoQQFeatureFindingStuff();
        //DoQQTrainingGeneratorStuff();
        DoTwsDataBrowserStuff();

        system("pause");
    }
}
