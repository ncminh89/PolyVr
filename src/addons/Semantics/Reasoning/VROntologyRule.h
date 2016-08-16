#ifndef VRONTOLOGYRULE_H_INCLUDED
#define VRONTOLOGYRULE_H_INCLUDED

#include "VROntologyUtils.h"
#include "addons/Semantics/VRSemanticsFwd.h"
#include "core/utils/VRName.h"

using namespace std;
namespace OSG {

struct VROntologyRule : public VROntoID, public VRName {
    string rule;
    VROntologyRule(string rule);

    static VROntologyRulePtr create(string rule = "");
    string toString();
};

}

#endif
