#include "./DefectInfo.h"
#include <rttr/registration.h>

using namespace rttr;
using namespace ORM_Model;

RTTR_REGISTRATION {
    registration::class_<DefectInfo>("DefectInfo")
        .constructor<>()
        .property("id", &DefectInfo::id)
        .property("product", &DefectInfo::product)
        .property("description", &DefectInfo::description)
        .property("flawno", &DefectInfo::flawno)
        .property("dac", &DefectInfo::dac)
        .property("type", &DefectInfo::type)
        .property("location", &DefectInfo::location)
        .property("length", &DefectInfo::length)
        .property("depth", &DefectInfo::depth)
        .property("width", &DefectInfo::width)
        .property("result", &DefectInfo::result);
}
