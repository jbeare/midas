#pragma once

class DataSource
{
public:

};

class TwsDataSource : public DataSource
{
public:

};

class SimulatedDataSource : public DataSource
{
public:

};

class GoogleDataSource : public SimulatedDataSource
{
public:

};

class QuantQuoteDataSource : public SimulatedDataSource
{
public:

};
