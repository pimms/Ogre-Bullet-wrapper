#include "StdAfx.h"
#include "PimEngine.h"

Ogre::Vector3 toOgre(const btVector3 &vec)
{
    return Ogre::Vector3(vec.x(), vec.y(), vec.z());
}
Ogre::Quaternion toOgre(const btQuaternion &q)
{
    return Ogre::Quaternion(q.w(), q.x(), q.y(), q.z());
}

btVector3 toBullet(const Ogre::Vector3 &vec)
{
    return btVector3(vec.x, vec.y, vec.z);
}
btQuaternion toBullet(const Ogre::Quaternion &q)
{
    return btQuaternion(q.x, q.y, q.z, q.w);
}