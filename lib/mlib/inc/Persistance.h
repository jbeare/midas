#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <iostream>
#include <fstream>
#include <string>

template<class T>
static void Load(_Inout_ T& object, _In_ std::string const& name)
{
    std::ifstream ifs(name, std::ios::binary);
    boost::archive::xml_iarchive ar(ifs);
    ar & object;
}

template<class T>
static void Save(_Inout_ T& object, _In_ std::string const& name)
{
    std::ofstream ofs(name, std::ios::binary);
    boost::archive::xml_oarchive ar(ofs);
    ar & object;
}
