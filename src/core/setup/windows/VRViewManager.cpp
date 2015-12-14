#include "VRViewManager.h"
#include "VRView.h"
#include "core/objects/VRTransform.h"

OSG_BEGIN_NAMESPACE;
using namespace std;

bool VRViewManager::checkView(int i) {
    if (i == -1) return false;
    else if (views.count(i)) return true;
    else {
        cout << "\nError! request for not existing view " << i << "\n";
        return false;
    }
}

void VRViewManager::setViewAnchor(VRTransformPtr a) { anchor = a; }

void VRViewManager::showViewStats(int i, bool b) {
    if (checkView(i)) views[i]->showStats(b);
}

VRViewManager::VRViewManager() {
    cout << "Init VRViewManager\n";
    //this->view_root = view_root;
}

VRViewManager::~VRViewManager() {}

//int VRViewManager::addView(bool active_stereo, bool stereo, bool projection, Pnt3f screenLowerLeft, Pnt3f screenLowerRight, Pnt3f screenUpperRight, Pnt3f screenUpperLeft, bool swapeyes) {
int VRViewManager::addView(string name) {
    //VRViewPtr view = new VRView(active_stereo, stereo, projection, screenLowerLeft, screenLowerRight, screenUpperRight, screenUpperLeft, swapeyes);
    VRViewPtr view = VRView::create(name);
    int id=0;
    while(views.count(id) == 1) id++;
    views[id] = view;
    view->setRoot(0, anchor);
    view->setID(id);
    return id;
}

void VRViewManager::setViewCamera(VRCameraPtr c, int i) {
    if (checkView(i)) views[i]->setCamera(c);
    else if(i == -1) for (auto v : views) v.second->setCamera(c);
}

void VRViewManager::setViewRoot(VRObjectPtr root, int i) {
    if (checkView(i)) views[i]->setRoot(root, anchor);
    else if(i == -1) for (auto v : views) v.second->setRoot(root, anchor);
}

void VRViewManager::setViewUser(VRTransformPtr user, int i) { // not used
    anchor->addChild(user);
    if (checkView(i)) views[i]->setUser(user);
    else if(i == -1) for (auto v : views) v.second->setUser(user);
}

void VRViewManager::setViewBackground(BackgroundRecPtr bg, int i) {
    if (checkView(i)) views[i]->setBackground(bg);
    else if(i == -1) for (auto v : views) v.second->setBackground(bg);
}

void VRViewManager::showViewportGeos(bool b) {
    for (auto v : views) v.second->showViewGeo(b);
}

void VRViewManager::removeView(int i) {
    if (!checkView(i)) return;
    views.erase(i);
}

//void VRViewManager::addRWElement(VRTransformPtr obj) { view_root->addChild(obj); }

VRTransformPtr VRViewManager::getViewUser(int i) { if (checkView(i)) return views[i]->getUser(); else return 0; }
VRViewPtr VRViewManager::getView(int i) { if (checkView(i)) return views[i]; else return 0; }

void VRViewManager::setStereo(bool b) { for (auto v : views) v.second->setStereo(b); }
void VRViewManager::setStereoEyeSeparation(float f) { for (auto v : views) v.second->setStereoEyeSeparation(f); }
void VRViewManager::resetViewports() { for (auto v : views) v.second->reset(); }
void VRViewManager::setFotoMode(bool b) { for (auto v : views) v.second->setFotoMode(b); }
void VRViewManager::setCallibrationMode(bool b) { for (auto v : views) v.second->setCallibrationMode(b); }

OSG_END_NAMESPACE;
