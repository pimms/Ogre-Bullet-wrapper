#include "StdAfx.h"
#include "PimEngine.h"

static CameraController *singleton = NULL;

CameraController::CameraController(GameNode *node)
{
    if (singleton)
        cprintf("\nPOSSIBLE ERROR: More than one camera controller is initialized.\n");

    if (!node)
        cprintf("\nPOSSIBLE ERROR: No parent node passed onto CameraController.\n");

    parent = node;
    cameraNode = parent->sceneNode->createChildSceneNode();
    camera = GameScene::getCamera();
    cameraNode->attachObject(camera);

    xFactor = 400.f;
    yFactor = 600.f;
}
CameraController::~CameraController(void)
{
    singleton = NULL;
    cameraNode->detachAllObjects();
    GameScene::getSceneManager()->destroySceneNode(cameraNode);
}

void CameraController::setCameraNodePosition(Ogre::Vector3 &pos)
{
    cameraNode->setPosition(pos);
}
void CameraController::setCameraPosition(Ogre::Vector3 &pos)
{
    camera->setPosition(pos);
}
void CameraController::lookAt(Ogre::Vector3 &pos)
{
    camera->lookAt(pos);
}
