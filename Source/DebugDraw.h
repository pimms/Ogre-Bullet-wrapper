#pragma once
#include "linearmath\btidebugdraw.h"
#include "PimEngine.h"

class DebugDraw :
    public btIDebugDraw
{
public:
    void drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &color);
    void drawBox (const btVector3 &bbMin, const btVector3 &bbMax, const btVector3 &color);
    void reportErrorWarning (const char *warningString);

    // UNIMPLEMENTED
    void drawContactPoint (const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
    {}
    void draw3dText (const btVector3 &location, const char *textString)
    {}
    void setDebugMode (int debugMode)
    {}
    int getDebugMode () const
    {
        return btIDebugDraw::DBG_DrawWireframe;
    }
};

