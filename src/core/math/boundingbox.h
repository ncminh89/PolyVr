#ifndef BOUNDINGBOX_H_INCLUDED
#define BOUNDINGBOX_H_INCLUDED

#include <OpenSG/OSGVector.h>
#include <OpenSG/OSGLine.h>
#include "core/objects/VRObjectFwd.h"
#include "core/math/VRMathFwd.h"

using namespace std;
OSG_BEGIN_NAMESPACE;

class Boundingbox {
    private:
        Vec3f bb1, bb2;
        bool cleared = true;

    public:
        Boundingbox();
        static BoundingboxPtr create();

        void clear();
        bool empty() const;

        void update(const Vec3f& v);
        void updateFromPoints(const vector<Vec3f>& v);
        void updateFromGeometry(VRGeometryPtr g);

        Vec3f min() const;
        Vec3f max() const;
        Vec3f center() const;
        Vec3f size() const;
        float radius() const;

        void setCenter(const Vec3f& t);
        void move(const Vec3f& t);
        void scale(float s);

        bool isInside(Vec3f p) const;
        bool intersectedBy(Line l);

        Vec3f getRandomPoint();
};

OSG_END_NAMESPACE;

#endif // BOUNDINGBOX_H_INCLUDED
