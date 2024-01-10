#include "./DetectInfo.h"
#include <rttr/registration.h>

using namespace rttr;
using namespace ORM_Model;

RTTR_REGISTRATION {
    registration::class_<DetectInfo>("DetectInfo")
        .constructor<>()
        .property("customer", &DetectInfo::customer)
        .property("customerContractNumber", &DetectInfo::customerContractNumber)
        .property("workOrder", &DetectInfo::workOrder)
        .property("reportNumber", &DetectInfo::reportNumber)
        .property("description", &DetectInfo::description)
        .property("materialStandards", &DetectInfo::materialStandards)
        .property("surfaceStates", &DetectInfo::surfaceStates)
        .property("surveyedArea", &DetectInfo::surveyedArea)
        .property("thickness", &DetectInfo::thickness)
        .property("unitType", &DetectInfo::unitType)
        .property("probeType", &DetectInfo::probeType)
        .property("referenceBlock", &DetectInfo::referenceBlock)
        .property("waveform", &DetectInfo::waveform)
        .property("scanningSensitivity", &DetectInfo::scanningSensitivity)
        .property("couplingAgent", &DetectInfo::couplingAgent)
        .property("executiveStandard", &DetectInfo::executiveStandard)
        .property("acceptanceStandard", &DetectInfo::acceptanceStandard)
        .property("detectRatio", &DetectInfo::detectRatio)
        .property("detectProcessNumber", &DetectInfo::detectProcessNumber);
}
