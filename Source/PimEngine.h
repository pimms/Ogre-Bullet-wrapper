#pragma once

#include "stdafx.h"

// STD
#include <vector>
#include <sstream>
#include <conio.h>

// Ogre
#include "OGRE\OgreCamera.h"
#include "OGRE\OgreEntity.h"
#include "OGRE\OgreLogManager.h"
#include "OGRE\OgreRoot.h"
#include "OGRE\OgreViewport.h"
#include "OGRE\OgreSceneManager.h"
#include "OGRE\OgreRenderWindow.h"
#include "OGRE\OgreConfigFile.h"
#include "OGRE\OgreWindowEventUtilities.h"
#include "OGRE\RenderSystems\GL\GL\gl.h"

// OIS
#include <OIS\OISEvents.h>
#include <OIS\OISInputManager.h>
#include <OIS\OISKeyboard.h>
#include <OIS\OISMouse.h>

// Bullet
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h"

// btOgre
#include "BtOgreExtras.h"
#include "BtOgreGP.h"
#include "BtOgrePG.h"

// TinyXML
#include "tinyxml.h"

// DotScene loader

// Game files
#include "DebugDraw.h"
#include "GameNode.h"
#include "GameScene.h"
#include "HeightfieldData.h"

#include "CameraController.h"
#include "ThirdPersonCameraController.h"

#include "GameControl.h"


Ogre::Vector3 toOgre(const btVector3&);
Ogre::Quaternion toOgre(const btQuaternion&);

btVector3 toBullet(const Ogre::Vector3&);
btQuaternion toBullet(const Ogre::Quaternion&);