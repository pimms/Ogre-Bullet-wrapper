#pragma once

#include "PimEngine.h"

class HeightfieldData
{
public:
    btScalar *data;
    int w;
    int h;

    HeightfieldData()
    {
        data=NULL;
        w=0;
        h=0;
    }
    ~HeightfieldData()
    {
        if (data)
            delete data;
    }
};