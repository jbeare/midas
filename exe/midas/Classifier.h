#pragma once

#include <memory>
#include <mlpack/core.hpp>
#include <mlpack/methods/naive_bayes/naive_bayes_classifier.hpp>
#include <mlpack/methods/pca/pca.hpp>
#include <sal.h>
#include <string>

#define THROW_IF_FAILED_BOOL(cond) {if(!cond){throw std::exception();}}

namespace Midas
{

class Pca : public std::enable_shared_from_this<Pca>
{
public:
    arma::mat Transform(
        _In_ const arma::mat& data,
        _In_ size_t maxDimensions)
    {
        if (maxDimensions == 0)
        {
            return data;
        }

        arma::vec rowMean = arma::sum(data, 1) / (uint32_t)data.n_cols;
        arma::mat centeredData = data - arma::repmat(rowMean, 1, data.n_cols);

        if (m_eigenVectors.is_empty())
        {
            arma::vec eigenValues;
            arma::mat rightSingularValues;

            if (data.n_rows < data.n_cols)
            {
                THROW_IF_FAILED_BOOL(arma::svd_econ(m_eigenVectors, eigenValues, rightSingularValues, centeredData, 'l'));
            }
            else
            {
                THROW_IF_FAILED_BOOL(arma::svd(m_eigenVectors, eigenValues, rightSingularValues, centeredData));
            }
        }

        if (m_eigenVectors.n_cols > maxDimensions)
        {
            m_eigenVectors.reshape(m_eigenVectors.n_rows, maxDimensions);
        }

        return (arma::trans(m_eigenVectors) * centeredData);
    }

private:
    arma::mat m_eigenVectors;
};

class Classifier : public std::enable_shared_from_this<Classifier>
{
public:
    Classifier(
        _In_ const arma::mat& data,
        _In_ const arma::Row<size_t>& labels,
        _In_ const size_t dimensions,
        _In_ const size_t classes) :
        m_dimensions(dimensions),
        m_pca(),
        m_nbc(m_pca.Transform(data, m_dimensions), labels, classes)
    {

    }

    Classifier(_In_ std::string name)
    {
        // Not implemented.
        throw std::exception();
    }

    arma::Row<size_t> Classify(_In_ const arma::mat& data)
    {
        arma::Row<size_t> results;
        m_nbc.Classify(m_pca.Transform(data, m_dimensions), results);
        return std::move(results);
    }
        
    void Store(_In_ std::string name)
    {
        // Not implemented.
        throw std::exception();
    }

private:
    size_t m_dimensions;
    Pca m_pca;
    mlpack::naive_bayes::NaiveBayesClassifier<> m_nbc;
    
};

} // namespace Midas