#ifndef VRCALLBACKWRAPPER_H_INCLUDED
#define VRCALLBACKWRAPPER_H_INCLUDED

#include "VRUtilsFwd.h"
#include "toString.h"

OSG_BEGIN_NAMESPACE;

struct VRCallbackWrapperBase {
    static string err;
};

template<typename Param>
struct VRCallbackWrapper : VRCallbackWrapperBase {
    VRCallbackWrapper() {}
    virtual ~VRCallbackWrapper() {}

    template<typename T>
    Param convert(const T& t);

    virtual bool execute(void* obj, const vector<Param>& params, Param& result) = 0;
};

OSG_END_NAMESPACE;

#endif // VRCALLBACKWRAPPER_H_INCLUDED
