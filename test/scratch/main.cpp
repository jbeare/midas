#include <Persistance.h>

struct I1
{
    I1() {}
    virtual ~I1() {}

    template<class Archive>
    void serialize(Archive &, const unsigned int)
    {
    }
};

BOOST_CLASS_EXPORT(I1)

struct C1 : I1
{
    virtual ~C1() {}

    template<class Archive>
    void serialize(Archive &ar, const unsigned int)
    {
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(I1);
    }
};

BOOST_CLASS_EXPORT(C1)

struct I2
{
    virtual ~I2() {}

    template<class Archive>
    void serialize(Archive &ar, const unsigned int)
    {
        ar & BOOST_SERIALIZATION_NVP(p);
    }

    std::shared_ptr<I1> p;
};

BOOST_CLASS_EXPORT(I2)

struct C2 : I2
{
    C2() { p = std::make_shared<C1>(); }
    virtual ~C2() {}

    template<class Archive>
    void serialize(Archive &ar, const unsigned int)
    {
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(I2);
    }
};

BOOST_CLASS_EXPORT(C2)

int main()
{
    I1 i1;

    Persistance::Save(i1, "blah");
    Persistance::Load(i1, "blah");

    C1 c1;

    Persistance::Save(c1, "blah");
    Persistance::Load(c1, "blah");

    I2 i2;

    Persistance::Save(i2, "blah");
    Persistance::Load(i2, "blah");

    C2 c2;

    Persistance::Save(c2, "blah");
    Persistance::Load(c2, "blah");
}
