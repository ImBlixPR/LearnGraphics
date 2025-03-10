#include "GL_renderer.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>


#include "ShapeInfo.h"
#include "BackEnd/BackEnd.h"

// Color mapping function
glm::vec3 GetColorFromPreset(ColorPreset preset)
{
    switch (preset)
    {
    case ColorPreset::Red:      return glm::vec3(1.0f, 0.0f, 0.0f);
    case ColorPreset::Green:    return glm::vec3(0.0f, 1.0f, 0.0f);
    case ColorPreset::Blue:     return glm::vec3(0.0f, 0.0f, 1.0f);
    case ColorPreset::Yellow:   return glm::vec3(1.0f, 1.0f, 0.0f);
    case ColorPreset::Cyan:     return glm::vec3(0.0f, 1.0f, 1.0f);
    case ColorPreset::Magenta:  return glm::vec3(1.0f, 0.0f, 1.0f);
    case ColorPreset::White:    return glm::vec3(1.0f, 1.0f, 1.0f);
    case ColorPreset::Black:    return glm::vec3(0.0f, 0.0f, 0.0f);
    case ColorPreset::Gray:     return glm::vec3(0.5f, 0.5f, 0.5f);
    case ColorPreset::Orange:   return glm::vec3(1.0f, 0.5f, 0.0f);
    case ColorPreset::Purple:   return glm::vec3(0.5f, 0.0f, 0.5f);
    case ColorPreset::Brown:    return glm::vec3(0.6f, 0.3f, 0.1f);
    case ColorPreset::Pink:     return glm::vec3(1.0f, 0.7f, 0.7f);
    case ColorPreset::Lime:     return glm::vec3(0.7f, 1.0f, 0.0f);
    case ColorPreset::Navy:     return glm::vec3(0.0f, 0.0f, 0.5f);
    case ColorPreset::Teal:     return glm::vec3(0.0f, 0.5f, 0.5f);
    default:                    return glm::vec3(1.0f, 1.0f, 1.0f); // Default to white
    }
}

glm::vec2 GetOriginPostion(OriginPostion postion)
{
    switch (postion)
    {
    case OriginPostion::Center:         return glm::vec2(0.5f, 0.5f);
    case OriginPostion::Top_left:       return glm::vec2(0.0f, 0.0f);
    case OriginPostion::Top_right:      return glm::vec2(1.0f, 0.0f);
    case OriginPostion::Bottom_left:    return glm::vec2(0.0f, 1.0f);
    case OriginPostion::Bottom_right:   return glm::vec2(1.0f, 1.0f);
    default:                            return glm::vec2(0.5f, 0.5f);
    }
}

namespace OpenglRenderer
{

	//gloable varible
	struct Shaders
	{
		Shader triangles;
		Shader texturedMesh;
	}g_shaders;

	glm::mat4 g_orthgraphicCamera;
    struct ShapeMesh
    {
        OpenGLDetachedMesh quad2d;
        OpenGLDetachedMesh triangle2d;
        OpenGLDetachedMesh equilateralTriangle2d;
        OpenGLDetachedMesh circle2d;
    }g_shapeMesh;
	

    void Init()
    {
        // Set up orthographic projection with (0,0) at top-left
        g_orthgraphicCamera = glm::ortho(
            0.0f, static_cast<float>(BackEnd::GetWindowCurrentWidth()),
            static_cast<float>(BackEnd::GetWindowCurrentHeight()), 0.0f,  // Y flipped to make top = 0
            -1.0f, 1.0f
        );

        g_shapeMesh.quad2d.UpdateVertexBuffer(ShapeInfo::Quad2d::vertices, ShapeInfo::Quad2d::indices);
        g_shapeMesh.triangle2d.UpdateVertexBuffer(ShapeInfo::Triangle2d::vertices, ShapeInfo::Triangle2d::indices);
        g_shapeMesh.equilateralTriangle2d.UpdateVertexBuffer(ShapeInfo::EquilateralTriangle2d::vertices, ShapeInfo::EquilateralTriangle2d::indices);
    }
    void HotLoadShaders()
	{
		std::cout << "Hotloading shaders...\n";

		g_shaders.triangles.Load("GL_triangles.vert", "GL_triangles.frag");
        g_shaders.texturedMesh.Load("GL_textureMesh.vert", "GL_textureMesh.frag");

	}

    // Enhanced Render2dQuad function with more flexibility
    void Render2dQuad(
        glm::vec2 position,
        ColorPreset colorPreset,
        float width, 
        float height,
        float rotation,
        OriginPostion originPos, 
        bool markOrigin
    )
    {
        //g_quad2d.UpdateVertexBuffer(ShapeInfo::Quad2d::vertices, ShapeInfo::Quad2d::indices);
        g_shaders.triangles.Use();
        g_shaders.triangles.SetMat4("projection", g_orthgraphicCamera);
        g_shaders.triangles.SetMat4("view", glm::mat4(1.0f));

        // Get color from preset
        glm::vec3 color = GetColorFromPreset(colorPreset);

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec2 origin = GetOriginPostion(originPos);
        // Order of transformations:
        // 1. Translate to position
        model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));

        // 2. Translate to rotation origin
        model = glm::translate(model, glm::vec3(width * origin.x, height * origin.y, 0.0f));

        // 3. Apply rotation
        if (rotation != 0.0f)
        {
            model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        }

        // 4. Translate back from rotation origin
        model = glm::translate(model, glm::vec3(-width * origin.x, -height * origin.y, 0.0f));

        // 5. Apply scale
        model = glm::scale(model, glm::vec3(width, height, 1.0f));

        glm::vec2 originPosition = glm::vec2(width * origin.x + position.x, height * origin.y + position.y);

        g_shaders.triangles.SetMat4("model", model);
        g_shaders.triangles.SetVec3("color", color);
        glBindVertexArray(g_shapeMesh.quad2d.GetVAO());
        glDrawElements(GL_TRIANGLES, g_shapeMesh.quad2d.GetIndexCount(), GL_UNSIGNED_INT, 0);
        
        if (markOrigin)
        {
            Render2dQuad(originPosition, ColorPreset::Green, 20.0f,
                20.0f, 0.0f, OriginPostion::Center, false);
        }
    }

    // Overloaded version that accepts explicit RGB color values
    void Render2dQuad(
        glm::vec2 position,
        glm::vec3 color,
        float width,
        float height,
        float rotation,
        OriginPostion originPos,
        bool markOrigin
    )
    {
        g_shaders.triangles.Use();
        g_shaders.triangles.SetMat4("projection", g_orthgraphicCamera);
        g_shaders.triangles.SetMat4("view", glm::mat4(1.0f));

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec2 origin = GetOriginPostion(originPos);
        // Apply transformations in the same order as the other function
        model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
        model = glm::translate(model, glm::vec3(width * origin.x, height * origin.y, 0.0f));
        if (rotation != 0.0f)
        {
            model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        model = glm::translate(model, glm::vec3(-width * origin.x, -height * origin.y, 0.0f));
        model = glm::scale(model, glm::vec3(width, height, 1.0f));

        glm::vec2 originPosition = glm::vec2(width * origin.x + position.x,height * origin.y+position.y);

        g_shaders.triangles.SetMat4("model", model);
        g_shaders.triangles.SetVec3("color", color);
        glBindVertexArray(g_shapeMesh.quad2d.GetVAO());
        glDrawElements(GL_TRIANGLES, g_shapeMesh.quad2d.GetIndexCount(), GL_UNSIGNED_INT, 0);

        if (markOrigin)
        {
            Render2dQuad(originPosition, ColorPreset::Green, 2.0f,
                2.0f, 0.0f, OriginPostion::Center, false);
        }
    }


    //triangle Render call
    void Render2dTriangle(
        glm::vec2 position,
        glm::vec3 color,
        float width,
        float height,
        float rotation,
        bool markOrigin
    )
    {
        float x = 0, y = 0;
        for (Vertex ver : ShapeInfo::Triangle2d::vertices)
        {
            x += ver.position.x;
            y += ver.position.y;
        }

        g_shaders.triangles.Use();
        g_shaders.triangles.SetMat4("projection", g_orthgraphicCamera);
        g_shaders.triangles.SetMat4("view", glm::mat4(1.0f));

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);

        //glm::vec2 origin = GetOriginPostion(originPos) - glm::vec2(0.25f, 0.25f);
        glm::vec2 origin = glm::vec2(x/3, y/3);
        
        // Apply transformations in the same order as the other function
        model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
        model = glm::translate(model, glm::vec3(width * origin.x, height * origin.y, 0.0f));
        if (rotation != 0.0f)
        {
            model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        model = glm::translate(model, glm::vec3(-width * origin.x, -height * origin.y, 0.0f));
        model = glm::scale(model, glm::vec3(width, height, 1.0f));

        glm::vec2 originPosition = glm::vec2(width * origin.x + position.x, height * origin.y + position.y);// -glm::vec2((float)width / 2, (float)height / 2);

        g_shaders.triangles.SetMat4("model", model);
        g_shaders.triangles.SetVec3("color", color);
        glBindVertexArray(g_shapeMesh.triangle2d.GetVAO());
        glDrawElements(GL_TRIANGLES, g_shapeMesh.triangle2d.GetIndexCount(), GL_UNSIGNED_INT, 0);

        if (markOrigin)
        {
            Render2dQuad(originPosition, ColorPreset::Green, 2.0f,
                2.0f, 0.0f, OriginPostion::Center, false);
        }
    }

    void Render2dEquilateralTriangle(
        glm::vec2 position,
        glm::vec3 color,
        float width,
        float height,
        float rotation,
        bool markOrigin
    )
    {
        float x = 0, y = 0;
        for (Vertex ver : ShapeInfo::EquilateralTriangle2d::vertices)
        {
            x += ver.position.x;
            y += ver.position.y;
        }

        g_shaders.triangles.Use();
        g_shaders.triangles.SetMat4("projection", g_orthgraphicCamera);
        g_shaders.triangles.SetMat4("view", glm::mat4(1.0f));

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);

        //glm::vec2 origin = GetOriginPostion(originPos) - glm::vec2(0.25f, 0.25f);
        glm::vec2 origin = glm::vec2(x / 3, y / 3);

        // Apply transformations in the same order as the other function
        model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
        model = glm::translate(model, glm::vec3(width * origin.x, height * origin.y, 0.0f));
        if (rotation != 0.0f)
        {
            model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        model = glm::translate(model, glm::vec3(-width * origin.x, -height * origin.y, 0.0f));
        model = glm::scale(model, glm::vec3(width, height, 1.0f));

        glm::vec2 originPosition = glm::vec2(width * origin.x + position.x, height * origin.y + position.y);// -glm::vec2((float)width / 2, (float)height / 2);

        g_shaders.triangles.SetMat4("model", model);
        g_shaders.triangles.SetVec3("color", color);
        glBindVertexArray(g_shapeMesh.equilateralTriangle2d.GetVAO());
        glDrawElements(GL_TRIANGLES, g_shapeMesh.equilateralTriangle2d.GetIndexCount(), GL_UNSIGNED_INT, 0);

        if (markOrigin)
        {
            Render2dQuad(originPosition, ColorPreset::Green, 2.0f,
                2.0f, 0.0f, OriginPostion::Center, false);
        }
    }


    //circle render call
    // Circle rendering function
    void Render2dCircle(
        glm::vec2 position,
        ColorPreset colorPreset,
        float radius,
        float rotation,
        int segments,
        bool markOrigin
    )
    {
        // Get color from preset
        glm::vec3 color = GetColorFromPreset(colorPreset);

        // If we want a custom number of segments, regenerate the geometry
        if (segments != 32) {
            g_shapeMesh.circle2d.UpdateVertexBuffer(
                ShapeInfo::Circle2d::GetVertices(segments),
                ShapeInfo::Circle2d::GetIndices(segments)
            );
        }
        else {
            g_shapeMesh.circle2d.UpdateVertexBuffer(
                ShapeInfo::Circle2d::vertices,
                ShapeInfo::Circle2d::indices
            );
        }

        g_shaders.triangles.Use();
        g_shaders.triangles.SetMat4("projection", g_orthgraphicCamera);
        g_shaders.triangles.SetMat4("view", glm::mat4(1.0f));

        // Get origin position
        glm::vec2 origin = GetOriginPostion(OriginPostion::Top_left);

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);

        // Apply transformations
        model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
        model = glm::translate(model, glm::vec3(radius * origin.x, radius * origin.y, 0.0f));
        if (rotation != 0.0f)
        {
            model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        model = glm::translate(model, glm::vec3(-radius * origin.x, -radius * origin.y, 0.0f));
        model = glm::scale(model, glm::vec3(radius * 2.0f, radius * 2.0f, 1.0f)); // Scale by diameter

        // Calculate the position of the origin point for marking
        glm::vec2 originPosition = glm::vec2(radius * origin.x + position.x, radius * origin.y + position.y);

        g_shaders.triangles.SetMat4("model", model);
        g_shaders.triangles.SetVec3("color", color);
        glBindVertexArray(g_shapeMesh.circle2d.GetVAO());
        glDrawElements(GL_TRIANGLES, g_shapeMesh.circle2d.GetIndexCount(), GL_UNSIGNED_INT, 0);

        // Optionally mark the origin point
        if (markOrigin)
        {
            Render2dQuad(originPosition, ColorPreset::Green, 2.0f, 2.0f, 0.0f, OriginPostion::Center, false);
        }
    }

    // Overloaded version that accepts explicit RGB color values
    void Render2dCircle(
        glm::vec2 position,
        glm::vec3 color,
        float radius,
        float rotation,
        int segments,
        bool markOrigin
    )
    {
        // If we want a custom number of segments, regenerate the geometry
        if (segments != 32) {
            g_shapeMesh.circle2d.UpdateVertexBuffer(
                ShapeInfo::Circle2d::GetVertices(segments),
                ShapeInfo::Circle2d::GetIndices(segments)
            );
        }
        else {
            g_shapeMesh.circle2d.UpdateVertexBuffer(
                ShapeInfo::Circle2d::vertices,
                ShapeInfo::Circle2d::indices
            );
        }

        g_shaders.triangles.Use();
        g_shaders.triangles.SetMat4("projection", g_orthgraphicCamera);
        g_shaders.triangles.SetMat4("view", glm::mat4(1.0f));

        // Get origin position
        glm::vec2 origin = GetOriginPostion(OriginPostion::Top_left);

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);

        // Apply transformations
        model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
        model = glm::translate(model, glm::vec3(radius * origin.x, radius * origin.y, 0.0f));
        if (rotation != 0.0f)
        {
            model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        model = glm::translate(model, glm::vec3(-radius * origin.x, -radius * origin.y, 0.0f));
        model = glm::scale(model, glm::vec3(radius * 2.0f, radius * 2.0f, 1.0f)); // Scale by diameter

        // Calculate the position of the origin point for marking
        glm::vec2 originPosition = glm::vec2(radius * origin.x + position.x, radius * origin.y + position.y);

        g_shaders.triangles.SetMat4("model", model);
        g_shaders.triangles.SetVec3("color", color);

        glBindVertexArray(g_shapeMesh.circle2d.GetVAO());
        glDrawElements(GL_TRIANGLE_FAN, g_shapeMesh.circle2d.GetIndexCount(), GL_UNSIGNED_INT, 0);
        // Optionally mark the origin point
        if (markOrigin)
        {
            Render2dQuad(originPosition, ColorPreset::Green, 2.0f, 2.0f, 0.0f, OriginPostion::Center, false);
        }
    }

    void Render2dMesh(
        OpenGLDetachedMesh* mesh,
        glm::vec2 position,
        glm::vec3 color,
        float width,
        float height,
        float rotation,
        bool markOrigin
    )
    {
        float x = 0, y = 0;
        for (Vertex ver :mesh->GetVerices())
        {
            x += ver.position.x;
            y += ver.position.y;
        }


        g_shaders.triangles.Use();
        g_shaders.triangles.SetMat4("projection", g_orthgraphicCamera);
        g_shaders.triangles.SetMat4("view", glm::mat4(1.0f));

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec2 origin = glm::vec2(x / mesh->GetVertexCount(), y / mesh->GetVertexCount());
        // Apply transformations in the same order as the other function
        model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
        model = glm::translate(model, glm::vec3(width * origin.x, height * origin.y, 0.0f));
        if (rotation != 0.0f)
        {
            model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        model = glm::translate(model, glm::vec3(-width * origin.x, -height * origin.y, 0.0f));
        model = glm::scale(model, glm::vec3(width, height, 1.0f));

        glm::vec2 originPosition = glm::vec2(width * origin.x + position.x, height * origin.y + position.y);

        g_shaders.triangles.SetMat4("model", model);
        g_shaders.triangles.SetVec3("color", color);
        glBindVertexArray(mesh->GetVAO());
        glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);

        if (markOrigin)
        {
            Render2dQuad(originPosition, ColorPreset::Green, 2.0f,
                2.0f, 0.0f, OriginPostion::Center, false);
        }
    }

    void Render2dTexture(
        unsigned int textureId,
        glm::vec2 position,
        float width,
        float height,
        float rotation,
        OriginPostion originPos,
        bool markOrigin
    )
    {
        // Ensure the texture shader is using the correct texture unit
        g_shaders.texturedMesh.Use();
        g_shaders.texturedMesh.SetInt("textureSampler", 0);  // Explicitly set texture unit

        // Projection and view setup
        g_shaders.texturedMesh.SetMat4("projection", g_orthgraphicCamera);
        g_shaders.texturedMesh.SetMat4("view", glm::mat4(1.0f));

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec2 origin = GetOriginPostion(originPos);

        // Apply transformations in the same order as other rendering functions
        model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
        model = glm::translate(model, glm::vec3(width * origin.x, height * origin.y, 0.0f));

        if (rotation != 0.0f)
        {
            model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        }

        model = glm::translate(model, glm::vec3(-width * origin.x, -height * origin.y, 0.0f));
        model = glm::scale(model, glm::vec3(width, height, 1.0f));

        // Calculate origin position for marking
        glm::vec2 originPosition = glm::vec2(width * origin.x + position.x, height * origin.y + position.y);

        // Set model matrix
        g_shaders.texturedMesh.SetMat4("model", model);

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Draw the textured quad
        glBindVertexArray(g_shapeMesh.quad2d.GetVAO());
        glDrawElements(GL_TRIANGLES, g_shapeMesh.quad2d.GetIndexCount(), GL_UNSIGNED_INT, 0);

        // Optional origin marker
        if (markOrigin)
        {
            Render2dQuad(originPosition, ColorPreset::Green, 10.0f, 10.0f, 0.0f, OriginPostion::Center, false);
        }
    }

    // Optional: Overloaded version with color tint
    void Render2dTexture(
        unsigned int textureId,
        glm::vec2 position,
        glm::vec3 colorTint,
        float width,
        float height,
        float rotation,
        OriginPostion originPos,
        bool markOrigin
    )
    {
        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        g_shaders.texturedMesh.Use();
        g_shaders.texturedMesh.SetInt("textureSampler", 0);  // Explicitly set texture unit
        g_shaders.texturedMesh.SetVec3("colorTint", colorTint);  // Add color tint support


        // Projection and view setup
        g_shaders.texturedMesh.SetMat4("projection", g_orthgraphicCamera);
        g_shaders.texturedMesh.SetMat4("view", glm::mat4(1.0f));

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec2 origin = GetOriginPostion(originPos);

        // Apply transformations in the same order as other rendering functions
        model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
        model = glm::translate(model, glm::vec3(width * origin.x, height * origin.y, 0.0f));

        if (rotation != 0.0f)
        {
            model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        }

        model = glm::translate(model, glm::vec3(-width * origin.x, -height * origin.y, 0.0f));
        model = glm::scale(model, glm::vec3(width, height, 1.0f));

        // Calculate origin position for marking
        glm::vec2 originPosition = glm::vec2(width * origin.x + position.x, height * origin.y + position.y);

        // Set model matrix
        g_shaders.texturedMesh.SetMat4("model", model);

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Draw the textured quad
        glBindVertexArray(g_shapeMesh.quad2d.GetVAO());
        glDrawElements(GL_TRIANGLES, g_shapeMesh.quad2d.GetIndexCount(), GL_UNSIGNED_INT, 0);

        // Optional origin marker
        if (markOrigin)
        {
            Render2dQuad(originPosition, ColorPreset::Green, 10.0f, 10.0f, 0.0f, OriginPostion::Center, false);
        }
    }
    

}


