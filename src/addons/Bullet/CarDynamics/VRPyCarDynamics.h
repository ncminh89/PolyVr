#ifndef VRPYCARDYNAMICS_H_INCLUDED
#define VRPYCARDYNAMICS_H_INCLUDED

#include "core/scripting/VRPyBase.h"
#include "core/objects/object/VRObject.h"
#include "CarDynamics.h"

struct VRPyCarDynamics : VRPyBaseT<OSG::CarDynamics> {
    static PyMethodDef methods[];

    static PyObject* update(VRPyCarDynamics* self, PyObject* args);
    static PyObject* setChassis(VRPyCarDynamics* self, PyObject* args);
    static PyObject* setWheel(VRPyCarDynamics* self, PyObject* args);
    static PyObject* setWheelOffsets(VRPyCarDynamics* self, PyObject* args);
    static PyObject* setWheelParams(VRPyCarDynamics* self, PyObject* args);
    static PyObject* setCarMass(VRPyCarDynamics* self, PyObject* args);
    static PyObject* reset(VRPyCarDynamics* self, PyObject* args);
    static PyObject* getSpeed(VRPyCarDynamics* self);
    static PyObject* getAcceleration(VRPyCarDynamics* self);
    static PyObject* getRoot(VRPyCarDynamics* self);
    static PyObject* getChassis(VRPyCarDynamics* self);
    static PyObject* getWheels(VRPyCarDynamics* self);
    static PyObject* followPath(VRPyCarDynamics* self, PyObject* args);
    static PyObject* stopPilot(VRPyCarDynamics* self);
};

#endif // VRPYCARDYNAMICS_H_INCLUDED
