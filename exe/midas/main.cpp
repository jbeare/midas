#include <stdlib.h>
#include <stdio.h>
#include <map>

#pragma warning(disable : 4348 4267 4244 4458 6285 6287 6011 28251)
#include <mlpack/core.hpp>
#include <mlpack/methods/naive_bayes/naive_bayes_classifier.hpp>
#include <mlpack/methods/pca/pca.hpp>
#pragma warning(default : 4348 4267 4244 4458 6285 6287 6011 28251)

#include <Classifier.h>

#include "FeatureFinder.h"

using namespace mlpack;

int main(int /*argc*/, char** /*argv*/)
{
    while (1)
    {
        arma::mat training_data, testing_data, aapl_data, ibm_data, intc_data;
        arma::Row<size_t> training_labels, testing_labels, aapl_labels, ibm_labels, intc_labels;
        
        aapl_data.load("AAPL_observations.csv", arma::file_type::csv_ascii);
        aapl_labels.load("AAPL_labels.csv", arma::file_type::csv_ascii);
        ibm_data.load("IBM_observations.csv", arma::file_type::csv_ascii);
        ibm_labels.load("IBM_labels.csv", arma::file_type::csv_ascii);
        intc_data.load("INTC_observations.csv", arma::file_type::csv_ascii);
        intc_labels.load("INTC_labels.csv", arma::file_type::csv_ascii);

        aapl_data = aapl_data.t();
        ibm_data = ibm_data.t();
        intc_data = intc_data.t();

        testing_data = aapl_data.cols(arma::span(aapl_data.n_cols - 800, aapl_data.n_cols - 1));
        testing_labels = aapl_labels.cols(arma::span(aapl_labels.n_cols - 800, aapl_labels.n_cols - 1));
        training_data = aapl_data.cols(arma::span(0, aapl_data.n_cols - 801));
        training_labels = aapl_labels.cols(arma::span(0, aapl_labels.n_cols - 801));

        arma::mat aapl, ibm, intc;

        aapl.load("AAPL_raw.csv", arma::file_type::csv_ascii);
        ibm.load("IBM_raw.csv", arma::file_type::csv_ascii);
        intc.load("INTC_raw.csv", arma::file_type::csv_ascii);

        arma::Row<size_t> aapl_results, ibm_results, intc_results, testing_results;

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

        {
            {
                auto c = Classifier::MakeShared(8, 5);
                c->Train("AAPL_observations.csv", "AAPL_labels.csv", true);

                aapl_results.resize(aapl_data.n_cols);
                for (int i = 0; i < aapl_data.n_cols; i++)
                {
                    aapl_results[i] = c->Classify(arma::conv_to<std::vector<double>>::from(aapl_data.col(i)));
                }
            }

            {
                auto c = Classifier::MakeShared(8, 5);
                c->Train("IBM_observations.csv", "IBM_labels.csv", true);

                ibm_results.resize(ibm_data.n_cols);
                for (int i = 0; i < ibm_data.n_cols; i++)
                {
                    ibm_results[i] = c->Classify(arma::conv_to<std::vector<double>>::from(ibm_data.col(i)));
                }
            }

            {
                auto c = Classifier::MakeShared(8, 5);
                c->Train("INTC_observations.csv", "INTC_labels.csv", true);

                intc_results.resize(intc_data.n_cols);
                for (int i = 0; i < intc_data.n_cols; i++)
                {
                    intc_results[i] = c->Classify(arma::conv_to<std::vector<double>>::from(intc_data.col(i)));
                }
            }

            FeatureFinder::Analyze(aapl, aapl_labels, aapl_results);
            FeatureFinder::Analyze(ibm, ibm_labels, ibm_results);
            FeatureFinder::Analyze(intc, intc_labels, intc_results);
        }

        system("pause");
    }
}
