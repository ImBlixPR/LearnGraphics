
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
#include "scene/Scene.h"
#include "game/types/PickableObject.h"
#include "game/types/Object.h"
#include "game/interaction/PickableManager.h"
#include "game/interaction/DeleteObjectHandler.h"


int main() {
    // Initialize your system
    BackEnd::Init();
    MK_ASSERT(BackEnd::GetWindowPointer(), "the window do not initilize!");
    OpenglRenderer::Init();
    OpenglRenderer::HotLoadShaders();

    // Create some objects
    Object* obj1 = new Object("res/textures/fruit1.png", glm::vec2(100, 100), 0.0f, 50, 50);
    Object* obj2 = new Object("res/textures/fruit1.png", glm::vec2(200, 200), 0.0f, 80, 80);

    // Register objects with the PickableManager
    auto& pickManager = PickableManager::GetInstance();
    pickManager.RegisterObject(obj1);
    pickManager.RegisterObject(obj2);

    //trash init
    PickableTrash trash{ "res/textures/null.png", glm::vec2(800.0f, 300.0f), 100.0f, 100.0f };

    glm::vec2 tipPosition = glm::vec2(400.0f, 240.0f);

    while (BackEnd::WindowIsOpen())
    {
        static float currentTime = 0.0f;
        float duration = 0.1f; // 2 seconds to draw the circle
        // Increment time based on frame time
        currentTime += BackEnd::GetDeltaTime();

        // Create a repeating 0-1 value (for looping animation)
        float normalizedTime = fmodf(currentTime, 1.0f);

        BackEnd::StartFrame();
        BackEnd::UpdateSubSystem();
        BackEnd::HandleSubSystem();
        //220, 215, 208
        glClearColor(0.859f, 0.839f, 0.812f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //166, 177, 209
        Renderer::DotedBackground(MiskUtil::GetNormalizeRGBColor(166.0f, 177.0f, 209.0f), 4.0f);
        // Update pickable manager (handles all drag/drop logic)
        pickManager.Update();


        //animation 

        trash.DeleteByThrowInLocation(&pickManager);
        trash.Draw();
        pickManager.Draw();
        // Render objects
        Renderer::DrawCircle(
            glm::vec2(400.0f, 300.0f),      // Center position
            glm::vec3(1.0f, 0.0f, 0.0f), // Red color
            60.0f,                    // Radius
            1.0f,                     // Line thickness
            0.0f                      // Gap between quads
        );
        Renderer::OneWayArrow(
            glm::vec2(400.0f, 300.0f),
            tipPosition,
            1.0f
        );
        Scene::AnimateArrowAroundCircle(&tipPosition,
            glm::vec2(400, 300), 60.0f, normalizedTime);
        Renderer::OneWayArrow({ 20.0f, 20.0f }, { 100.0f, 400.0f });
        Renderer::DrawArc(glm::vec2(100, 100), glm::vec3(1.0f), 90.0f, 60.0f);
        BackEnd::EndFrame();
    }

    // Cleanup
    //delete obj1;
    //delete obj2;

    return 0;
}



