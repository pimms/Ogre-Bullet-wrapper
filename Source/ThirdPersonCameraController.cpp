#include "StdAfx.h"
#include "PimEngine.h"


ThirdPersonCameraController::ThirdPersonCameraController(GameNode *node, Ogre::Vector3 pos)
    : CameraController(node)
{
    zoomAllowed		  = true;
    zoomLimitNear	  = 10.f;
    zoomLimitFar	  = 25.f;
    camDest			  = pos;
    oldCamDest		  = pos;
    overriddenCamDest = false;
    initialPos		  = pos;

    camera->setPosition(camDest);
}

void ThirdPersonCameraController::mouseMoved(const OIS::MouseEvent &evt)
{
    Ogre::Radian yaw = parent->getRotation().getYaw();
    float newYaw = yaw.valueRadians();
    newYaw += (float)evt.state.X.rel / -xFactor;
    yaw = Ogre::Radian(newYaw);

    Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;
    q.FromAngleAxis(yaw, Ogre::Vector3(0,1,0));
    parent->setOrientation(toBullet(q));

    Ogre::Radian pitch = cameraNode->getOrientation().getPitch();
    float newPitch = pitch.valueRadians();
    newPitch += (float)evt.state.Y.rel / -yFactor;
    pitch = Ogre::Radian(newPitch);

    q = Ogre::Quaternion::IDENTITY;
    q.FromAngleAxis(pitch, Ogre::Vector3(1,0,0));
    cameraNode->setOrientation(q);

    if (!zoomAllowed) return;

    int wheelStep = evt.state.Z.rel / -120;
    if (wheelStep != 0)
    {
        camDest.z += 0.5f * wheelStep;
        if (camDest.z < zoomLimitNear) camDest.z = zoomLimitNear;
        if (camDest.z > zoomLimitFar) camDest.z = zoomLimitFar;
    }
}

void ThirdPersonCameraController::updateCamZoom(float dt)
{
    transit(camera->getPosition(), camDest, 1.f, dt * 6.f);
}
void ThirdPersonCameraController::updateCamTransit(Ogre::Vector3 &pos,float total, float dt)
{
    pos -= cameraNode->getPosition();
    transit(camera->getPosition(), pos, total, dt * 6.f);
}
void ThirdPersonCameraController::transit(Ogre::Vector3 from, Ogre::Vector3 to, float total, float dt)
{
    Ogre::Vector3 newPos = camera->getPosition() + (to - from) * (dt / total);
    camera->setPosition(newPos);
}

void ThirdPersonCameraController::rayCastCameraPosition(btDynamicsWorld *world)
{
    btVector3 from = toBullet(cameraNode->convertLocalToWorldPosition(Ogre::Vector3(0.f,0.f,0.f)));
    btVector3 to = toBullet(cameraNode->convertLocalToWorldPosition(camera->getPosition()));

    btCollisionWorld::ClosestRayResultCallback cb(from,to);
    GameScene::getWorld()->rayTest(from,to,cb);
    if (cb.hasHit())
    {
        if (!overriddenCamDest)
        {
            oldCamDest = camDest;
            overriddenCamDest = true;
        }
        camDest = cameraNode->convertWorldToLocalPosition(toOgre(cb.m_hitPointWorld));
    }
    else if (overriddenCamDest)
    {
        overriddenCamDest = false;
        camDest = oldCamDest;
    }
}
Ogre::Vector3 ThirdPersonCameraController::camWorldPosition()
{
    return Ogre::Vector3(cameraNode->convertLocalToWorldPosition(camera->getPosition()));
}
