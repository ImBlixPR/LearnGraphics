
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


//game header
#include "game/Game.h"



int main() {
    // Initialize your system
    BackEnd::Init();
    MK_ASSERT(BackEnd::GetWindowPointer(), "the window do not initilize!");
    OpenglRenderer::Init();
    OpenglRenderer::HotLoadShaders();
    //game

    if (!Game::Init()) return 0;
    while (BackEnd::WindowIsOpen())
    {   
        BackEnd::StartFrame();
        BackEnd::UpdateSubSystem();
        BackEnd::HandleSubSystem();
        //220, 215, 208
        glClearColor(0.859f, 0.839f, 0.812f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //166, 177, 209
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

        BackEnd::EndFrame();
    }
    Game::Clear();
    return 0;
}



