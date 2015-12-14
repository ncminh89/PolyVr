#ifndef VRPYSETUP_H_INCLUDED
#define VRPYSETUP_H_INCLUDED

#include "VRPyBase.h"
#include "core/setup/VRSetup.h"
#include "core/setup/windows/VRView.h"

struct VRPySetup : VRPyBaseT<OSG::VRSetup> {
    static PyMethodDef methods[];
    static PyObject* getView(VRPySetup* self, PyObject* args);
};

struct VRPyView : VRPyBaseT<OSG::VRView> {
    static PyMethodDef methods[];
    static PyObject* toggleStereo(VRPyView* self);
    static PyObject* setPose(VRPyView* self, PyObject* args);
};

#endif // VRPYSETUP_H_INCLUDED
