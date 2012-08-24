#pragma once
#include "PimEngine.h"

class HeightfieldData;

class GameNode :
    public Ogre::FrameListener,
    public Ogre::RenderQueueListener,
    public OIS::MouseListener,
    public OIS::KeyListener
{
public:
    GameNode(bool isGameScene=false);
    ~GameNode(void);

    static btCollisionShape* createShapeFromEntity(Ogre::Entity*);
    static HeightfieldData* createHeightfieldData(std::string&,std::string&);

    virtual bool frameStarted		 (const Ogre::FrameEvent &)
    {
        return true;
    }
    virtual bool frameRenderingQueued(const Ogre::FrameEvent &)
    {
        return true;
    }
    virtual bool frameEnded			 (const Ogre::FrameEvent &)
    {
        return true;
    }

    virtual void preRenderQueues() {}
    virtual void postRenderQueues() {}
    virtual void renderQueueStarted(Ogre::uint8,const Ogre::String&, bool&) {}
    virtual void renderQueueEnded(Ogre::uint8,const Ogre::String&, bool&) {}

    virtual bool mouseMoved(const OIS::MouseEvent&)
    {
        return true;
    }
    virtual bool mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID)
    {
        return true;
    }
    virtual bool mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID)
    {
        return true;
    }

    virtual bool keyPressed(const OIS::KeyEvent&)
    {
        return true;
    }
    virtual bool keyReleased(const OIS::KeyEvent&)
    {
        return true;
    }

    /*
    	IMPORTANT NOTICE ABOUT SCHEDULE METHODS!
    	The scheduler methods are a "safer" alternative to scheduling
    	a game node for Ogre events. When using the schedule methods,
    	the GameControl singleton is delegating the events as they
    	appear.

    	In the future, GameControl won't send events if the game is
    	paused. Ogre however, will.
    */
    void scheduleFrameListener();
    void unscheduleFrameListener();

    void scheduleRenderQueueListener();
    void unscheduleRenderQueueListener();

    void scheduleKeyListener();
    void unscheduleKeyListener();

    void scheduleMouseListener();
    void unscheduleMouseListener();

    bool addChild(GameNode *node);
    bool removeChild(GameNode *node, bool cleanup);
    void removeAllChildren(bool cleanup);
    bool isChildren(GameNode *node);

    void addEntity(Ogre::String,bool);
    void addEntity(Ogre::Entity*,bool);
    void destroyEntity(Ogre::Entity*);
    void destroyAllEntities();

    void addLight(Ogre::Light*);
    void removeLight(Ogre::Light*);

    void createStaticHeightmap(std::string,std::string);
    void createStaticBody(btCollisionShape*,bool);

    void createRigidBody(btCollisionShape*,bool,float);
    void setBodyMeshDifference(Ogre::Vector3&);
    void alignSceneNode();

    void clearPhysicsObjects();

    virtual Ogre::Vector3 getPosition();
    virtual Ogre::Quaternion getRotation();
    virtual float getPitch();
    virtual float getRoll();
    virtual float getYaw();

    virtual void setPosition(btVector3&);
    virtual void setOrientation(btQuaternion&);
    virtual void setWorldTransform(btVector3&,btQuaternion&);

    GameNode				*parent;
    std::vector<GameNode*>	children;
    Ogre::SceneNode			*sceneNode;
    Ogre::Entity			*mainEntity;
    btDefaultMotionState	*motionState;
    btCollisionShape		*collisionShape;
    btRigidBody				*body;
    btCollisionObject		*staticBody;
    Ogre::Vector3			bodyMeshDifference;
    bool deleteShapeWithBody;
    bool setup;
};

