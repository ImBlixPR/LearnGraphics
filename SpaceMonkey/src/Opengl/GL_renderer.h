#pragma once

#include <glm/glm.hpp>


#include "shader/GL_Shader.h"
#include "types/GL_detachedMesh.h"
//#include "Enums/MiskEnums.h"


// Color enumeration with predefined colors
enum class ColorPreset
{
    Red,
    Green,
    Blue,
    Yellow,
    Cyan,
    Magenta,
    White,
    Black,
    Gray,
    Orange,
    Purple,
    Brown,
    Pink,
    Lime,
    Navy,
    Teal
};

enum class OriginPostion
{
    Center,
    Top_left,
    Top_right,
    Bottom_left,
    Bottom_right
};


namespace OpenglRenderer
{
	

    void Init();

	void HotLoadShaders();


	//shadpe renderer

    //quad render call
    void Render2dQuad(
        glm::vec2 position,                 // Position (top-left corner)
        ColorPreset colorPreset,            // Predefined color
        float width = 20.0f,                // Width of the quad
        float height = 20.0f,               // Height of the quad (can be different from width)
        float rotation = 0.0f,              // Rotation in degrees
        OriginPostion originPos = OriginPostion::Center,  // Origin point for rotation (0,0 is top-left, 0.5,0.5 is center)
        bool markOrigin = false
    );
    void Render2dQuad(
        glm::vec2 position,                 // Position (top-left corner)
        glm::vec3 color,                    // Custom RGB color
        float width = 20.0f,                // Width of the quad
        float height = 20.0f,               // Height of the quad (can be different from width)
        float rotation = 0.0f,              // Rotation in degrees
        OriginPostion originPos = OriginPostion::Center,  // Origin point for rotation (0,0 is top-left, 0.5,0.5 is center)
        bool markOrigin = false
    );
    void Render2dQuad(
        glm::vec2 position,                 // Position (top-left corner)
        glm::vec4 color,                    // Custom RGBA color
        float width = 20.0f,                // Width of the quad
        float height = 20.0f,               // Height of the quad (can be different from width)
        float rotation = 0.0f,              // Rotation in degrees
        OriginPostion originPos = OriginPostion::Center,  // Origin point for rotation (0,0 is top-left, 0.5,0.5 is center)
        bool markOrigin = false
    );

    //triangle Render call
    void Render2dTriangle(
        glm::vec2 position,
        glm::vec3 color,
        float width = 20.0f,
        float height = 20.0f,
        float rotation = 0.0f,
        bool markOrigin = false
    );

    void Render2dEquilateralTriangle(
        glm::vec2 position,
        glm::vec3 color,
        float width = 20.0f,
        float height = 20.0f,
        float rotation = 0.0f,
        bool markOrigin = false
    );

    //circle render call
    void Render2dCircle(
        glm::vec2 position,
        ColorPreset colorPreset,
        float radius = 20.0f,
        float rotation = 0.0f,
        int segments = 32,
        bool markOrigin = false
    );

    void Render2dCircle(
        glm::vec2 position,
        glm::vec3 color,
        float radius = 20.0f,
        float rotation = 0.0f,
        int segments = 32,
        bool markOrigin = false
    );
    void Render2dCircle(
        glm::vec2 position,
        glm::vec4 color,
        float radius = 20.0f,
        float rotation = 0.0f,
        int segments = 32,
        bool markOrigin = false
    );

    //Mesh render call
    void Render2dMesh(
        OpenGLDetachedMesh* mesh,
        glm::vec2 position,
        glm::vec3 color,
        float width = 20.0f,
        float height = 20.0f,
        float rotation = 0.0f,
        bool markOrigin = false
    );

    //texture mesh render call
    void Render2dTexture(
        unsigned int textureId,
        glm::vec2 position,
        float width = 20.0f,                // Width of the quad
        float height = 20.0f,               // Height of the quad (can be different from width)
        float rotation = 0.0f,              // Rotation in degrees
        OriginPostion originPos = OriginPostion::Center,
        bool markOrigin = false
    );

    void Render2dTexture(
        unsigned int textureId,
        glm::vec2 position,
        glm::vec3 colorTint,
        float width = 20.0f,                // Width of the quad
        float height = 20.0f,               // Height of the quad (can be different from width)
        float rotation = 0.0f,              // Rotation in degrees
        OriginPostion originPos = OriginPostion::Center,
        bool markOrigin = false
    );
}