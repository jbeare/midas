#include <stdlib.h>
#include <stdio.h>
#include <mlpack/core.hpp>
#include <mlpack/methods/naive_bayes/naive_bayes_classifier.hpp>
#include <mlpack/methods/pca/pca.hpp>
#include "Classifier.h"

using namespace mlpack;

void Analyze(arma::mat& data, arma::Row<size_t>& labels, arma::Row<size_t>& results)
{
    if ((data.n_rows != labels.n_cols) || (results.n_cols != labels.n_cols) || (results.n_rows != labels.n_rows))
    {
        printf("Data error!\n");
        return;
    }

    float maxGain[8] = {};
    float actGain[8] = {};
    int labelSpread[8]{};
    int resultSpread[8]{};
    int correctSpread[8]{};
    int underSpread[8]{};
    int overSpread[8]{};
    int correct{};
    int under{};
    int over{};

    for (int i = 1; i < data.n_rows - 1; i++)
    {
        labelSpread[labels[i]]++;
        resultSpread[results[i]]++;

        if (labels[i] == results[i]) // We classified the data correctly.
        {
            // Gain profit equal to the classification.
            if (labels[i] > 0)
            {
                actGain[results[i]]++;
            }
            correct++;
            correctSpread[results[i]]++;
        }
        else if (labels[i] > results[i]) // We underestimated the classification.
        {
            // Gain profit equal to the under-classification.
            if (labels[i] > 0)
            {
                actGain[results[i]]++;
            }
            under++;
            underSpread[labels[i] - results[i]]++;
        }
        else // We overestimated the classification.
        {
            // It's a wash.
            over++;
            overSpread[results[i] - labels[i]]++;
        }
    }

    maxGain[0] = labelSpread[0] * 0;
    maxGain[1] = labelSpread[1] * 0.005;
    maxGain[2] = labelSpread[2] * 0.01;
    maxGain[3] = labelSpread[3] * 0.05;
    maxGain[4] = labelSpread[4] * 0.10;
    maxGain[5] = labelSpread[5] * 0.15;
    maxGain[6] = labelSpread[6] * 0.20;
    maxGain[7] = labelSpread[7] * 0.25;
    float maxGainTotal = maxGain[0] + maxGain[1] + maxGain[2] + maxGain[3] + maxGain[4] + maxGain[5] + maxGain[6] + maxGain[7];

    actGain[0] = actGain[0] * 0;
    actGain[1] = actGain[1] * 0.005;
    actGain[2] = actGain[2] * 0.01;
    actGain[3] = actGain[3] * 0.05;
    actGain[4] = actGain[4] * 0.10;
    actGain[5] = actGain[5] * 0.15;
    actGain[6] = actGain[6] * 0.20;
    actGain[7] = actGain[7] * 0.25;
    float actGainTotal = actGain[0] + actGain[1] + actGain[2] + actGain[3] + actGain[4] + actGain[5] + actGain[6] + actGain[7];

    // Accuracy (spot on, over, under)
    // Oppourtunities taken (taken chances/total chances, correct changes/total chances)
    // Gainz (actual gain / max gain)

    int c = correct;
    int r = data.n_rows;
    printf("Accuracy +/-0:%d/%d(%.2f) +/-1:%d/%d(%.2f) +/-2:%d/%d(%.2f) +/-3:%d/%d(%.2f) +/-4:%d/%d(%.2f)\n",
        c, r, (float)c / r,
        c + underSpread[1] + overSpread[1], r, ((float)c + underSpread[1] + overSpread[1]) / r,
        c + underSpread[1] + overSpread[1] + underSpread[2] + overSpread[2], r, ((float)c + underSpread[1] + overSpread[1] + underSpread[2] + overSpread[2]) / r,
        c + underSpread[1] + overSpread[1] + underSpread[2] + overSpread[2] + underSpread[3] + overSpread[3], r, ((float)c + underSpread[1] + overSpread[1] + underSpread[2] + overSpread[2] + underSpread[3] + overSpread[3]) / r,
        c + underSpread[1] + overSpread[1] + underSpread[2] + overSpread[2] + underSpread[3] + overSpread[3] + underSpread[4] + overSpread[4], r, ((float)c + underSpread[1] + overSpread[1] + underSpread[2] + overSpread[2] + underSpread[3] + overSpread[3] + underSpread[4] + overSpread[4]) / r);

    printf("Gain %f/%f\n",actGainTotal, maxGainTotal);

    printf("     .005     |      .01      |      .05      |      .10      |      .15      |      .20      |      .25\n");
    printf("%6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f\n\n",
        actGain[1], maxGain[1],
        actGain[2], maxGain[2],
        actGain[3], maxGain[3],
        actGain[4], maxGain[4],
        actGain[5], maxGain[5],
        actGain[6], maxGain[6],
        actGain[7], maxGain[7]);
}

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

        //static int dimensions = 5;
        //dimensions++;
        // Experimentally, 7 dimensions seems to work well for the # of features I chose...
        Midas::Classifier c(training_data, training_labels, 7, 8);
        aapl_results = c.Classify(aapl_data);
        ibm_results = c.Classify(ibm_data);
        intc_results = c.Classify(intc_data);

        Analyze(aapl, aapl_labels, aapl_results);
        Analyze(ibm, ibm_labels, ibm_results);
        Analyze(intc, intc_labels, intc_results);

        system("pause");
    }
}
