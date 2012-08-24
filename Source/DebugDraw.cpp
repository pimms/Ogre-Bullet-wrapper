#include "StdAfx.h"
#include "DebugDraw.h"


void DebugDraw::drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    glColor3f(color.x(), color.y(), color.z());
    glBegin(GL_LINES);
    glVertex3f(from.x(), from.y(), from.z());
    glVertex3f(to.x(), to.y(), to.z());
    glEnd();
}
void DebugDraw::drawBox (const btVector3 &bbMin, const btVector3 &bbMax, const btVector3 &color)
{
    glColor3f(color.x(), color.y(), color.z());
    glBegin(GL_LINE_LOOP);
    glVertex3f(bbMin.x(), bbMin.y(), bbMin.z());
    glVertex3f(bbMax.x(), bbMin.y(), bbMin.z());
    glVertex3f(bbMax.x(), bbMin.y(), bbMax.z());
    glVertex3f(bbMin.x(), bbMin.y(), bbMax.z());
    glVertex3f(bbMin.x(), bbMax.y(), bbMin.z());
    glVertex3f(bbMax.x(), bbMax.y(), bbMin.z());
    glVertex3f(bbMax.x(), bbMax.y(), bbMax.z());
    glVertex3f(bbMin.x(), bbMax.y(), bbMax.z());
    glEnd();
}
void DebugDraw::reportErrorWarning(const char *warningString)
{
    Ogre::LogManager::getSingleton().logMessage("BULLET ERROR:");
    Ogre::LogManager::getSingleton().logMessage(warningString);
}
