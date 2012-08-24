#PimEngine v0.1 readme

PimEngine is a library aiming to close the gap between Ogre3D and Bullet Physics,
and at the same time provide a logical and simple interface for game logic. As Ogre3D
already implements a familiar parent-child-node hierarchy, PimEngine will expand this 
relationship. Obviously, Ogre3D and Bullet are still directly available to use, but
the most common used techniques in these libraries will be handled automatically
by PimEngine. More specific features (currently including shaders, materials) must
be handled manually by the user, but more simplistic methods of using these features
will be implemented in the future.

PimEngine is currently in an early stage, and should not be used by anyone for 
any purposes other than testing or contributing. It's not completed. Not even a little bit.
If you want to make a game using Ogre and Bullet, <b>use Ogre and Bullet!</b>

PimEngine isn't, and probably never will be, guaranteed to work. It is a delicate 
little flower, and you must be super nice to it.

#Setup

Currently, PimEngine is only supported under a Windows environment. 

This tutorial assumes that you have some knowledge of linking, setting header paths
and otherwise working your way around your favorite IDE. 

<b>Step 1 - Getting the library</b>

Download the precompiled PimEngine binaries. Extract them at a position of your liking.
Remember this position. 

If you haven't already, you have to download and install Ogre3D and Bullet by yourself.

<a href="http://www.ogre3d.org/download">Ogre download</a><br />
<a href="http://code.google.com/p/bullet/downloads/list">Bullet download</a>

PimEngine requires you to link with Ogre and Bullet manually.

<b>Step 2 - Header search paths</b>

<u>PLEASE NOTE:</u> Variable names must be replaced with the top level folder path if you haven't
set these variables to be system wide by yourself. Note that $(Configuration) should be replaced
with either "Release" or "Debug", depending on your build type. $(Configuration) is set automatically
in Visual Studio.

It's important that your project has it's search paths set <b>*exactly*</b> as described
in the following sections. Otherwise, stuff won't be found. And we need stuff to make other
stuff work.

Ogre's header search paths:
	$(OGRE_HOME)\include
	$(OGRE_HOME)\include\ogre
	$(OGRE_HOME)\boost
	
...and Bullet Physic's header search paths:
	$(BULLET_HOME)\src
	
Obviously, the PimEngine headers would be nice to find as well.
	$(PIMENG_HOME)\include
	
<b>Step 3 - Library search paths</b>

Ogre library search paths:
	$(OGRE_HOME)\lib\$(Configuration)
	$(ORGE_HOME)\boost\lib
	$(OGRE_HOME)\dependencies\lib

Bullet Library search paths:
	$(BULLET_HOME)\lib

PimEngine library search paths:
	$(PIMENG_HOME)\bin
	
<b>Step 4 - Linking</b>

It's important to notice that if you want any extra features from Ogre or Bullet,
simply link with these libraries as well. You may have to set some extra search paths,
but if you need these features, you'll know how to figure it out. This list is merely
the *required* libraries.

You may be required to link to the Bullet libraries manually as well, depending on your
setup.

<u>Release configuration</u>
	PimEngine.lib
	OgreMain.lib
	OIS.lib
	OpenGL32.lib
	
<u>Debug configuration</u>
	PimEngine_d.lib
	OgreMain.lib
	OIS_d.lib
	OpenGL32.lib

<b>Step 5 - DLLs and config files</b>

As PimEngine is linking Ogre dynamically, you have to include the DLLs, and the config
files. Head on over to $(OGRE_HOME)\bin\$(Configuration) and copy all of the DLLs if you're
unsure of exactly which you need. I know I am. Also, copy "resources.cfg" and "plugins.cfg".
Put the files where your binaries will be built.

<b>Step 6 - Making sure stuff went right</b>

In your project, replace your main file with this piece of code:

	#include "pimengine.h"
	
	class TestScene : public GameScene
	{
		void loadSceneResources() { std::cout<<"Sup\n"; }
	};
	
	int main(int argc, char** argv[])
	{
		GameControl *gc = new GameControl(new TestScene);
		try 
		{
			gc->go();
		}
		catch( Ogre::Exception &e)
		{
			std::cout<<"An exception has occured!\n" <<e.getFullDescription() <<"\n";
			getch();
		}
		return 0;
	}
	
Unless I have forgotten to mention something, or you have done something wrong, a screen should
appear. If not, check your search paths and library linkings.