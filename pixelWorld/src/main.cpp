
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


// Smooth step function (cubic)
float smoothStep(float t) {
    // Clamp t to 0-1 range
    t = glm::clamp(t, 0.0f, 1.0f);
    // Apply cubic smoothing: 3t² - 2t³
    return t * t * (3.0f - 2.0f * t);
}

// Smoother step function (quintic)
float smootherStep(float t) {
    // Clamp t to 0-1 range
    t = glm::clamp(t, 0.0f, 1.0f);
    // Apply quintic smoothing: 6t⁵ - 15t⁴ + 10t³
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

// Smooth lerp using cubic easing
float smoothLerp(float a, float b, float t) {
    return glm::mix(a, b, smoothStep(t));
}
glm::vec2 smoothLerp(glm::vec2 a, glm::vec2 b, float t) {
    return glm::mix(a, b, smoothStep(t));
}

// Smoother lerp using quintic easing
float smootherLerp(float a, float b, float t) {
    return glm::mix(a, b, smootherStep(t));
}
glm::vec2 smootherLerp(glm::vec2 a, glm::vec2 b, float t) {
    return glm::mix(a, b, smootherStep(t));
}

// Time-based smooth easing function with customizable curve
float timeEase(float currentTime, float duration, float easingPower = 2.0f) {
    // Get normalized time (0 to 1)
    float t = glm::clamp(currentTime / duration, 0.0f, 1.0f);

    // Apply easing based on power (higher = sharper curve)
    if (easingPower == 1.0f) {
        return t; // Linear (no easing)
    }
    else if (easingPower == 2.0f) {
        return smoothStep(t); // Cubic easing
    }
    else if (easingPower == 3.0f) {
        return smootherStep(t); // Quintic easing
    }
    else if (easingPower > 0.0f) {
        // Custom power-based easing for ease-in
        return powf(t, easingPower);
    }
    else {
        // Negative power for ease-out
        return 1.0f - powf(1.0f - t, -easingPower);
    }
}

// Time-based lerp with various easing options
float timeLerp(float start, float end, float currentTime, float duration, float easingPower = 2.0f) {
    return glm::mix(start, end, timeEase(currentTime, duration, easingPower));
}


//temp 
glm::vec2 cirlePos = { 100.0f, 100.0f };
int main() {
    // Initialize your system
    BackEnd::Init();
    MK_ASSERT(BackEnd::GetWindowPointer(), "the window do not initilize!");
    OpenglRenderer::Init();
    OpenglRenderer::HotLoadShaders();

    // Create some objects
    Object* obj1 = new Object("res/textures/fruit1.png", glm::vec2(100, 100), 0.0f, 50, 50);
    Object* obj2 = new Object("res/textures/fruit1.png", glm::vec2(200, 200), 0.0f, 80, 80);
    Object* obj3 = new Object(glm::vec3(1.0f, 0.0f, 0.0f), cirlePos, 0.0f, 80, 80);

    // Register objects with the PickableManager
    auto& pickManager = PickableManager::GetInstance();
    pickManager.RegisterObject(obj1);
    pickManager.RegisterObject(obj2);
    //pickManager.RegisterObject(obj3);

    //trash init
    PickableTrash trash{ "res/textures/null.png", glm::vec2(800.0f, 300.0f), 100.0f, 100.0f };

    glm::vec2 tipPosition = glm::vec2(400.0f, 240.0f);


    float currentAngle = 0.0f;
    float finalAngle = 360.0f;
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

        //temp 
        cirlePos = smoothLerp(cirlePos, Input::GetMousePos(), 0.3f);
        obj3->SetPosition(cirlePos);
        //animation 
        float t = normalizedTime * normalizedTime * normalizedTime;
        //currentAngle = glm::mix(0.0f, finalAngle, smootherStep(normalizedTime));
        //currentAngle = glm::mix(0.0f, finalAngle, normalizedTime );
        currentAngle = timeLerp(0.0f, 360.0f, currentTime, 10.0f, 3.0f);
        //printf("the current angle = %.3f\n", currentAngle);
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
        Renderer::DrawArc({ 400,100 }, glm::vec3(0.8f, 0.0f, 0.0f), 120.0f);
        Renderer::DrawArcWithSrartAngle(glm::vec2(300, 200), glm::vec3(1.0f), 270.0f, currentAngle, 90.0f, 5.0f, 0.0f, 32);
       
        obj3->Draw();
        
        BackEnd::EndFrame();
    }

    // Cleanup
    //delete obj1;
    //delete obj2;

    return 0;
}



