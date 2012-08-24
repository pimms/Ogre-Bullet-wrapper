#pragma once
#include "PimEngine.h"

class CameraController
{
public:
    CameraController(GameNode*);
    ~CameraController(void);

    void setCameraNodePosition(Ogre::Vector3&);
    void setCameraPosition(Ogre::Vector3&);
    void lookAt(Ogre::Vector3&);

    virtual void mouseMoved(const OIS::MouseEvent&) = 0;
    virtual void mouseClicked(const OIS::MouseEvent&) {}

    Ogre::Camera *camera;
    Ogre::SceneNode *cameraNode;
    GameNode *parent;

    float xFactor, yFactor;
};

