#include "StdAfx.h"
#include "PimEngine.h"
#include "DebugDraw.h"

static GameScene* singleton = NULL;

static int sceneNodeCount = 0;
static int entityCount = 0;

GameScene::GameScene() : GameNode(true)
{
    mCamera		= NULL;
    mSceneMgr	= NULL;

    broadphase	= NULL;
    config		= NULL;
    dispatcher	= NULL;
    solver		= NULL;
    world		= NULL;
    debugDraw	= NULL;
}
GameScene::~GameScene(void)
{
    GameControl::getSingleton()->removeAllListeners();

    removeAllChildren(true);

    destroyAllEntities();
    mSceneMgr->destroySceneNode(sceneNode);
    sceneNode = NULL;

    mSceneMgr->destroyCamera(mCamera);
    mSceneMgr->destroyAllMovableObjects();
    GameControl::getRoot()->destroySceneManager(mSceneMgr);

    mCamera   = NULL;
    mSceneMgr = NULL;

    if (broadphase)	delete broadphase;
    if (config)		delete config;
    if (dispatcher)	delete dispatcher;
    if (solver)		delete solver;
    if (debugDraw)	delete debugDraw;
    if (world)		delete world;

    sceneNodeCount	= 0;
    entityCount		= 0;
    singleton		= NULL;
}

void GameScene::setupScene()
{
    if (singleton)
        throw new std::exception("Singleton already implemented");
    singleton = this;

    mSceneMgr = GameControl::getRoot()->createSceneManager(Ogre::SceneType::ST_GENERIC, "SceneManager");
    mCamera	  = mSceneMgr->createCamera("SceneCamera");

    sceneNode = createSceneNode();
}

GameScene* GameScene::getSingleton(void)
{
    return singleton;
}

Ogre::SceneNode* GameScene::createSceneNode(void)
{
    std::string s;
    std::stringstream out;
    out << sceneNodeCount++;
    s = out.str();

    return createSceneNode(s);
}
Ogre::SceneNode* GameScene::createSceneNode(Ogre::String str)
{
    return getSceneManager()->getRootSceneNode()->createChildSceneNode();
}
Ogre::Entity* GameScene::createEntity(Ogre::String str)
{
    std::string s;
    std::stringstream out;
    out << entityCount++;
    s = out.str();

    return getSceneManager()->createEntity(s, str);
}

void GameScene::physicsStep(float dt)
{
    if (world)
    {
        world->stepSimulation(dt, 10);
    }
}
void GameScene::drawDebugData()
{
    if (debugDraw)
        world->debugDrawWorld();
}

void GameScene::defaultBulletInit()
{
    broadphase  = new btDbvtBroadphase();
    config		= new btDefaultCollisionConfiguration();
    dispatcher  = new btCollisionDispatcher(config);
    solver		= new btSequentialImpulseConstraintSolver();
    //world		= new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);
    world		= new btSimpleDynamicsWorld(dispatcher, broadphase, solver, config);
    world->setGravity( btVector3(0,-150,0) );
}

void GameScene::clearScene()
{
    removeAllChildren(true);
}