#include <stdlib.h>
#include <stdio.h>
#include <mlpack/core.hpp>
#include <mlpack/methods/naive_bayes/naive_bayes_classifier.hpp>
#include <mlpack/methods/pca/pca.hpp>
#include "Classifier.h"

using namespace mlpack;

void Analyze(arma::mat& data, arma::u64_mat& labels, arma::u64_mat& results)
{
    if ((data.n_rows != labels.n_cols) || (results.n_cols != labels.n_cols) || (results.n_rows != labels.n_rows))
    {
        printf("Data error!\n");
        return;
    }

    //results.print();
    
    // Each loop is a different classification
    for (int i = 0; i < labels.n_rows; i++)
    {
        auto l = labels.row(i);
        auto r = results.row(i);

        int total = 0;
        int correct = 0;
        int totalTrades = 0;
        int correctTrades = 0;

        // Each loop is a different observation
        for (int j = 0; j < data.n_rows; j++)
        {
            auto d = data.row(j);

            if (l[j] == r[j])
            {
                correct++;
            }
            total++;

            if (r[j] == 1)
            {
                totalTrades++;
                if (l[j] == r[j])
                {
                    correctTrades++;
                }
            }
        }

        if (i == 0)
        {
            printf("\n");
        }

        printf("For label %2d we got %4d/%4d (%2.2f) %4d/%4d (%2.2f)\n", i, correct, total, (float)correct/total, correctTrades, totalTrades, (float)correctTrades/totalTrades);
    }

    float maxGain[8] = {};
    float actGain[8] = {};

    for (int i = 0; i < data.n_rows - 1; i++)
    {
        auto l = labels.col(i);
        auto r = results.col(i);
        auto d = data.row(i);

        if (l.n_rows != 7)
        {
            // We should have the number of labels + 1
            printf("Data error!");
        }

        for (int j = l.n_rows - 1; j >= 0; j--)
        {
            if (l[j])
            {
                maxGain[j]++;
                maxGain[7] += data.row(i + 1)[2] - data.row(i)[4];
                break;
            }
        }

        for (int j = l.n_rows - 1; j >= 0; j--)
        {
            if (r[j])
            {
                if (l[j])
                {
                    actGain[j]++;
                    actGain[7] += data.row(i + 1)[2] - data.row(i)[4];
                }
                else
                {
                    // TODO: Think about this some more; we're basically saying on failed classification we cash out at the end of the interval.
                    //actGain[j]--;
                    actGain[7] -= data.row(i + 1)[4] - data.row(i)[4];
                }
                break;
            }
        }
    }

    maxGain[0] = maxGain[0] * 0.005;
    maxGain[1] = maxGain[1] * 0.01;
    maxGain[2] = maxGain[2] * 0.05;
    maxGain[3] = maxGain[3] * 0.10;
    maxGain[4] = maxGain[4] * 0.15;
    maxGain[5] = maxGain[5] * 0.20;
    maxGain[6] = maxGain[6] * 0.50;
    float maxGainTotal = maxGain[0] + maxGain[1] + maxGain[2] + maxGain[3] + maxGain[4] + maxGain[5] + maxGain[6];

    actGain[0] = actGain[0] * 0.005;
    actGain[1] = actGain[1] * 0.01;
    actGain[2] = actGain[2] * 0.05;
    actGain[3] = actGain[3] * 0.10;
    actGain[4] = actGain[4] * 0.15;
    actGain[5] = actGain[5] * 0.20;
    actGain[6] = actGain[6] * 0.50;
    float actGainTotal = actGain[0] + actGain[1] + actGain[2] + actGain[3] + actGain[4] + actGain[5] + actGain[6];

    printf("     .005     |      .01      |      .05      |      .10      |      .15      |     Gain      |   Max Gain   \n");
    printf("%6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f | %6.2f %6.2f\n",
        actGain[0], maxGain[0],
        actGain[1], maxGain[1],
        actGain[2], maxGain[2],
        actGain[3], maxGain[3],
        actGain[4], maxGain[4],
        //actGain[5], maxGain[5],
        //actGain[6], maxGain[6],
        actGainTotal, maxGainTotal,
        actGain[7], maxGain[7]);
}

int main(int argc, char** argv)
{
    while (1)
    {
        arma::mat training_data, testing_data, aapl_data, ibm_data, intc_data;
        arma::u64_mat training_labels, testing_labels, aapl_labels, ibm_labels, intc_labels;
        
        aapl_data.load("AAPL_observations.csv", arma::file_type::csv_ascii);
        aapl_labels.load("AAPL_labels.csv", arma::file_type::csv_ascii);
        ibm_data.load("IBM_observations.csv", arma::file_type::csv_ascii);
        ibm_labels.load("IBM_labels.csv", arma::file_type::csv_ascii);
        intc_data.load("INTC_observations.csv", arma::file_type::csv_ascii);
        intc_labels.load("INTC_labels.csv", arma::file_type::csv_ascii);

        aapl_data = aapl_data.t();
        ibm_data = ibm_data.t();
        intc_data = intc_data.t();
        aapl_labels = aapl_labels.t();
        ibm_labels = ibm_labels.t();
        intc_labels = intc_labels.t();

        testing_data = aapl_data.cols(arma::span(aapl_data.n_cols - 800, aapl_data.n_cols - 1));
        testing_labels = aapl_labels.cols(arma::span(aapl_labels.n_cols - 800, aapl_labels.n_cols - 1));
        training_data = aapl_data.cols(arma::span(0, aapl_data.n_cols - 801));
        training_labels = aapl_labels.cols(arma::span(0, aapl_labels.n_cols - 801));

        arma::mat aapl, ibm, intc;

        aapl.load("AAPL_raw.csv", arma::file_type::csv_ascii);
        ibm.load("IBM_raw.csv", arma::file_type::csv_ascii);
        intc.load("INTC_raw.csv", arma::file_type::csv_ascii);

        arma::u64_mat aapl_results, ibm_results, intc_results, testing_results;

        //static int dimensions = 5;
        //dimensions++;
        for (int i = 0; i < training_labels.n_rows; i++)
        {
            // Experimentally, 7 dimensions seems to work well for the # of features I chose...
            Midas::Classifier c(training_data, training_labels.row(i), 7, 2);
            aapl_results.insert_rows(aapl_results.n_rows, c.Classify(aapl_data));
            ibm_results.insert_rows(ibm_results.n_rows, c.Classify(ibm_data));
            intc_results.insert_rows(intc_results.n_rows, c.Classify(intc_data));
            testing_results.insert_rows(testing_results.n_rows, c.Classify(testing_data));
        }

        Analyze(aapl, aapl_labels, aapl_results);
        Analyze(ibm, ibm_labels, ibm_results);
        Analyze(intc, intc_labels, intc_results);

        system("pause");
    }
}
