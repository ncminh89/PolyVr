#ifndef VRPYMOLECULE_H_INCLUDED
#define VRPYMOLECULE_H_INCLUDED

#include "core/scripting/VRPyObject.h"
#include "VRMolecule.h"

struct VRPyMolecule : VRPyBaseT<OSG::VRMolecule> {
    static PyMethodDef methods[];

    static PyObject* set(VRPyMolecule* self, PyObject* args);
    static PyObject* setRandom(VRPyMolecule* self, PyObject* args);
};

#endif // VRPYMOLECULE_H_INCLUDED