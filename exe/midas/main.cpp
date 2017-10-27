#include <stdlib.h>
#include <stdio.h>
#include <mlpack/core.hpp>
#include <mlpack/methods/naive_bayes/naive_bayes_classifier.hpp>

using namespace mlpack;

void AnalyzeResults(std::string symbol, arma::Row<arma::u64> labels, arma::Row<arma::u64>& results)
{
    int correct = 0;
    int truetrade = 0;
    int falsetrade = 0;

    for (int i = 0; i < labels.size(); i++)
    {
        if (labels[i] == results[i])
        {
            correct++;
        }

        if (labels[i] && results[i])
        {
            truetrade++;
        }

        if (!labels[i] && results[i])
        {
            falsetrade++;
        }
    }

    printf("%s: %d/%d = %f tt: %d ft: %d\n", symbol.c_str(), correct, labels.size(), (float)correct / labels.size(), truetrade, falsetrade);
}

void SimulateTrading(std::string symbol, arma::mat& data, arma::Row<arma::u64>& results)
{
    double baseline = 0;
    double profit = 0;
    int trades = 0;
    int goodTrades = 0;
    int badTrades = 0;
    int goodBaseline = 0;
    int badBaseline = 0;

    for (int i = 1; i < data.n_rows - 1; i++)
    {
        if ((data.row(i)[0] - data.row(i - 1)[0] <= 60) && (data.row(i + 1)[0] - data.row(i)[0] <= 60))
        {
            double delta = data.row(i + 1)[2] - data.row(i)[4];
            baseline += delta;
            (delta > 0) ? goodBaseline++ : badBaseline++;

            if (results[i])
            {
                trades++;
                profit += delta;
                (delta > 0) ? goodTrades++ : badTrades++;
            }

            //printf("t: %f c: %f r: %d d: %f\n", data.row(i)[0], data.row(i)[4], results[i], delta);
        }
    }

    printf("%s: baseline: %f profit: %f trades: %d gt: %d bt: %d gb: %d bb: %d\n", symbol.c_str(), baseline, profit, trades, goodTrades, badTrades, goodBaseline, badBaseline);
}

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

        printf("training_data   %d %d\n", training_data.n_cols, training_data.n_rows);
        printf("testing_data    %d %d\n", testing_data.n_cols, testing_data.n_rows);
        printf("training_labels %d %d\n", training_labels.n_cols, training_labels.n_rows);
        printf("testing_labels  %d %d\n", testing_labels.n_cols, testing_labels.n_rows);
        printf("aapl_data %d %d\n", aapl_data.n_cols, aapl_data.n_rows);
        printf("ibm_data %d %d\n", ibm_data.n_cols, ibm_data.n_rows);
        printf("intc_data  %d %d\n", intc_data.n_cols, intc_data.n_rows);
        printf("aapl_labels %d %d\n", aapl_labels.n_cols, aapl_labels.n_rows);
        printf("ibm_labels %d %d\n", ibm_labels.n_cols, ibm_labels.n_rows);
        printf("intc_labels  %d %d\n", intc_labels.n_cols, intc_labels.n_rows);

        arma::mat aapl, ibm, intc;

        aapl.load("AAPL_raw.csv", arma::file_type::csv_ascii);
        ibm.load("IBM_raw.csv", arma::file_type::csv_ascii);
        intc.load("INTC_raw.csv", arma::file_type::csv_ascii);

        arma::u64_mat aapl_results, ibm_results, intc_results, testing_results;

        for (int i = 0; i < training_labels.n_rows; i++)
        {
            arma::Row<arma::u64> results;
            naive_bayes::NaiveBayesClassifier<> nbc(training_data, training_labels.row(i), 2, false);
            nbc.Classify(aapl_data, results);
            aapl_results.insert_rows(aapl_results.n_rows, results);
            results.clear();
            nbc.Classify(ibm_data, results);
            ibm_results.insert_rows(ibm_results.n_rows, results);
            results.clear();
            nbc.Classify(intc_data, results);
            intc_results.insert_rows(intc_results.n_rows, results);
            results.clear();
            nbc.Classify(testing_data, results);
            testing_results.insert_rows(testing_results.n_rows, results);
            results.clear();
        }

        Analyze(aapl, aapl_labels, aapl_results);
        Analyze(ibm, ibm_labels, ibm_results);
        Analyze(intc, intc_labels, intc_results);

        system("pause");
    }
}
