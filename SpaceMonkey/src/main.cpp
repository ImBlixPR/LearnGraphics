
// Prevent accidentally selecting integrated GPU
extern "C" {
	__declspec(dllexport) unsigned __int32 AmdPowerXpressRequestHighPerformance = 0x1;
	__declspec(dllexport) unsigned __int32 NvOptimusEnablement = 0x1;
}

#include <iostream>


#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>


#include "BackEnd/BackEnd.h"
#include "BackEnd/Log.h"
#include "Opengl/GL_renderer.h"
#include "Opengl/types/GL_detachedMesh.h"
#include "AssetManger/AssetManger.h"
#include "utility/MeskUtil.h"
#include "Input/Input.h"
#include "renderer/Renderer.h"
#include "Opengl/types/GL_frameBuffer.h"
#include "postProcessing/Effects/VHS.h"
#include "postProcessing/Effects/CRT.h"


//game header
#include "game/Game.h"

enum class EffectType {
    NONE,
    CRT,
    VHS
};

EffectType currentEffect = EffectType::CRT;


int main() {
    // Initialize your system
    BackEnd::Init();
    MK_ASSERT(BackEnd::GetWindowPointer(), "the window do not initilize!");
    OpenglRenderer::Init();
    OpenglRenderer::HotLoadShaders();

    //frame buffer
    OpenglFrameBuffer frameBuffer;
    frameBuffer.ConfigFrameBuffer();
    ScreenEffect* crt = new CRT();
    ScreenEffect* vhs = new VHS();

    //game

    if (!Game::Init()) return 0;
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (BackEnd::WindowIsOpen())
    {   
        BackEnd::StartFrame();
        BackEnd::UpdateSubSystem();
        BackEnd::HandleSubSystem();

        if (Input::KeyPressed(MK_KEY_L))
        {
            crt->HotReload();
            vhs->HotReload();

        }

        // Switch between effects
        if (Input::KeyPressed(MK_KEY_1))
            currentEffect = EffectType::NONE;
        else if (Input::KeyPressed(MK_KEY_2))
            currentEffect = EffectType::CRT;
        else if (Input::KeyPressed(MK_KEY_3))
            currentEffect = EffectType::VHS;

        frameBuffer.Bind();
        //220, 215, 208
        glClearColor(0.859f, 0.839f, 0.812f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //166, 177, 209
        glDisable(GL_DEPTH_TEST);

        Renderer::DotedBackground(MiskUtil::GetNormalizeRGBColor(166.0f, 177.0f, 209.0f), 2.0f, 14, 14);

        // Modified to always call Menu() first for the transition
        if (!Game::IsRunning())
            Game::Menu();
        else
        {
            //printf("work here\n");
            Game::Run(BackEnd::GetDeltaTime());
        }

        Game::UpdateTransition(BackEnd::GetDeltaTime());

        switch (currentEffect)
        {
        case EffectType::NONE:
            // No effect applied
            frameBuffer.ApplyNormalShader();
            break;
        case EffectType::CRT:
            crt->ApplyEffect();
            break;
        case EffectType::VHS:
            vhs->ApplyEffect();
            break;
        }

        frameBuffer.Draw();
        BackEnd::EndFrame();
    }
    Game::Clear();
    delete crt;
    delete vhs;
    return 0;
}



