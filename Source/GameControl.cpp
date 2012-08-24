#include "StdAfx.h"
#include "PimEngine.h"

static GameControl *singleton = NULL;
static int sceneNodeCount = 0;
static int entityCount = 0;

GameControl::GameControl(GameScene *scene)
{
    if (singleton)
        throw new std::exception("Error: Singleton already instantiated!");
    if (!scene)
        throw new std::exception("Error: A GameScene must be passed to GameControl at init.");
    singleton = this;
    mShutDown = false;
    drawDebugData = false;
    gameScene = scene;
}
GameControl::~GameControl(void)
{
    if (gameScene)
        delete gameScene;

    singleton = NULL;
    removeAllListeners();
}

GameControl* GameControl::getSingleton(void)
{
    return singleton;
}

void GameControl::go(std::string pName)
{
#ifdef _DEBUG
    mPluginsCfg = "plugins_d.cfg";
    mResourcesCfg = "resources.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

	windowName = pName;

    if (!setup())
        return;

    gameScene->loadSceneResources();

    mRoot->startRendering();

    delete gameScene;
    delete mRoot;
}

bool GameControl::setup()
{
    mRoot = new Ogre::Root(mPluginsCfg);

    if (!configure()) return false;

    gameScene->setupScene();

    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();

    setupResources();

    // Load resources
    loadResources();

    setupOIS();

    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
    mRoot->addFrameListener(this);
    gameScene->getSceneManager()->addRenderQueueListener(this);

    return true;
}
bool GameControl::configure()
{
    if(mRoot->restoreConfig())
    {
        mWindow = mRoot->initialise(true, windowName);
        return true;
    }
    else
    {
        return false;
    }
}
void GameControl::createViewports()
{
    // Create one viewport, entire window
    mWindow->removeAllViewports();

    mViewport = mWindow->addViewport(gameScene->mCamera);
    mViewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.2f, 0.7f));

    // Alter the camera aspect ratio to match the viewport
    gameScene->mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
}
void GameControl::createResourceListener()
{
    // For loading screens.. ?
}
void GameControl::setupResources()
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}
void GameControl::loadResources()
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
void GameControl::setupOIS()
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    //Set initial mouse clipping size
    windowResized(mWindow);

    Ogre::LogManager::getSingletonPtr()->logMessage("*** OIS initialized ***");
}

void GameControl::removeAllListeners()
{
    frameListeners.clear();
    renderQueueListeners.clear();
    mouseListeners.clear();
    keyListeners.clear();
}

void GameControl::addFrameListener(GameNode *list)
{
    frameListeners.push_back(list);
}
void GameControl::removeFrameListener(GameNode *list)
{
    for (unsigned int i=0; i<frameListeners.size(); i++)
    {
        if (frameListeners[i] == list)
            frameListeners.erase(frameListeners.begin() + i);
    }
}
bool GameControl::frameStarted(const Ogre::FrameEvent &evt)
{
    gameScene->physicsStep(evt.timeSinceLastFrame);

    for (unsigned int i=0; i<frameListeners.size(); i++)
        frameListeners[i]->frameStarted(evt);

    return true;
}
bool GameControl::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

    for (unsigned int i=0; i<frameListeners.size(); i++)
        frameListeners[i]->frameRenderingQueued(evt);

    return true;
}
bool GameControl::frameEnded(const Ogre::FrameEvent &evt)
{
    for (unsigned int i=0; i<frameListeners.size(); i++)
        frameListeners[i]->frameEnded(evt);

    return true;
}

void GameControl::addRenderQueueListener(GameNode *list)
{
    renderQueueListeners.push_back(list);
}
void GameControl::removeRenderQueueListener(GameNode *list)
{
    for (unsigned int i=0; i<renderQueueListeners.size(); i++)
    {
        if (renderQueueListeners[i] == list)
            renderQueueListeners.erase(renderQueueListeners.begin() + i);
    }
}
void GameControl::preRenderQueues()
{
    for (unsigned int i=0; i<renderQueueListeners.size(); i++)
        renderQueueListeners[i]->preRenderQueues();
}
void GameControl::postRenderQueues()
{
    for (unsigned int i=0; i<renderQueueListeners.size(); i++)
        renderQueueListeners[i]->postRenderQueues();
}
void GameControl::renderQueueStarted(Ogre::uint8 id, const Ogre::String& inv, bool& repInv)
{
    for (unsigned int i=0; i<renderQueueListeners.size(); i++)
        renderQueueListeners[i]->renderQueueStarted(id,inv,repInv);
}
void GameControl::renderQueueEnded(Ogre::uint8 id, const Ogre::String& inv, bool& repInv)
{
    // PRESERVATION
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();

    Ogre::RenderSystem *renderSystem = gameScene->mSceneMgr->getDestinationRenderSystem();
    renderSystem->_setWorldMatrix(gameScene->sceneNode->_getFullTransform());
    renderSystem->_setViewMatrix(gameScene->mCamera->getViewMatrix());
    renderSystem->_setProjectionMatrix(gameScene->mCamera->getProjectionMatrixRS());

    Ogre::MaterialPtr clearMat = Ogre::MaterialManager::getSingleton().getByName("BaseWhite");
    Ogre::Pass* clearPass = clearMat->getTechnique(0)->getPass(0);

    GLboolean stencilTestEnabled = glIsEnabled(GL_STENCIL_TEST);
    glDisable(GL_STENCIL_TEST);
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    // Tell listeners to do their shit
    for (unsigned int i=0; i<renderQueueListeners.size(); i++)
        renderQueueListeners[i]->renderQueueEnded(id,inv,repInv);

    // Draw dat debug
    if (drawDebugData)
        gameScene->drawDebugData();

    // RESTORATION
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    if (stencilTestEnabled)
        glEnable(GL_STENCIL_TEST);

    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void GameControl::addKeyListener(GameNode *list)
{
    keyListeners.push_back(list);
}
void GameControl::removeKeyListener(GameNode *list)
{
    for (unsigned int i=0; i<keyListeners.size(); i++)
    {
        if (keyListeners[i] == list)
            keyListeners.erase(keyListeners.begin() + i);
    }
}
bool GameControl::keyPressed( const OIS::KeyEvent &arg )
{
    for (unsigned int i=0; i<keyListeners.size(); i++)
        keyListeners[i]->keyPressed(arg);

    return true;
}
bool GameControl::keyReleased( const OIS::KeyEvent &arg )
{
    for (unsigned int i=0; i<keyListeners.size(); i++)
        keyListeners[i]->keyReleased(arg);

    return true;
}

void GameControl::addMouseListener(GameNode *list)
{
    mouseListeners.push_back(list);
}
void GameControl::removeMouseListener(GameNode *list)
{
    for (unsigned int i=0; i<mouseListeners.size(); i++)
    {
        if (mouseListeners[i] == list)
            mouseListeners.erase(mouseListeners.begin() + i);
    }
}
bool GameControl::mouseMoved( const OIS::MouseEvent &arg )
{
    for (unsigned int i=0; i<mouseListeners.size(); i++)
        mouseListeners[i]->mouseMoved(arg);

    return true;
}
bool GameControl::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    for (unsigned int i=0; i<mouseListeners.size(); i++)
        mouseListeners[i]->mousePressed(arg, id);

    return true;
}
bool GameControl::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    for (unsigned int i=0; i<mouseListeners.size(); i++)
        mouseListeners[i]->mouseReleased(arg, id);

    return true;
}

void GameControl::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}
void GameControl::windowClosed(Ogre::RenderWindow* rw)
{
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

void GameControl::setGameScene(GameScene *scene)
{
    if (gameScene) delete gameScene;

    removeAllListeners();

    gameScene = scene;
    gameScene->setupScene();
    createViewports();
    gameScene->loadSceneResources();

    gameScene->mSceneMgr->addRenderQueueListener(this);
}
