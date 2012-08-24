#pragma once
#include "PimEngine.h"

class ThirdPersonCameraController : public CameraController
{
public:
    ThirdPersonCameraController(GameNode*,Ogre::Vector3);

    void mouseMoved(const OIS::MouseEvent&);


    // IMPORTANT!! CALL EITHER updateCamZoom OR updateCamTransit.
    // CALLING ONE WILL NULLIFY THE WORK OF THE OTHER.
    // Method must be called externally (f.i., player->update)
    void updateCamZoom(float dt);
    bool zoomAllowed;
    float zoomLimitNear;
    float zoomLimitFar;

    // Method must be called externally.
    // Example usage: move cam to the iron sights of a gun.
    // The position passed is relative to the parent of cameraNode,
    // which usually is the player node itself.
    //
    // updateCamTransit MUST BE CALLED EACH FRAME.
    void updateCamTransit(Ogre::Vector3 &pos,float total,float dt);

    // Used internally by updateCamTransit. Use at own risk.
    // Might be removed in the future.
    void transit(Ogre::Vector3 from, Ogre::Vector3 to,float total, float dt);

    // Ray cast to avoid camera clipping through the ground.
    // Method must be called externally (f.i., player->update)
    void rayCastCameraPosition(btDynamicsWorld *world);

    // Get the world position of the camera.
    Ogre::Vector3 camWorldPosition();

private:
    // The initial position of the camera, assigned to this variable
    // at creation. This is the "default" position of the camera.
    Ogre::Vector3 initialPos;

    // The destination the updateCamZoom function transits to.
    Ogre::Vector3 camDest;

    // Whenever the camera's position is overriden by rayCastCameraPosition,
    // the original camDest value is stored in oldCamDest, and
    // overridenCamDest is flagged. These are internal values, and
    // should never, ever, ever, ever be modified manually.
    Ogre::Vector3 oldCamDest;
    bool overriddenCamDest;
};

