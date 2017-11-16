#define NOMINMAX

#include <memory>
#include <sal.h>
#include <string>
#include <MLib.h>

#pragma warning(disable : 4348 4267 4244 4458 6285 6287 6011 28251)
#include <mlpack/core.hpp>
#include <mlpack/methods/naive_bayes/naive_bayes_classifier.hpp>
#include <mlpack/methods/pca/pca.hpp>
#pragma warning(default : 4348 4267 4244 4458 6285 6287 6011 28251)

#include "Classifier.h"

class Pca
{
public:
    // Each Row is a feature.
    // Each Column is a datapoint.
    arma::mat Transform(_In_ const arma::mat& data, _In_ uint32_t maxDimensions)
    {
        if (maxDimensions == 0)
        {
            return data;
        }

        arma::mat centeredData;

        if (m_eigenVectors.is_empty())
        {
            m_mean = arma::sum(data, 1) / (uint32_t)data.n_cols;
            centeredData = data - arma::repmat(m_mean, 1, data.n_cols);

            arma::vec eigenValues;
            arma::mat rightSingularValues;

            if (data.n_rows < data.n_cols)
            {
                THROW_HR_IF_FAILED_BOOL(arma::svd_econ(m_eigenVectors, eigenValues, rightSingularValues, centeredData, 'l'));
            }
            else
            {
                THROW_HR_IF_FAILED_BOOL(arma::svd(m_eigenVectors, eigenValues, rightSingularValues, centeredData));
            }
        }
        else
        {
            centeredData = data - arma::repmat(m_mean, 1, data.n_cols);
        }

        if (m_eigenVectors.n_cols > maxDimensions)
        {
            m_eigenVectors.reshape(m_eigenVectors.n_rows, maxDimensions);
        }

        return (arma::trans(m_eigenVectors) * centeredData);
    }

    template<typename Archive>
    void Serialize(Archive& ar)
    {
        ar & mlpack::data::CreateNVP(m_eigenVectors, "Pca_m_eigenVectors");
        ar & mlpack::data::CreateNVP(m_mean, "Pca_m_mean");
    }

private:
    arma::mat m_eigenVectors;
    arma::vec m_mean;
};

class ClassifierImpl : public Classifier
{
public:
    ClassifierImpl(_In_ uint32_t classes, _In_ uint32_t dimensions) :
        m_classes{classes},
        m_dimensions{dimensions},
        m_nbc{m_dimensions, m_classes}
    {

    };

    ClassifierImpl(_In_ std::string name)
    {
        Load(name);
    }

#pragma region Classifier
    virtual void Train(_In_ std::string const& trainingDataPath, _In_ std::string const& trainingLabelsPath, _In_ bool updatePca)
    {
        arma::Mat<double> data;
        arma::Row<size_t> labels;

        // The data is expected to be in row=datapoint column=feature format.
        THROW_HR_IF_FAILED_BOOL(data.load(trainingDataPath, arma::file_type::csv_ascii));

        // The labels are expected to be in column=datapoint format.
        THROW_HR_IF_FAILED_BOOL(labels.load(trainingLabelsPath, arma::file_type::csv_ascii));

        data = data.t();

        if (m_features == 0)
        {
            MLIB_ASSERT((data.n_rows <= UINT32_MAX), E_INVALIDARG);
            m_features = static_cast<uint32_t>(data.n_rows);
            MLIB_ASSERT((m_features >= m_dimensions), E_INVALIDARG);
        }

        MLIB_ASSERT((data.n_rows == m_features), E_INVALIDARG);
        MLIB_ASSERT((data.n_cols > 0), E_INVALIDARG);
        MLIB_ASSERT((data.n_cols == labels.n_cols), E_INVALIDARG);

        if (updatePca)
        {
            m_pca = std::make_unique<Pca>();
        }

        m_nbc.Train(m_pca->Transform(data, m_dimensions), labels);
    }

    virtual uint32_t Classify(_In_ std::vector<double> const& data)
    {
        MLIB_ASSERT((data.size() == m_features), E_INVALIDARG);

        arma::Row<size_t> results;
        m_nbc.Classify(m_pca->Transform(data, m_dimensions), results);

        MLIB_ASSERT((results.size() == 1), E_UNEXPECTED);
        MLIB_ASSERT((results[0] <= UINT32_MAX), E_UNEXPECTED);

        return static_cast<uint32_t>(results[0]);
    }

    virtual void Load(_In_ std::string const& name)
    {
        std::ifstream ifs(name, std::ios::binary);
        boost::archive::xml_iarchive ar(ifs);
        Serialize<boost::archive::xml_iarchive>(ar);
    }

    virtual void Store(_In_ std::string const& name)
    {
        std::ofstream ofs(name, std::ios::binary);
        boost::archive::xml_oarchive ar(ofs);
        Serialize<boost::archive::xml_oarchive>(ar);
    }
#pragma endregion

    template<typename Archive>
    void Serialize(Archive& ar)
    {
        ar & mlpack::data::CreateNVP(m_classes, "ClassifierImpl_m_classes");
        ar & mlpack::data::CreateNVP(m_dimensions, "ClassifierImpl_m_dimensions");
        ar & mlpack::data::CreateNVP(m_features, "ClassifierImpl_m_features");
        m_pca->Serialize<Archive>(ar);
        m_nbc.Serialize<Archive>(ar, 0);
    }

private:
    uint32_t m_classes{};
    uint32_t m_dimensions{};
    uint32_t m_features{};
    std::unique_ptr<Pca> m_pca{std::make_unique<Pca>()};
    mlpack::naive_bayes::NaiveBayesClassifier<> m_nbc;
};

_Use_decl_annotations_
std::shared_ptr<Classifier> Classifier::MakeShared(uint32_t classes, uint32_t dimensions)
{
    return std::make_shared<ClassifierImpl>(classes, dimensions);
}

_Use_decl_annotations_
std::shared_ptr<Classifier> Classifier::MakeShared(std::string const& name)
{
    return std::make_shared<ClassifierImpl>(name);
}
