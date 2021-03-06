#ifndef VRSKY_H_INCLUDED
#define VRSKY_H_INCLUDED

#include "core/objects/VRObjectFwd.h"
#include "core/scene/VRSceneManager.h"
#include "VRGeometry.h"

OSG_BEGIN_NAMESPACE;
using namespace std;

class VRSky : public VRGeometry {
    protected:
        struct Date {
            double second = 0;
            int hour = 12;
            int day = 0;
            int year = 2000;

            void propagate(double seconds); // positive or negative
            double getDay();

        };

        struct Position {
            float latitude = 0;
            float longitude = 0;
        };

        VRUpdateCbPtr updatePtr;
        VRMaterialPtr mat;

        // time and location
        double lastTime = 0;
        Date date;
        float speed; // how quickly time passes
        Position observerPosition;

        // clouds
        uint textureSize;
        Vec2f cloudVel;
        Vec2f cloudOffset;
        float cloudDensity;
        float cloudScale;
        float cloudHeight;

        // luminance
        float turbidity;
        Vec3f xyY_z;
        Vec3f coeffsA;
        Vec3f coeffsB;
        Vec3f coeffsC;
        Vec3f coeffsD;
        Vec3f coeffsE;

        // sun
        Vec3f sunPos;
        float theta_s;

        void setTurbidity(float t);
        void calculateCoeffs();
        void calculateZenithColor();
        void updateClouds(float dt);
        void sunFromTime();
        void setCloudVel(float x, float z);
        void update();

    public:
        VRSky();
        ~VRSky();

        static VRSkyPtr create();
        VRSkyPtr ptr();
        void setTime(double second, int hour, int day, int year = 2000); // 0-3600, 0-24, 0-356, x
        void setSpeed(float speed = 1);
        void setWeather(float cCover, float cHeight, float wind, float haze);
        void setPosition(float latitude, float longitude);
        void reloadShader();
};

OSG_END_NAMESPACE;

#endif // VRSKY_H_INCLUDED
