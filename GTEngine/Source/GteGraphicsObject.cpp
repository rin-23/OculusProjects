// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GTEnginePCH.h"
#include "GteGraphicsObject.h"
using namespace gte;

//----------------------------------------------------------------------------
GraphicsObject::~GraphicsObject()
{
    msLFDMutex.lock();
    {
        for (auto listener : msLFDSet)
        {
            listener->OnDestroy(this);
        }
    }
    msLFDMutex.unlock();
}
//----------------------------------------------------------------------------
GraphicsObject::GraphicsObject()
    :
    mType(GT_GRAPHICS_OBJECT)
{
}
//----------------------------------------------------------------------------
void GraphicsObject::SubscribeForDestruction(ListenerForDestruction* listener)
{
    msLFDMutex.lock();
    {
        msLFDSet.insert(listener);
    }
    msLFDMutex.unlock();
}
//----------------------------------------------------------------------------
void GraphicsObject::UnsubscribeForDestruction(ListenerForDestruction* listener)
{
    msLFDMutex.lock();
    {
        msLFDSet.erase(listener);
    }
    msLFDMutex.unlock();
}
//----------------------------------------------------------------------------

std::mutex GraphicsObject::msLFDMutex;
std::set<GraphicsObject::ListenerForDestruction*> GraphicsObject::msLFDSet;
