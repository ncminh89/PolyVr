#ifndef VRSETUPFWD_H_INCLUDED
#define VRSETUPFWD_H_INCLUDED

#include <memory>

#define ptrFwd( X ) \
class X; \
typedef std::shared_ptr<X> X ## Ptr; \
typedef std::weak_ptr<X> X ## WeakPtr;

namespace OSG {

ptrFwd(VRSetup);
ptrFwd(VRView);
ptrFwd(VRWindow);
ptrFwd(VRGtkWindow);
ptrFwd(VRGlutWindow);
ptrFwd(VRMultiWindow);
ptrFwd(VRNetwork);
ptrFwd(VRNetworkNode);
ptrFwd(VRNetworkSlave);
ptrFwd(VRRenderStudio);
ptrFwd(Vive);

}

#endif // VRSETUPFWD_H_INCLUDED
