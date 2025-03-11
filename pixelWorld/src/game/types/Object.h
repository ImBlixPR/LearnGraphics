#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

class Object
{
public:
    // Constructors (existing)
    Object();
    Object(
        std::string texturepath,
        glm::vec2 position,
        float angle = 0.0f,
        float width = 100.0f, float height = 100.0f);
    Object(
        glm::vec3 solidColor,
        glm::vec2 position,
        float angle = 0.0f,
        float width = 100.0f,
        float height = 100.0f);

    // Multiple texture constructor (new)
    Object(const std::vector<std::string>& texturePaths, glm::vec2 position, float angle, float width = 100.0f, float height = 100.0f);

    //rendering methods (existing with modifications)
    virtual void Draw();

    // Collision detection (existing)
    bool IsInsideObject(glm::vec2 position);

    // Transform getters and setters (new/modified)
    void SetPosition(const glm::vec2& position);
    glm::vec2 GetPosition() const;
    void SetAngle(float angle);
    float GetAngle() const;
    void SetScale(float width, float height);
    glm::vec2 GetScale() const;

    // Appearance setters (new)
    void SetColor(const glm::vec3& color);
    void SetActiveTexture(unsigned int index);
    void SetHoverTexture(unsigned int textureIndex);

    // Interaction state methods (new/modified)
    void SetDragging(bool dragging);
    bool IsDragging() const;
    void SetHovering(bool hovering);
    bool IsHovering() const;
    void SetClickOffset(const glm::vec2& offset);
    glm::vec2 GetClickOffset() const;

    // ID and tagging system (new)
    void SetId(int id);
    int GetId() const;
    void SetTag(const std::string& tag);
    const std::string& GetTag() const;

protected:
    // Appearance
    unsigned int m_textureId[3] = { 0 };
    unsigned int m_activeTextureIndex = 0;
    unsigned int m_activeHoverTextureIndex = 0;
    glm::vec3 m_solidColor;
    bool isSolid;

    // Dimensions
    float m_width;
    float m_height;

    // Object state
    bool m_isDragging;
    bool m_isHovering;
    glm::vec2 m_clickOffset;

    // Transform
    struct {
        glm::vec2 m_position;
        float m_angle;
        glm::vec2 m_direction;
    } m_status;

    // Identification
    int m_id = -1;
    std::string m_tag = "";
};