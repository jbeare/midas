#include <stdlib.h>
#include <stdio.h>
#include <map>

#include <Classifier.h>
#include <SimpleMatrix.h>
#include <FeatureFinder.h>

int main(int /*argc*/, char** /*argv*/)
{
    while (1)
    {
        /*arma::mat aapl_data, ibm_data, intc_data;
        arma::Row<size_t> aapl_labels, ibm_labels, intc_labels;
        
        aapl_data.load("AAPL_observations.csv", arma::file_type::csv_ascii);
        aapl_labels.load("AAPL_labels.csv", arma::file_type::csv_ascii);
        ibm_data.load("IBM_observations.csv", arma::file_type::csv_ascii);
        ibm_labels.load("IBM_labels.csv", arma::file_type::csv_ascii);
        intc_data.load("INTC_observations.csv", arma::file_type::csv_ascii);
        intc_labels.load("INTC_labels.csv", arma::file_type::csv_ascii);

        aapl_data = aapl_data.t();
        ibm_data = ibm_data.t();
        intc_data = intc_data.t();

        arma::mat aapl, ibm, intc;

        aapl.load("AAPL_raw.csv", arma::file_type::csv_ascii);
        ibm.load("IBM_raw.csv", arma::file_type::csv_ascii);
        intc.load("INTC_raw.csv", arma::file_type::csv_ascii);

        arma::Row<size_t> aapl_results, ibm_results, intc_results;*/

        /*
        auto specs = FeatureFinder::FindFeatures(aapl_data, aapl_labels, ibm, ibm_data, ibm_labels, aapl_data.n_rows);

        std::map<uint32_t, uint32_t> featureMap;

        for (auto& spec : specs)
        {
            for (auto& feature : spec.Features)
            {
                featureMap[feature]++;
            }
        }

        for (auto& pair : featureMap)
        {
            printf("%d : %d\n", pair.first, pair.second);
        }
        */

        /*{
            auto c = Classifier::MakeShared(8, 5);
            c->Train("AAPL_observations.csv", "AAPL_labels.csv", true);

            aapl_results.resize(aapl_data.n_cols);
            for (int i = 0; i < aapl_data.n_cols; i++)
            {
                aapl_results[i] = c->Classify(arma::conv_to<std::vector<double>>::from(aapl_data.col(i)));
            }

            ibm_results.resize(ibm_data.n_cols);
            for (int i = 0; i < ibm_data.n_cols; i++)
            {
                ibm_results[i] = c->Classify(arma::conv_to<std::vector<double>>::from(ibm_data.col(i)));
            }

            intc_results.resize(intc_data.n_cols);
            for (int i = 0; i < intc_data.n_cols; i++)
            {
                intc_results[i] = c->Classify(arma::conv_to<std::vector<double>>::from(intc_data.col(i)));
            }

            FeatureFinder::Analyze(aapl, aapl_labels, aapl_results);
            FeatureFinder::Analyze(ibm, ibm_labels, ibm_results);
            FeatureFinder::Analyze(intc, intc_labels, intc_results);
        }*/

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

        std::vector<uint32_t> aapl_results, ibm_results, intc_results;

        {
            {
                auto c = Classifier::MakeShared(8, 5);
                c->Train("AAPL_observations.csv", "AAPL_labels.csv", true);

                aapl_results.resize(aapl_data.NumRows());
                for (uint32_t i = 0; i < aapl_data.NumRows(); i++)
                {
                    aapl_results[i] = c->Classify(aapl_data.Row(i).GetVector());
                }

                c->Store("AAPL_Archive");
            }

            {
                auto c = Classifier::MakeShared(8, 5);
                c->Train("IBM_observations.csv", "IBM_labels.csv", true);

                ibm_results.resize(ibm_data.NumRows());
                for (uint32_t i = 0; i < ibm_data.NumRows(); i++)
                {
                    ibm_results[i] = c->Classify(ibm_data.Row(i).GetVector());
                }

                c->Store("IBM_Archive");
            }

            {
                auto c = Classifier::MakeShared(8, 5);
                c->Train("INTC_observations.csv", "INTC_labels.csv", true);

                intc_results.resize(intc_data.NumRows());
                for (uint32_t i = 0; i < intc_data.NumRows(); i++)
                {
                    intc_results[i] = c->Classify(intc_data.Row(i).GetVector());
                }

                c->Store("INTC_Archive");
            }

            FeatureFinder::Analyze(aapl, aapl_labels.GetVector(), aapl_results);
            FeatureFinder::Analyze(ibm, ibm_labels.GetVector(), ibm_results);
            FeatureFinder::Analyze(intc, intc_labels.GetVector(), intc_results);
        }

        {
            {
                auto c = Classifier::MakeShared(8, 5);
                c->Train(aapl_data, aapl_labels.GetVector(), true);

                aapl_results.resize(aapl_data.NumRows());
                for (uint32_t i = 0; i < aapl_data.NumRows(); i++)
                {
                    aapl_results[i] = c->Classify(aapl_data.Row(i).GetVector());
                }

                c->Store("AAPL_Archive");
            }

            {
                auto c = Classifier::MakeShared(8, 5);
                c->Train(ibm_data, ibm_labels.GetVector(), true);

                ibm_results.resize(ibm_data.NumRows());
                for (uint32_t i = 0; i < ibm_data.NumRows(); i++)
                {
                    ibm_results[i] = c->Classify(ibm_data.Row(i).GetVector());
                }

                c->Store("IBM_Archive");
            }

            {
                auto c = Classifier::MakeShared(8, 5);
                c->Train(intc_data, intc_labels.GetVector(), true);

                intc_results.resize(intc_data.NumRows());
                for (uint32_t i = 0; i < intc_data.NumRows(); i++)
                {
                    intc_results[i] = c->Classify(intc_data.Row(i).GetVector());
                }

                c->Store("INTC_Archive");
            }

            FeatureFinder::Analyze(aapl, aapl_labels.GetVector(), aapl_results);
            FeatureFinder::Analyze(ibm, ibm_labels.GetVector(), ibm_results);
            FeatureFinder::Analyze(intc, intc_labels.GetVector(), intc_results);
        }

        {
            {
                auto c = Classifier::MakeShared("AAPL_Archive");

                aapl_results.resize(aapl_data.NumRows());
                for (uint32_t i = 0; i < aapl_data.NumRows(); i++)
                {
                    aapl_results[i] = c->Classify(aapl_data.Row(i).GetVector());
                }
            }

            {
                auto c = Classifier::MakeShared("IBM_Archive");

                ibm_results.resize(ibm_data.NumRows());
                for (uint32_t i = 0; i < ibm_data.NumRows(); i++)
                {
                    ibm_results[i] = c->Classify(ibm_data.Row(i).GetVector());
                }
            }

            {
                auto c = Classifier::MakeShared("INTC_Archive");

                intc_results.resize(intc_data.NumRows());
                for (uint32_t i = 0; i < intc_data.NumRows(); i++)
                {
                    intc_results[i] = c->Classify(intc_data.Row(i).GetVector());
                }
            }

            FeatureFinder::Analyze(aapl, aapl_labels.GetVector(), aapl_results);
            FeatureFinder::Analyze(ibm, ibm_labels.GetVector(), ibm_results);
            FeatureFinder::Analyze(intc, intc_labels.GetVector(), intc_results);
        }

        {
            {
                auto c = Classifier::MakeShared("AAPL_Archive");
                aapl_results = c->Classify(aapl_data);
            }

            {
                auto c = Classifier::MakeShared("IBM_Archive");
                ibm_results = c->Classify(ibm_data);
            }

            {
                auto c = Classifier::MakeShared("INTC_Archive");
                intc_results = c->Classify(intc_data);
            }

            FeatureFinder::Analyze(aapl, aapl_labels.GetVector(), aapl_results);
            FeatureFinder::Analyze(ibm, ibm_labels.GetVector(), ibm_results);
            FeatureFinder::Analyze(intc, intc_labels.GetVector(), intc_results);
        }

        {
            {
                auto c = Classifier::MakeShared("AAPL_Archive");
                aapl_results = c->Classify("AAPL_observations.csv");
            }

            {
                auto c = Classifier::MakeShared("IBM_Archive");
                ibm_results = c->Classify("IBM_observations.csv");
            }

            {
                auto c = Classifier::MakeShared("INTC_Archive");
                intc_results = c->Classify("INTC_observations.csv");
            }

            FeatureFinder::Analyze(aapl, aapl_labels.GetVector(), aapl_results);
            FeatureFinder::Analyze(ibm, ibm_labels.GetVector(), ibm_results);
            FeatureFinder::Analyze(intc, intc_labels.GetVector(), intc_results);
        }

        system("pause");
    }
}
