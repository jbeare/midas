#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

#include <iostream>
#include <fstream>
#include <string>

namespace Persistance
{
    template<class T>
    static void Load(_Out_ T& object, _In_ std::string const& name)
    {
        std::ifstream ifs(name, std::ios::binary);
        boost::archive::xml_iarchive ar(ifs);
        ar & boost::serialization::make_nvp("Persistance_object", object);
    }

    template<class T>
    static void Save(_In_ T const& object, _In_ std::string const& name)
    {
        std::ofstream ofs(name, std::ios::binary);
        boost::archive::xml_oarchive ar(ofs);
        ar & boost::serialization::make_nvp("Persistance_object", object);
    }
}
