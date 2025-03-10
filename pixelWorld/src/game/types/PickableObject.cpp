#include "PickableObject.h"
#include "AssetManger/AssetManger.h"
#include "Input/Input.h"
#include "Opengl/GL_renderer.h"


enum DrawStat
{
    TINIT = 0,
    HOVER = 1,
    MAIN = 2
};

PickableObject::PickableObject() :
    m_solidColor(1.0f),
    m_width(100.0f),
    m_height(100.0f),
    m_isDragging(false),
    m_isHovering(false),
    isSolid(false),
    m_clickOffset(0.0f)
{
    m_status.m_position = glm::vec3(0.0f);
    m_status.m_angle = 0.0f;
    m_status.m_direction = glm::vec3(0.0f);
}

PickableObject::PickableObject(std::string texturepath, glm::vec2 position, float angle, float width, float height) :
    m_isDragging(false),
    m_isHovering(false),
    m_width(width),
    m_height(height),
    m_clickOffset(0.0f)
{
    m_textureId[MAIN] = AssetManger::LoadTexture(texturepath);
    m_status.m_position = glm::vec3(position, 0.0f);
    m_status.m_angle = angle;
    isSolid = false;
}

PickableObject::PickableObject(glm::vec3 solidColor, glm::vec2 position, float angle, float width, float height) :
    m_isDragging(false),
    m_isHovering(false),
    m_width(width),
    m_height(height),
    m_clickOffset(0.0f)
{
    m_solidColor = solidColor;
    m_status.m_position = glm::vec3(position, 0.0f);
    m_status.m_angle = angle;
    isSolid = true;
}

void PickableObject::Update()
{
    glm::vec2 mousePos = Input::GetMousePos();
    bool mouseLeftDown = Input::MouseButtonDown(MK_MOUSE_BUTTON_LEFT);
    bool mouseLeftPressed = Input::MouseButtonPressed(MK_MOUSE_BUTTON_LEFT);
    bool mouseLeftReleased = Input::MouseButtonReleased(MK_MOUSE_BUTTON_LEFT);

    // Check if mouse is hovering over object
    m_isHovering = IsInsideObject(mousePos);

    if (m_isHovering && mouseLeftPressed && !m_isDragging) {
        // Start dragging
        m_isDragging = true;
        glm::vec2 objCenter = glm::vec2(m_status.m_position.x, m_status.m_position.y);
        m_clickOffset = mousePos - objCenter;
    }
    else if (mouseLeftReleased && m_isDragging) {
        // Stop dragging
        m_isDragging = false;
    }

    if (m_isDragging && mouseLeftDown) {
        // Update position while dragging
        glm::vec2 newPos = mousePos - m_clickOffset;
        m_status.m_position.x = newPos.x;
        m_status.m_position.y = newPos.y;
    }
}

void PickableObject::UpdateStatus(glm::vec2 position, float angle, glm::vec2 direction)
{
    m_status.m_position = glm::vec3(position, 0.0f);
    m_status.m_angle = angle;
    m_status.m_direction = glm::vec3(direction, 0.0f);
}

void PickableObject::SetHoverTexture(std::string texturepath)
{
    m_textureId[HOVER] = AssetManger::LoadTexture(texturepath);
}

void PickableObject::PickObject()
{
    m_isDragging = true;
    glm::vec2 mousePos = Input::GetMousePos();
    m_clickOffset = glm::vec2(m_width * 0.5f, m_height * 0.5f); // Center offset
    m_status.m_position = glm::vec3(mousePos - m_clickOffset, 0.0f);
}

bool PickableObject::IsInsideObject(glm::vec2 position)
{
    // Get the center position of the object
    glm::vec2 centerPos = glm::vec2(m_status.m_position.x + m_width/2, m_status.m_position.y +m_height / 2);

    // If the object is rotated, we need to transform the position to object space
    if (m_status.m_angle != 0.0f) {
        // Calculate the angle in radians
        float angleRad = glm::radians(m_status.m_angle);

        // Translate position relative to object center
        glm::vec2 relativePos = position - centerPos;

        // Rotate the point in the opposite direction of the object's rotation
        float cosAngle = cos(-angleRad);
        float sinAngle = sin(-angleRad);

        glm::vec2 rotatedPos;
        rotatedPos.x = relativePos.x * cosAngle - relativePos.y * sinAngle;
        rotatedPos.y = relativePos.x * sinAngle + relativePos.y * cosAngle;

        // Use the rotated position for detection
        position = rotatedPos + centerPos;
    }

    // Get the bounds of the object
    float halfWidth = m_width * 0.5f;
    float halfHeight = m_height * 0.5f;

    // Check if the position is inside the object's bounds
    return (position.x >= centerPos.x - halfWidth &&
        position.x <= centerPos.x + halfWidth &&
        position.y >= centerPos.y - halfHeight &&
        position.y <= centerPos.y + halfHeight);
}

void PickableObject::Draw()
{
    // Highlight when hovered or dragged
    glm::vec3 color = m_solidColor;
    float width = m_width;
    float height = m_height;


    if (m_isHovering) {
        // Slightly highlight when hovering
        if (isSolid) {
            color = glm::vec3(
                glm::min(color.r * 1.2f, 1.0f),
                glm::min(color.g * 1.2f, 1.0f),
                glm::min(color.b * 1.2f, 1.0f)
            );
        }
    }

    if (m_isDragging) {
        // Add a slight scale effect when dragging
        width *= 1.05f;
        height *= 1.05f;
    }

    if (isSolid)
    {
        OpenglRenderer::Render2dQuad(
            m_status.m_position,
            color,
            width,
            height,
            m_status.m_angle
        );
    }
    else
    {
        // Apply tint when hovering for texture
        if (m_isHovering || m_isDragging)
        {
            if (m_textureId[HOVER] == 0)
            {
                glm::vec3 tint = m_isHovering ? glm::vec3(1.2f, 1.2f, 1.2f) : glm::vec3(1.0f);
                OpenglRenderer::Render2dTexture(
                    m_textureId[MAIN],
                    m_status.m_position,
                    tint,
                    width,
                    height,
                    m_status.m_angle
                );
            }
            else
            {
                OpenglRenderer::Render2dTexture(
                    m_textureId[HOVER],
                    m_status.m_position,
                    width,
                    height,
                    m_status.m_angle
                );
            }

        }
        else
        {
            OpenglRenderer::Render2dTexture(
                m_textureId[MAIN],
                m_status.m_position,
                width,
                height,
                m_status.m_angle
            );
        }


    }
}

// Getters and setters for new properties
void PickableObject::SetDimensions(float width, float height)
{
    m_width = width;
    m_height = height;
}

bool PickableObject::IsDragging() const
{
    return m_isDragging;
}

bool PickableObject::IsHovering() const
{
    return m_isHovering;
}