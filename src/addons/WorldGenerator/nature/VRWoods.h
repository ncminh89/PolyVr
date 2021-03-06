#ifndef VRWOODS_H_INCLUDED
#define VRWOODS_H_INCLUDED

#include "core/objects/VRLod.h"
#include "core/objects/VRTransform.h"
#include "core/scene/VRObjectManager.h"
#include "core/math/VRMathFwd.h"
#include "addons/RealWorld/VRRealWorldFwd.h"

OSG_BEGIN_NAMESPACE;
using namespace std;

class VRLodLeaf : public VRTransform {
    private:
        Octree* oLeaf = 0;
        int lvl = 0;
        VRLodPtr lod;
        vector<VRObjectPtr> levels;

    public:
        VRLodLeaf(string name, Octree* o, int lvl);
        ~VRLodLeaf();
        static VRLodLeafPtr create(string name, Octree* o, int lvl);
        VRLodLeafPtr ptr();

        void addLevel(float dist);
        void add(VRObjectPtr obj, int lvl);
        void set(VRObjectPtr obj, int lvl);

        Octree* getOLeaf();
        int getLevel();
};

class VRLodTree : public VRObject {
    protected:
        OctreePtr octree;
        VRLodLeafPtr rootLeaf;
        map<Octree*, VRLodLeafPtr> leafs;
        map<int, vector<VRTransformPtr> > objects;

        VRLodLeafPtr addLeaf(Octree* o, int lvl);

    public:
        VRLodTree(string name, float size = 10);
        ~VRLodTree();
        static VRLodTreePtr create(string name);
        VRLodTreePtr ptr();

        VRLodLeafPtr addObject(VRTransformPtr obj, Vec3f p, int lvl);
        VRLodLeafPtr remObject(VRTransformPtr obj);
        void reset(float size = 0);

        vector<VRLodLeafPtr> getSubTree(VRLodLeafPtr l);
};

class VRWoods : public VRLodTree {
    private:
        map<int, VRTreePtr> treesByID;
        map<string, VRTreePtr> treeTemplates;
        map<string, shared_ptr<VRObjectManager::Entry> > treeEntries;
        map<VRTree*, VRTreePtr> treeRefs;
        VRGeometryPtr collisionMesh;

        map<VRGrassPatch*, VRGrassPatchPtr> grassPatchRefs;

        VRMaterialPtr truncMat;
        VRMaterialPtr leafMat1;
        VRMaterialPtr leafMat2;
        VRMaterialPtr grassMat;

        void computeFirstLevel();
        void computeSecondLevel();

        void setup();
        void initLOD();

    public:
        VRWoods(string name);
        ~VRWoods();

        static VRWoodsPtr create(string name = "woods");
        VRWoodsPtr ptr();

        void clear();
        VRTreePtr addTree(VRTreePtr t, bool updateLODs = 0, bool addToStore = true);
        VRTreePtr getTree(int id);
        VRTransformPtr addGrassPatch(PolygonPtr area, bool updateLODs = 0);
        void remTree(int id);
        void computeLODs();
        void computeLODs(VRLodLeafPtr leaf);
        void computeLODs(map<Octree*, VRLodLeafPtr>& leafs);

        void addCollisionModels();
};

OSG_END_NAMESPACE;

#endif // VRWOODS_H_INCLUDED
