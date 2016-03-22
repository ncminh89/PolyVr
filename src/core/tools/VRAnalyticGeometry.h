#ifndef VRANALYTICGEOMETRY_H_INCLUDED
#define VRANALYTICGEOMETRY_H_INCLUDED

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGVector.h>
#include <string>

#include "core/objects/VRObjectFwd.h"
#include "core/objects/object/VRObject.h"

using namespace std;
OSG_BEGIN_NAMESPACE;

class VRAnalyticGeometry : public VRObject {
    private:
        VRAnnotationEnginePtr ae = 0;
        VRGeometryPtr vectorLinesGeometry = 0;
        VRGeometryPtr vectorEndsGeometry = 0;
        VRGeometryPtr circlesGeometry = 0;

        VRMaterialPtr vecMat = 0;
        VRMaterialPtr pntMat = 0;
        VRMaterialPtr cirMat = 0;

        void resize(int i, int j = 0, int k = 0);

        static string circle_vp;
        static string circle_fp;

    protected:
        void init();

    public:
        VRAnalyticGeometry();
        ~VRAnalyticGeometry();

        static VRAnalyticGeometryPtr create();
        VRAnalyticGeometryPtr ptr();

        void setLabelParams(float size, bool screen_size = false, bool billboard = false);

        void setVector(int i, Vec3f pos, Vec3f vec, Vec3f color, string label="");
        void setAngle(int i, Vec3f pos, Vec3f v1, Vec3f v2, Vec3f c1, Vec3f c2, string label="");
        void setCircle(int i, Vec3f pos, Vec3f norm, float r, Vec3f color, string label="");
        void clear();
};

OSG_END_NAMESPACE;

#endif // VRANALYTICGEOMETRY_H_INCLUDED
