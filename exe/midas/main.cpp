#include <stdlib.h>
#include <stdio.h>
#include <mlpack/core.hpp>
#include <mlpack/methods/naive_bayes/naive_bayes_classifier.hpp>
#include <mlpack/methods/pca/pca.hpp>
#include "Classifier.h"
#include "FeatureFinder.h"

using namespace mlpack;

int main(int argc, char** argv)
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

        FeatureFinder::FindFeatures(aapl_data, aapl_labels, ibm, ibm_data, ibm_labels);

        //static int dimensions = 5;
        //dimensions++;
        // Experimentally, 7 dimensions seems to work well for the # of features I chose...
        Midas::Classifier c(training_data, training_labels, 7, 8);
        aapl_results = c.Classify(aapl_data);
        ibm_results = c.Classify(ibm_data);
        intc_results = c.Classify(intc_data);

        system("pause");
    }
}
