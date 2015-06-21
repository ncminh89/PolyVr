#ifndef VRCAMERAMANAGER_H_INCLUDED
#define VRCAMERAMANAGER_H_INCLUDED

#include <OpenSG/OSGConfig.h>
#include <string>
#include <vector>

OSG_BEGIN_NAMESPACE;
using namespace std;

class VRCamera;
class VRTransform;

class VRCameraManager {
    private:
        vector<VRCamera*> cameras;

        int active;

    public:
        VRCameraManager();
        ~VRCameraManager();

        VRTransform* addCamera(string name);
        void addCamera(VRCamera* cam);

        VRCamera* getCamera(int ID);

        void setMActiveCamera(int ID);

        VRCamera* getActiveCamera();
        int getActiveCameraIndex();

        vector<VRCamera*> getCameras();
        vector<string> getCameraNames();
};

OSG_END_NAMESPACE;

#endif // VRCAMERAMANAGER_H_INCLUDED
