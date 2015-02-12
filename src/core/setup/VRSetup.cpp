#include "VRSetup.h"
#include "windows/VRWindow.h"
#include "core/scene/VRSceneManager.h"
#include "core/scene/VRScene.h"
#include "core/utils/VROptions.h"
#include "core/utils/VRVisualLayer.h"
#include "core/setup/devices/VRMouse.h"
#include "core/objects/VRTransform.h"
#include "core/objects/VRCamera.h"
#include "core/objects/object/VRObjectT.h"
#include <libxml++/libxml++.h>
#include <libxml++/nodes/element.h>

OSG_BEGIN_NAMESPACE;
using namespace std;




//parser callback for the xml scene import
void VRSetup::parseSetup(xmlpp::Element* setup) {
    ;
}


void VRSetup::processOptions() {
    cfgfile = VROptions::get()->getOption<string>("setupCfg");

    static xmlpp::DomParser parser;
    parser.parse_file(cfgfile);
    parseSetup(parser.get_document()->get_root_node());
}

VRSetup::VRSetup(string name) {
    setName(name);
    real_root = new VRTransform("VR Setup");
    setup_cam = new VRCamera("Setup");
    setViewAnchor(real_root);
    setDeviceRoot(real_root);
    real_root->addAttachment("treeviewNotDragable", 0);
    real_root->addAttachment("global", 0);
    real_root->addChild(setup_cam);
    setup_cam->setAcceptRoot(false);
    user = 0;
    tracking = "None";

    setup_layer = new VRVisualLayer("setup", "setup.png");
    setup_layer->setCallback( new VRFunction<bool>("showSetup", boost::bind(&VRSetup::showSetup, this, _1) ) );
}

VRSetup::~VRSetup() {
    if (user) delete user;
    delete real_root;
}

void VRSetup::setScene(VRScene* scene) {
    if (scene == 0) return;
    VRCamera* cam = scene->getActiveCamera();
    if (cam == 0) return;
    setViewRoot(scene->getRoot(), -1);
    setViewCamera(cam, -1);

    VRMouse* mouse = (VRMouse*)getDevice("mouse");
    if (mouse and cam) mouse->setCamera(cam);

    setViewBackground(scene->getBackground());

    for (auto w : getWindows()) w.second->setContent(true);

    scene->addCamera(setup_cam);

    //scene->initDevices();
}

void VRSetup::addObject(VRObject* o) { real_root->addChild(o); }

void VRSetup::showSetup(bool b) { // TODO
    showViewportGeos(b);
}

VRTransform* VRSetup::getUser() { return user; }
VRTransform* VRSetup::getRoot() { return real_root; }

VRTransform* VRSetup::getTracker(string t) {
    vector<string> devs = getARTDevices();
    for (uint i=0; i< devs.size(); i++) {
        ART_device* dev = getARTDevice(devs[i]);
        if (dev->ent->getName() == t) return dev->ent;
    }

    /*vector<int> IDs = getVRPNTrackerIDs();
    for (uint i=0; i< IDs.size(); i++) {
        VRPN_device* tr = getVRPNTracker(IDs[i]);
        if (tr->getName() == t) return tr->getBeacon();
    }*/

    return 0;
}

xmlpp::Element* VRSetup::getElementChild(xmlpp::Element* e, string name) {
    xmlpp::Node::NodeList nl = e->get_children();
    xmlpp::Node::NodeList::iterator itr;
    for (itr = nl.begin(); itr != nl.end(); itr++) {
        xmlpp::Node* n = *itr;

        xmlpp::Element* el = dynamic_cast<xmlpp::Element*>(n);
        if (!el) continue;

        if (el->get_name() == name) return el;
    }
    return 0;
}

void VRSetup::save(string file) {
    xmlpp::Document doc;
    xmlpp::Element* setupN = doc.create_root_node("Setup", "", "VRF"); //name, ns_uri, ns_prefix
    xmlpp::Element* displayN = setupN->add_child("Displays");
    xmlpp::Element* deviceN = setupN->add_child("Devices");
    xmlpp::Element* trackingARTN = setupN->add_child("TrackingART");
    xmlpp::Element* trackingVRPNN = setupN->add_child("TrackingVRPN");

    VRWindowManager::save(displayN);
    VRDeviceManager::save(deviceN);
    ART::save(trackingARTN);
    VRPN::save(trackingVRPNN);

    doc.write_to_file_formatted(file);
}

void VRSetup::load(string file) {
    xmlpp::DomParser parser;
    parser.set_validate(false);
    parser.parse_file(file.c_str());

    xmlpp::Node* n = parser.get_document()->get_root_node();
    xmlpp::Element* setupN = dynamic_cast<xmlpp::Element*>(n);
    xmlpp::Element* displayN = getElementChild(setupN, "Displays");
    xmlpp::Element* deviceN = getElementChild(setupN, "Devices");
    xmlpp::Element* trackingARTN = getElementChild(setupN, "TrackingART");
    xmlpp::Element* trackingVRPNN = getElementChild(setupN, "TrackingVRPN");

    if (trackingARTN) ART::load(trackingARTN);
    if (trackingVRPNN) VRPN::load(trackingVRPNN);
    if (deviceN) VRDeviceManager::load(deviceN);
    if (displayN) VRWindowManager::load(displayN);
}

OSG_END_NAMESPACE;
