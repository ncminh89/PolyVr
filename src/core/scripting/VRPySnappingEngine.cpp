#include "VRPySnappingEngine.h"
#include "VRPyTransform.h"
#include "VRPyBaseT.h"

using namespace OSG;

simpleVRPyType(SnappingEngine, New)

PyMethodDef VRPySnappingEngine::methods[] = {
    {"addObject", (PyCFunction)VRPySnappingEngine::addObject, METH_VARARGS, "Add an object to be checked for snapping - addObject(obj)" },
    {"remObject", (PyCFunction)VRPySnappingEngine::remObject, METH_VARARGS, "Remove an object - remObject(obj)" },
    {"addTree", (PyCFunction)VRPySnappingEngine::addTree, METH_VARARGS, "Add all subtree objects to be checked for snapping - addTree(obj)" },
    {"setPreset", (PyCFunction)VRPySnappingEngine::setPreset, METH_VARARGS, "Initiate the engine with a preset - setPreset(str preset)\n   preset can be: 'snap back', 'simple alignment'" },
    {"addRule", (PyCFunction)VRPySnappingEngine::addRule, METH_VARARGS, "Add snapping rule - int addRule(str translation, str orientation, "
                                                                        "prim_t[x,y,z,x0,y0,z0], prim_o[x,y,z,x0,y0,z0], float dist, float weight, obj local)"
                                                                        "\n\ttranslation/oriantation: 'NONE', 'POINT', 'LINE', 'PLANE', 'POINT_LOCAL', 'LINE_LOCAL', 'PLANE_LOCAL'"
                                                                        "\n\texample: addRule('POINT', 'POINT', [0,0,0,0,0,0], [0,1,0,0,0,-1], R, 1, None)"
                                                                         },
    {"remRule", (PyCFunction)VRPySnappingEngine::remRule, METH_VARARGS, "Remove a rule - remRule(int ID)" },
    {"addObjectAnchor", (PyCFunction)VRPySnappingEngine::addObjectAnchor, METH_VARARGS, "Remove a rule - addObjectAnchor(obj transform, obj anchor)" },
    {"clearObjectAnchors", (PyCFunction)VRPySnappingEngine::clearObjectAnchors, METH_VARARGS, "Remove a rule - clearObjectAnchors(obj transform)" },
    {"remLocalRules", (PyCFunction)VRPySnappingEngine::remLocalRules, METH_VARARGS, "Remove all object relative rules - clearObjectAnchors(obj transform)" },
    {NULL}  /* Sentinel */
};

PyObject* VRPySnappingEngine::remLocalRules(VRPySnappingEngine* self, PyObject* args) {
    if (self->obj == 0) { PyErr_SetString(err, "VRPySnappingEngine::remLocalRules - Object is invalid"); return NULL; }
    VRPyTransform *o;
    if (! PyArg_ParseTuple(args, "O", &o)) return NULL;
    self->obj->remLocalRules( o->objPtr );
    Py_RETURN_TRUE;
}

PyObject* VRPySnappingEngine::addObjectAnchor(VRPySnappingEngine* self, PyObject* args) {
    if (self->obj == 0) { PyErr_SetString(err, "VRPySnappingEngine::addObjectAnchor - Object is invalid"); return NULL; }
    VRPyTransform *o, *a;
    if (! PyArg_ParseTuple(args, "OO", &o, &a)) return NULL;
    self->obj->addObjectAnchor( o->objPtr, a->objPtr );
    Py_RETURN_TRUE;
}

PyObject* VRPySnappingEngine::clearObjectAnchors(VRPySnappingEngine* self, PyObject* args) {
    if (self->obj == 0) { PyErr_SetString(err, "VRPySnappingEngine::clearObjectAnchors - Object is invalid"); return NULL; }
    VRPyTransform *o;
    if (! PyArg_ParseTuple(args, "O", &o )) return NULL;
    self->obj->clearObjectAnchors( o->objPtr );
    Py_RETURN_TRUE;
}

PyObject* VRPySnappingEngine::remRule(VRPySnappingEngine* self, PyObject* args) {
    if (self->obj == 0) { PyErr_SetString(err, "VRPySnappingEngine::remRule - Object is invalid"); return NULL; }
    self->obj->remRule( parseInt(args) );
    Py_RETURN_TRUE;
}

PyObject* VRPySnappingEngine::addRule(VRPySnappingEngine* self, PyObject* args) {
    if (self->obj == 0) { PyErr_SetString(err, "VRPySnappingEngine::addRule - Object is invalid"); return NULL; }
    PyObject *t, *o; // string
    PyObject *pt, *po; // Vec4f
    float d, w;
    VRPyTransform* l;
    if (! PyArg_ParseTuple(args, "OOOOffO", &t, &o, &pt, &po, &d, &w, &l)) return NULL;
    OSG::VRTransformPtr trans = isNone((PyObject*)l) ? 0 : l->objPtr;
    auto _t = self->obj->typeFromStr( PyString_AsString(t) );
    auto _o = self->obj->typeFromStr( PyString_AsString(o) );
    int r = self->obj->addRule(_t, _o, PyToLine(pt), PyToLine(po), d, w, trans);
    return PyInt_FromLong(r);
}

PyObject* VRPySnappingEngine::remObject(VRPySnappingEngine* self, PyObject* args) {
    if (self->obj == 0) { PyErr_SetString(err, "VRPySnappingEngine::remObject - Object is invalid"); return NULL; }
    VRPyTransform* obj = 0;
    if (! PyArg_ParseTuple(args, "O", &obj)) return NULL;

    if (obj->obj) self->obj->remObject(obj->objPtr);
    Py_RETURN_TRUE;
}

PyObject* VRPySnappingEngine::addTree(VRPySnappingEngine* self, PyObject* args) {
    if (self->obj == 0) { PyErr_SetString(err, "VRPySnappingEngine::addTree - Object is invalid"); return NULL; }
    VRPyObject* obj = 0;
    if (! PyArg_ParseTuple(args, "O", &obj)) return NULL;

    if (obj->obj) self->obj->addTree(obj->objPtr);
    Py_RETURN_TRUE;
}

PyObject* VRPySnappingEngine::addObject(VRPySnappingEngine* self, PyObject* args) {
    if (self->obj == 0) { PyErr_SetString(err, "VRPySnappingEngine::addObject - Object is invalid"); return NULL; }
    VRPyTransform* obj = 0;
    if (! PyArg_ParseTuple(args, "O", &obj)) return NULL;

    if (obj->obj) self->obj->addObject(obj->objPtr);
    Py_RETURN_TRUE;
}

PyObject* VRPySnappingEngine::setPreset(VRPySnappingEngine* self, PyObject* args) {
    if (self->obj == 0) { PyErr_SetString(err, "VRPySnappingEngine::setPreset - Object is invalid"); return NULL; }
    string ps = parseString(args);
    if (ps == "simple alignment") self->obj->setPreset(OSG::VRSnappingEngine::SIMPLE_ALIGNMENT);
    if (ps == "snap back") self->obj->setPreset(OSG::VRSnappingEngine::SNAP_BACK);
    Py_RETURN_TRUE;
}

