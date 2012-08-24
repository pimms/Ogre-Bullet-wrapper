#pragma once
#include "PimEngine.h"

class DebugDraw;
class GameControl;
class GameScene : public GameNode
{
public:
    GameScene();
    ~GameScene(void);

    void setupScene();
    virtual void loadSceneResources() = 0;

    static GameScene* getSingleton(void);
    static btDynamicsWorld* getWorld()
    {
        return getSingleton()->world;
    }
    static Ogre::SceneManager* getSceneManager(void)
    {
        return getSingleton()->mSceneMgr;
    }
    static Ogre::Camera* getCamera(void)
    {
        return getSingleton()->mCamera;
    }

    static Ogre::SceneNode* createSceneNode(void);
    static Ogre::SceneNode* createSceneNode(Ogre::String);
    static Ogre::Entity*	createEntity(Ogre::String);

    void physicsStep(float);
    void drawDebugData();

protected:
    friend GameControl;

    void defaultBulletInit();

    void clearScene();

    Ogre::SceneManager					*mSceneMgr;
    Ogre::Camera						*mCamera;
    btDynamicsWorld						*world;
    btBroadphaseInterface				*broadphase;
    btDefaultCollisionConfiguration		*config;
    btCollisionDispatcher				*dispatcher;
    btSequentialImpulseConstraintSolver *solver;
    DebugDraw							*debugDraw;
};

