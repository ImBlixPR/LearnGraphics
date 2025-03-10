#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

class PickableObject
{
public:
    // Constructors
    PickableObject();
    PickableObject(
        std::string texturepath,
        glm::vec2 position,
        float angle,
        float width = 40.0f, float height = 40.0f
    );
    PickableObject(
        glm::vec3 solidColor,
        glm::vec2 position,
        float angle = 0.0f,
        float width = 40.0f, float height = 40.0f
    );

    //Set texture 
    void SetHoverTexture(std::string texturepath);

    // Update and interaction methods
    void Update();
    void UpdateStatus(glm::vec2 position, float angle, glm::vec2 direction);
    void PickObject();
    bool IsInsideObject(glm::vec2 position);
    void Draw();

    // Getters and setters
    void SetDimensions(float width, float height);
    bool IsDragging() const;
    bool IsHovering() const;

private:
    // Object appearance
    std::unordered_map<unsigned int, unsigned int> m_textureId{ 0 };
    glm::vec3 m_solidColor;
    bool isSolid;

    // Object dimensions
    float m_width;
    float m_height;

    // Object state
    bool m_isDragging;
    bool m_isHovering;
    glm::vec2 m_clickOffset;

    // Object status/transform
    struct {
        glm::vec3 m_position;
        float m_angle;
        glm::vec3 m_direction;
    } m_status;
};