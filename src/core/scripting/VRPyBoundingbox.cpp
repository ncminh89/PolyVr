#include "VRPyBoundingbox.h"
#include "VRPyBaseT.h"
#include "VRPyBaseFactory.h"

namespace OSG {
    typedef Boundingbox VRBoundingbox;
}

using namespace OSG;

newPyType(Boundingbox, Boundingbox, New_ptr);

PyMethodDef VRPyBoundingbox::methods[] = {
    {"min", PyWrap(Boundingbox, min, "Get the minimum vector", Vec3f) },
    {"max", PyWrap(Boundingbox, max, "Get the maximum vector", Vec3f) },
    {"update", PyWrap(Boundingbox, update, "Update the bounding box", void, Vec3f) },
    {NULL}  /* Sentinel */
};


