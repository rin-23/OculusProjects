// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "SharedTexturesWindow.h"

//----------------------------------------------------------------------------
SharedTexturesWindow::~SharedTexturesWindow ()
{
    mSharedTexture.reset();
    mOverlay.reset();
    delete mSecondEngine;
}
//----------------------------------------------------------------------------
SharedTexturesWindow::SharedTexturesWindow (Parameters& parameters)
    :
    Window(parameters),
    mTextColor(0.0f, 0.0f, 0.0f, 1.0f),
    mSecondEngine(nullptr),
    mOverlay(nullptr)
{
    // The first engine is mEngine, provided by the Window class.  Create a
    // second engine that owns a texture.  The access must be GPU_RW for
    // the texture to be shared by the first engine.
    int const tsize = 16;
    mSecondEngine = new DX11Engine();
    mSharedTexture.reset(new Texture2(DF_R8G8B8A8_UNORM, tsize, tsize));
    mSharedTexture->SetUsage(Resource::SHADER_OUTPUT);

    // Set the texture values to random numbers.
    char* data = mSharedTexture->GetData();
    std::mt19937 mte;
    std::uniform_int_distribution<int> rnd(0, 255);
    for (size_t i = 0; i < mSharedTexture->GetNumBytes(); ++i)
    {
        *data++ = static_cast<char>(rnd(mte));
    }

    // Specify the texture must be shared *before* telling DX11 to create
    // a GPU resource for it.
    mSharedTexture->MakeShared();
    mSecondEngine->Bind(mSharedTexture);

    // Tell the first engine to share the texture owned by the second engine.
    mEngine->Share(mSharedTexture, mSecondEngine);

    // The first engine will display the shared texture.
    mOverlay.reset(new OverlayEffect(mXSize, mYSize, tsize, tsize,
        SamplerState::MIN_L_MAG_L_MIP_P, SamplerState::CLAMP,
        SamplerState::CLAMP, true));
    mOverlay->SetTexture(mSharedTexture);
}
//----------------------------------------------------------------------------
void SharedTexturesWindow::OnIdle ()
{
    MeasureTime();

    mEngine->ClearBuffers();
    mEngine->Draw(mOverlay);
    DrawFrameRate(8, mYSize-8, mTextColor);
    mEngine->DisplayColorBuffer(0);

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
