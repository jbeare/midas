#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

template <typename T>
class RowRef;

template <typename T>
class ColRef;

//  1   2   3
//  4   5   6
//  7   8   9
//
//  1   2   3   4   5   6   7   8   9

template <typename T>
class SimpleMatrix
{
public:
    //SimpleMatrix();

    //SimpleMatrix(uint32_t rows, uint32_t cols);

    SimpleMatrix(std::vector<T> data, uint32_t rows, uint32_t cols) :
        m_matrix(data), m_rows(rows), m_cols(cols)
    {

    }

    SimpleMatrix(std::string name)
    {
        std::ifstream ifs(name);
        std::string line;

        while (std::getline(ifs, line))
        {
            m_rows++;
            std::stringstream stream(line);
            std::string cell;

            while (std::getline(stream, cell, ','))
            {
                if (m_rows == 1)
                {
                    m_cols++;
                }

                m_matrix.push_back(Convert<T>(cell));
            }
        }
    }

    RowRef<T> Row(uint32_t index) const
    {
        return RowRef<T>(this, index);
    }

    ColRef<T> Col(uint32_t index) const
    {
        return ColRef<T>(this, index);
    }

    uint32_t NumRows() const
    {
        return m_rows;
    }

    uint32_t NumCols() const
    {
        return m_cols;
    }

    std::vector<T> const& GetVector() const
    {
        return m_matrix;
    }

private:
    template <typename T>
    T Convert(std::string const& str);

    template <>
    double Convert<double>(std::string const& str)
    {
        return std::stod(str);
    }

    template <>
    uint32_t Convert<uint32_t>(std::string const& str)
    {
        return std::stoul(str);
    }

    template <>
    std::string Convert<std::string>(std::string const& str)
    {
        return str;
    }

    std::vector<T> m_matrix;
    uint32_t m_rows{};
    uint32_t m_cols{};
};

template <typename T>
class RowRef
{
public:
    RowRef(SimpleMatrix<T> const* matrix, uint32_t row) :
        m_matrix(matrix), m_row(row)
    {

    }

    T const& operator[] (int const index) const
    {
        return m_matrix->GetVector()[(m_row * m_matrix->NumCols()) + index];
    }

    std::vector<T> GetVector() const
    {
        auto first = m_matrix->GetVector().begin() + (m_row * m_matrix->NumCols());
        auto last = m_matrix->GetVector().begin() + ((m_row + 1) * m_matrix->NumCols());
        return std::vector<T>(first, last);
    }

private:
    SimpleMatrix<T> const* m_matrix;
    uint32_t m_row;
};

template <typename T>
class ColRef
{
public:
    ColRef(SimpleMatrix<T> const* matrix, uint32_t col) :
        m_matrix(matrix), m_col(col)
    {

    }

    T const& operator[] (int const index) const
    {
        return m_matrix->GetVector()[(index * m_matrix->NumCols()) + m_col];
    }

    std::vector<T> GetVector() const
    {
        std::vector<T> v;

        for (uint32_t i = 0; i < m_matrix->NumRows(); i++)
        {
            v.push_back(m_matrix->GetVector()[(i * m_matrix->NumCols()) + m_col]);
        }

        return v;
    }

private:
    SimpleMatrix<T> const* m_matrix;
    uint32_t m_col;
};
