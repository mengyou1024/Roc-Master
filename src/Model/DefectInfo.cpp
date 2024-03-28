#include "./DefectInfo.h"
#include <rttr/registration.h>

using namespace rttr;
using namespace ORM_Model;

RTTR_REGISTRATION {
    registration::class_<DefectInfo>("DefectInfo")
        .constructor<>()
        .property("id", &DefectInfo::id)
        .property("channel", &DefectInfo::channel)
        .property("location", &DefectInfo::location)
        .property("length", &DefectInfo::length)
        .property("depth", &DefectInfo::depth)
        .property("maxAmp", &DefectInfo::maxAmp);
}
