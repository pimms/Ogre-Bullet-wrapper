#pragma once

#include "PimEngine.h"

class GameControl :
    public Ogre::FrameListener,
    public Ogre::RenderQueueListener,
    public Ogre::WindowEventListener,
    public OIS::KeyListener,
    public OIS::MouseListener
{
public:
    GameControl(GameScene*);
    ~GameControl();

    static GameControl* getSingleton();
    static Ogre::Root*  getRoot()
    {
        return getSingleton()->mRoot;
    }
    static Ogre::RenderWindow* getRenderWindow()
    {
        return getSingleton()->mWindow;
    }
    static Ogre::Viewport* getViewport()
    {
        return getSingleton()->mViewport;
    }

    virtual void go(std::string pName = "PimEngine");

    virtual bool setup();
    virtual bool configure();
    virtual void createViewports();
    virtual void createResourceListener();
    virtual void setupResources();
    virtual void loadResources();
    virtual void setupOIS();

    void removeAllListeners();

    void addFrameListener(GameNode*);
    void removeFrameListener(GameNode*);
    bool frameStarted			(const Ogre::FrameEvent &);
    bool frameRenderingQueued	(const Ogre::FrameEvent& evt);
    bool frameEnded				(const Ogre::FrameEvent&);

    void addRenderQueueListener(GameNode*);
    void removeRenderQueueListener(GameNode*);
    void preRenderQueues();
    void postRenderQueues();
    void renderQueueStarted(Ogre::uint8,const Ogre::String&, bool&);
    void renderQueueEnded(Ogre::uint8,const Ogre::String&, bool&);

    void addKeyListener(GameNode*);
    void removeKeyListener(GameNode*);
    bool keyPressed		(const OIS::KeyEvent & );
    bool keyReleased	(const OIS::KeyEvent & );

    void addMouseListener(GameNode*);
    void removeMouseListener(GameNode*);
    bool mouseMoved		(const OIS::MouseEvent & );
    bool mousePressed	(const OIS::MouseEvent &, OIS::MouseButtonID );
    bool mouseReleased	(const OIS::MouseEvent &, OIS::MouseButtonID );

    virtual void windowResized(Ogre::RenderWindow*);
    virtual void windowClosed(Ogre::RenderWindow*);

    void setGameScene(GameScene*);

    // vars
    Ogre::Root			*mRoot;
    Ogre::RenderWindow	*mWindow;
    Ogre::Viewport		*mViewport;
    Ogre::String		mResourcesCfg;
    Ogre::String		mPluginsCfg;
    bool mShutDown;
    bool drawDebugData;

    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;

    GameScene			*gameScene;

    std::vector<GameNode*> mouseListeners;
    std::vector<GameNode*> keyListeners;
    std::vector<GameNode*> frameListeners;
    std::vector<GameNode*> renderQueueListeners;

	
	std::string windowName;
};

