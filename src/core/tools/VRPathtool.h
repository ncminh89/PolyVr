#ifndef VRPATHTOOL_H_INCLUDED
#define VRPATHTOOL_H_INCLUDED

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGVector.h>
#include <vector>
#include <map>

#include "core/objects/VRObjectFwd.h"
#include "core/utils/VRFunctionFwd.h"
#include "core/tools/VRToolsFwd.h"
#include "core/objects/object/VRObject.h"
#include "core/math/graph.h"
#include "core/math/VRMathFwd.h"

using namespace std;
OSG_BEGIN_NAMESPACE

class VRDevice;

class VRManipulator {
    private:
        VRObjectPtr anchor;
        VRGeometryPtr gTX, gTY, gTZ;
        VRGeometryPtr gRX, gRY, gRZ;
        VRTransformPtr sel = 0;

        void setup();

    public:
        VRManipulator();

        void handle(VRGeometryPtr g);
        void manipulate(VRTransformPtr t);
};

class VRExtruder {
    private:
        void update();

    public:
        VRExtruder();

        // update fkt
        //  check if any device beacon changed
        //  intersect with invisible cones
        //  on intersection
        //   make cone visible

        //  check if any device drags a cone of ours
        //   trigger events -> start drag/ update drag/ stop drag
};

class VRPathtool : public VRObject {
    public:
        struct entry {
            int edge = 0;
            pathPtr p = 0;
            map<VRGeometry*, int> points;
            vector<VRGeometryWeakPtr> handles;
            VRStrokeWeakPtr line;
            VRObjectWeakPtr anchor;

            entry(VRObjectPtr anchor);
            void addHandle(VRGeometryPtr h, int i);
        };

        struct knot {
            vector<int> in;
            vector<int> out;
            VRGeometryWeakPtr handle;
        };

        struct option {
            int resolution = 10;
            bool useControlHandles = false;

            option(int r = 10, bool uch = false);
        };

        typedef shared_ptr<entry> entryPtr;

    private:
        GraphPtr graph;
        map<int, pathPtr> paths;
        map<int, option > options;
        vector<VRGeometryWeakPtr> handles;
        vector<VRGeometryWeakPtr> controlhandles;

        map<path*, entryPtr> pathToEntry;
        map<VRGeometry*, vector<entryPtr> > handleToEntries; // map handle geometries to the entries
        map<VRGeometry*, int> handleToNode;
        map<int, knot> knots; // maps graph node ids to pathtool knots
        pathPtr selectedPath = 0;

        VRMaterialPtr lmat;
        VRMaterialPtr lsmat;

        VRUpdateCbPtr updatePtr;
        VRManipulator* manip = 0;
        VRExtruder* ext = 0;
        VRObjectPtr projObj;

        VRGeometryPtr customHandle;
        VRGeometryPtr newControlHandle(VRGeometryPtr handle, Vec3f n);
        VRGeometryPtr newHandle();
        entryPtr newEntry(pathPtr p, option o, int eID, VRObjectPtr anchor = 0);
        void setupHandles(entryPtr p, VRGeometryPtr ha, VRGeometryPtr he);
        void updateHandle(VRGeometryPtr handle);
        void updateEntry(entryPtr e);
        void updateDevs();

        VRGeometryPtr setGraphNode(int i);
        void setGraphEdge(Graph::edge& e, bool handles = false);
        void setGraphEdge(Graph::edge& e, bool handles, Vec3f n1, Vec3f n2);
        void projectHandle(VRGeometryPtr handle, VRDevicePtr dev);

    public:
        VRPathtool();
        static VRPathtoolPtr create();
        void setup();
        void setupBefore();

        void setProjectionGeometry(VRObjectPtr obj);

        void setGraph(GraphPtr g, bool doClear = true);
        int addNode(posePtr p);
        void remNode(int i);
        int getNodeID(VRObjectPtr o);
        void connect(int i1, int i2, bool handles = true);
        void connect(int i1, int i2, bool handles, Vec3f n1, Vec3f n2);
        void disconnect(int i1, int i2);

        pathPtr newPath(VRDevicePtr dev, VRObjectPtr anchor, int resolution = 10, bool doCHandles = false);
        VRGeometryPtr extrude(VRDevicePtr dev, pathPtr p);
        void remPath(pathPtr p);

        void addPath(pathPtr p, VRObjectPtr anchor = 0, VRGeometryPtr ha = 0, VRGeometryPtr he = 0, bool handles = true);
        void setVisuals(bool handles, bool lines);
        void setHandleGeometry(VRGeometryPtr geo);
        void clear(pathPtr p = 0);

        vector<pathPtr> getPaths(VRGeometryPtr h = 0);
        pathPtr getPath(VRGeometryPtr h1, VRGeometryPtr h2);
        VRGeometryPtr getHandle(int ID);
        vector<VRGeometryPtr> getHandles(pathPtr p);
        VRStrokePtr getStroke(pathPtr p);

        VRMaterialPtr getPathMaterial();

        void select(VRGeometryPtr handle);
        void select(pathPtr p);
        void deselect();
        void update();
};

OSG_END_NAMESPACE

#endif // VRPATHTOOL_H_INCLUDED
