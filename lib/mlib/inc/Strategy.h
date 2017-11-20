#pragma once

class Strategy
{
public:
    virtual bool DiscreteTrainingData() = 0;

    virtual uint32_t PercentTrainingData() = 0;

    virtual uint32_t Dimensions() = 0;
};

class DefaultStrategy : public Strategy
{
public:
    virtual bool DiscreteTrainingData()
    {
        return false;
    }

    virtual uint32_t PercentTrainingData()
    {
        return 100;
    }

    virtual uint32_t Dimensions()
    {
        return 5;
    }
};
