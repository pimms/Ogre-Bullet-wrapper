#include "StdAfx.h"
#include "PimEngine.h"
#include "HeightfieldData.h"

GameNode::GameNode(bool isGameScene)
{
    parent				= NULL;
    sceneNode			= NULL;
    mainEntity			= NULL;
    collisionShape		= NULL;
    body				= NULL;
    staticBody			= NULL;
    deleteShapeWithBody = false;
    bodyMeshDifference	= Ogre::Vector3(0,0,0);

    if (!isGameScene) sceneNode = GameScene::createSceneNode();
}
GameNode::~GameNode(void)
{

    if (parent)
        parent->removeChild(this,false);

    removeAllChildren(true);

    if (sceneNode)
    {
        destroyAllEntities();
        GameScene::getSceneManager()->destroySceneNode(sceneNode);
        sceneNode = NULL;
    }

    clearPhysicsObjects();

}

btCollisionShape* GameNode::createShapeFromEntity(Ogre::Entity *ent)
{
    BtOgre::StaticMeshToShapeConverter con;
    con.addEntity(ent);
    return con.createTrimesh();
}
HeightfieldData* GameNode::createHeightfieldData(std::string &file, std::string &resGrp)
{
    HeightfieldData *hfd = new HeightfieldData;

    Ogre::Image img;
    img.load(file, resGrp);

    // Allocate the data
    int IMX=img.getWidth()/2, IMY=img.getHeight()/2;
    hfd->data = new btScalar[IMX*IMY];
    hfd->w = IMX;
    hfd->h = IMY;

    for (int y=0; y<IMY; y++)
    {
        for (int x=0; x<IMX; x++)
        {
            // The texture should be grayscale, don't bother with averaging it out.
            hfd->data[y*IMY+x] = img.getColourAt(x,y,0).r * 50.f;
        }
    }

    return hfd;
}

void GameNode::scheduleFrameListener()
{
    GameControl::getSingleton()->addFrameListener(this);
}
void GameNode::unscheduleFrameListener()
{
    GameControl::getSingleton()->removeFrameListener(this);
}

void GameNode::scheduleRenderQueueListener()
{
    GameControl::getSingleton()->addRenderQueueListener(this);
}
void GameNode::unscheduleRenderQueueListener()
{
    GameControl::getSingleton()->removeRenderQueueListener(this);
}

void GameNode::scheduleKeyListener()
{
    GameControl::getSingleton()->addKeyListener(this);
}
void GameNode::unscheduleKeyListener()
{
    GameControl::getSingleton()->removeKeyListener(this);
}

void GameNode::scheduleMouseListener()
{
    GameControl::getSingleton()->addMouseListener(this);
}
void GameNode::unscheduleMouseListener()
{
    GameControl::getSingleton()->removeMouseListener(this);
}

bool GameNode::addChild(GameNode *node)
{
    if (!isChildren(node))
    {
        children.push_back(node);
        node->parent = this;

        if (node->sceneNode->getParentSceneNode())
            node->sceneNode->getParentSceneNode()->removeChild(node->sceneNode);
        sceneNode->addChild(node->sceneNode);
        return true;
    }

    return false;
}
bool GameNode::removeChild(GameNode *node, bool cleanup)
{
    for (unsigned int i=0; i<children.size(); i++)
    {
        if (children[i] == node)
        {
            if (node->sceneNode)
                sceneNode->removeChild(node->sceneNode);
            node->parent = NULL;

            if (cleanup)
            {
                delete node;
            }

            children.erase(children.begin() + i);
            return true;
        }
    }

    return false;
}
void GameNode::removeAllChildren(bool cleanup)
{
    for (int i=children.size()-1; i>=0; i--)
    {
        removeChild(children[i], cleanup);
    }
}
bool GameNode::isChildren(GameNode *node)
{
    for (unsigned int i=0; i<children.size(); i++)
    {
        if (children[i] == node)
            return true;
    }

    return false;
}

void GameNode::addEntity(Ogre::String meshFile, bool isMain)
{
    addEntity(GameScene::createEntity(meshFile), isMain);
}
void GameNode::addEntity(Ogre::Entity *entity, bool isMain)
{
    sceneNode->attachObject(entity);
    if (isMain)
        mainEntity = entity;
}
void GameNode::destroyEntity(Ogre::Entity *entity)
{
    if (entity->isAttached())
        entity->detachFromParent();
    GameScene::getSceneManager()->destroyEntity(entity);
}
void GameNode::destroyAllEntities()
{
    Ogre::SceneNode::ObjectIterator itEnt = sceneNode->getAttachedObjectIterator();

    while (itEnt.hasMoreElements())
    {
        Ogre::MovableObject *ent = itEnt.getNext();
        ent->detachFromParent();
        GameScene::getSceneManager()->destroyMovableObject(ent);
    }
}

void GameNode::addLight(Ogre::Light *light)
{
    sceneNode->attachObject(light);
}
void GameNode::removeLight(Ogre::Light *light)
{
    sceneNode->detachObject(light);
}

void GameNode::createStaticHeightmap(std::string file, std::string resGrp)
{
    HeightfieldData *hfd = createHeightfieldData(file, resGrp);

    btHeightfieldTerrainShape *shape = new btHeightfieldTerrainShape(
        hfd->w, hfd->h, hfd->data, 1.f, 1, true, false
    );

    createStaticBody(shape, true);
    delete hfd;
}
void GameNode::createStaticBody(btCollisionShape *shape, bool del)
{
    clearPhysicsObjects();

    collisionShape = shape;
    deleteShapeWithBody = del;

    staticBody = new btCollisionObject;
    staticBody->setCollisionShape(collisionShape);
    staticBody->setUserPointer(this);
    GameScene::getWorld()->addCollisionObject(staticBody);

	staticBody->setCollisionFlags(
        staticBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT
    );
}

void GameNode::createRigidBody(btCollisionShape *shape, bool del, float mass)
{
    clearPhysicsObjects();

    btVector3 inertia;
    shape->calculateLocalInertia(mass, inertia);

    motionState = new btDefaultMotionState();
    body = new btRigidBody(mass, motionState, shape, inertia);
    body->setUserPointer(this);
    collisionShape = shape;
    deleteShapeWithBody = del;

    GameScene::getWorld()->addRigidBody(body);
}
void GameNode::setBodyMeshDifference(Ogre::Vector3 &diff)
{
    bodyMeshDifference = diff;
    alignSceneNode();
}
void GameNode::alignSceneNode()
{
    if (body)
    {
        // Add the rotation to the BMD-vector
        float angle = bodyMeshDifference.angleBetween(Ogre::Vector3(0.f, 0.f, 1.f)).valueRadians();
        if (bodyMeshDifference.x < 0) angle += 3.14f;
        Ogre::Vector3 newBMD;
        newBMD.x = bodyMeshDifference.length() * sinf( angle + getYaw() );
        newBMD.y = 0.f;
        newBMD.z = bodyMeshDifference.length() * cosf( angle + getYaw() );
        //cprintf("%f - %f, %f\n", angle, newBMD.x, newBMD.z);

        sceneNode->setPosition( toOgre(body->getWorldTransform().getOrigin()) + newBMD );
        sceneNode->setOrientation( toOgre(body->getWorldTransform().getRotation()) );
    }
    else if (staticBody)
    {
        sceneNode->setPosition( toOgre( staticBody->getWorldTransform().getOrigin() ) );
        sceneNode->setOrientation( toOgre( staticBody->getWorldTransform().getRotation() ) );
    }
}

void GameNode::clearPhysicsObjects()
{
    if (body)
    {
        GameScene::getWorld()->removeRigidBody(body);
        delete body;
        delete motionState;
        body		= NULL;
    }
    if (staticBody)
    {
        GameScene::getWorld()->removeCollisionObject(staticBody);
        delete staticBody;
        motionState = NULL;
    }

    if (deleteShapeWithBody && collisionShape)
    {
        delete collisionShape;
        deleteShapeWithBody = false;
        collisionShape = NULL;
    }
}

Ogre::Vector3 GameNode::getPosition()
{
    if (body)
        return toOgre(body->getWorldTransform().getOrigin());
    if (staticBody)
        return toOgre(staticBody->getWorldTransform().getOrigin());

    return sceneNode->getPosition();
}
Ogre::Quaternion GameNode::getRotation()
{
    if (body)
        return toOgre(body->getWorldTransform().getRotation());
    if (staticBody)
        return toOgre(staticBody->getWorldTransform().getRotation());
    return sceneNode->getOrientation();
}
float GameNode::getPitch()
{
    if (body)
        return toOgre(body->getWorldTransform().getRotation()).getPitch().valueRadians();
    if (staticBody)
        return toOgre(staticBody->getWorldTransform().getRotation()).getPitch().valueRadians();

    return sceneNode->getOrientation().getPitch().valueRadians();
}
float GameNode::getRoll()
{
    if (body)
        return toOgre(body->getWorldTransform().getRotation()).getRoll().valueRadians();
    if (staticBody)
        return toOgre(staticBody->getWorldTransform().getRotation()).getRoll().valueRadians();

    return sceneNode->getOrientation().getRoll().valueRadians();
}
float GameNode::getYaw()
{
    if (body)
        return toOgre(body->getWorldTransform().getRotation()).getYaw().valueRadians();
    if (staticBody)
        return toOgre(staticBody->getWorldTransform().getRotation()).getYaw().valueRadians();

    return sceneNode->getOrientation().getYaw().valueRadians();
}

void GameNode::setPosition(btVector3 &pos)
{
    if (body)
    {
        btTransform t = body->getWorldTransform();
        t.setOrigin(pos);
        body->setWorldTransform(t);
        alignSceneNode();
    }
    else if (staticBody)
    {
        btTransform t = staticBody->getWorldTransform();
        t.setOrigin(pos);
        staticBody->setWorldTransform(t);
        alignSceneNode();
    }
    else
    {
        sceneNode->setPosition( toOgre(pos) );
    }
}
void GameNode::setOrientation(btQuaternion &q)
{
    if (body)
    {
        btTransform t = body->getWorldTransform();
        t.setRotation(q);
        body->setWorldTransform(t);
        alignSceneNode();
    }
    else if (staticBody)
    {
        btTransform t = staticBody->getWorldTransform();
        t.setRotation(q);
        staticBody->setWorldTransform(t);
        alignSceneNode();
    }
    else
    {
        sceneNode->setOrientation( toOgre(q) );
    }
}
void GameNode::setWorldTransform(btVector3 &pos, btQuaternion &q)
{
    if (body)
    {
        btTransform t = body->getWorldTransform();
        t.setRotation(q);
        t.setOrigin(pos);
        body->setWorldTransform(t);
        alignSceneNode();
    }
    else if (staticBody)
    {
        btTransform t = staticBody->getWorldTransform();
        t.setRotation(q);
        t.setOrigin(pos);
        staticBody->setWorldTransform(t);
        alignSceneNode();
    }
    else
    {
        sceneNode->setPosition( toOgre(pos) );
        sceneNode->setOrientation( toOgre(q) );
    }
}