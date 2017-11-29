#pragma once

#include <vector>
#include <LabelStream.h>
#include <SimpleMatrix.h>
#include <WorkPool.h>
#include <Classifier.h>
#include <Analyzer.h>
#include <Bar.h>
#include <FeatureStream.h>

class LabelFinderResult
{
public:
    LabelConfig Config;
    Analysis Details;

    bool operator< (LabelFinderResult const& f)
    {
        return (Score() > f.Score()) ? true : false;
    }

private:
    double Score() const
    {
        auto net = Details.netSpread[1] + Details.netSpread[2] + Details.netSpread[3] + Details.netSpread[4] + Details.netSpread[5] + Details.netSpread[6] + Details.netSpread[7];
        return net - (Details.trades * 0.005);
    }
};

class LabelFinder
{
public:
    // Data is in row=datapoint column=feature format.
    std::vector<LabelFinderResult> FindLabelConfig(
        _In_ std::vector<Bar> const& trainingBars,
        _In_ std::vector<Bar> const& testingBars,
        _In_ uint32_t dimensionCount,
        _In_ uint32_t labelCount)
    {
        if (labelCount > 2)
        {
            throw MLibException(E_NOTIMPL);
        }

        std::vector<FeatureSet> trainingFs;
        std::vector<FeatureSet> testingFs;

        {
            FeatureStream<> fs;
            fs << trainingBars;
            fs >> trainingFs;
        }

        {
            FeatureStream<> fs;
            fs << testingBars;
            fs >> testingFs;
        }

        for (double config = -0.01; config < 0.01; config += 0.00001)
        {
            m_workPool.QueueWork([&, config]()
            {
                _FindLabelConfig(trainingBars, testingBars, trainingFs, testingFs, dimensionCount, LabelConfig{config});
            });
        }

        m_workPool.WaitForCompletion();

        std::sort(m_results.begin(), m_results.end());
        return m_results;
    }

private:
    void _FindLabelConfig(
        _In_ std::vector<Bar> const& trainingBars,
        _In_ std::vector<Bar> const& testingBars,
        _In_ std::vector<FeatureSet> const& trainingFs,
        _In_ std::vector<FeatureSet> const& testingFs,
        _In_ uint32_t dimensionCount,
        _In_ LabelConfig config)
    {
        std::vector<BarLabel> trainingBl;
        std::vector<BarLabel> testingBl;

        {
            LabelStream ls(config);
            ls << trainingBars;
            ls >> trainingBl;
        }

        {
            LabelStream ls(config);
            ls << testingBars;
            ls >> testingBl;
        }

        auto trainingData = AlignTimestamps(trainingBars, trainingFs, trainingBl);
        auto trainingRaw = SimpleMatrixFromBarVector(std::get<0>(trainingData));
        auto trainingFeatures = SimpleMatrixFromFeatureSetVector(std::get<1>(trainingData));
        auto trainingLabels = VectorFromLabelVector(std::get<2>(trainingData));

        auto testingData = AlignTimestamps(testingBars, testingFs, testingBl);
        auto testingRaw = SimpleMatrixFromBarVector(std::get<0>(testingData));
        auto testingFeatures = SimpleMatrixFromFeatureSetVector(std::get<1>(testingData));
        auto testingLabels = VectorFromLabelVector(std::get<2>(testingData));

        auto c = Classifier::MakeShared(static_cast<uint32_t>(config.size()) + 1, dimensionCount);
        c->Train(trainingFeatures, trainingLabels, true);
        auto testResults = c->Classify(testingFeatures);
        auto a = Analyzer::Analyze2(testingRaw, testingLabels, testResults);
        std::lock_guard<std::mutex> lock(m_mutex);
        m_results.push_back(LabelFinderResult{config, a});
    }

    WorkPool m_workPool;
    std::mutex m_mutex;
    _Guarded_by_(m_mutex) std::vector<LabelFinderResult> m_results;
};
