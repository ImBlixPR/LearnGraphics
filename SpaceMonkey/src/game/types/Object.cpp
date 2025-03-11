#include "Object.h"


#include "AssetManger/AssetManger.h"
#include "Opengl/GL_renderer.h"
#include "renderer/Renderer.h"





Object::Object() :
    m_solidColor(1.0f),
    m_width(100.0f),
    m_height(100.0f),
    m_isDragging(false),
    m_isHovering(false),
    isSolid(false),
    m_clickOffset(0.0f)
{
    m_status.m_position = glm::vec2(0.0f);
    m_status.m_angle = 0.0f;
    m_status.m_direction = glm::vec2(0.0f);
}

Object::Object(
    std::string texturepath,
    glm::vec2 position,
    float angle, float width, float height) :
    m_isDragging(false),
    m_isHovering(false),
    m_width(width),
    m_height(height),
    m_clickOffset(0.0f)
{
    m_textureId[0] = AssetManger::LoadTexture(texturepath);
    m_status.m_position = position;
    m_status.m_angle = angle;
    isSolid = false;
}

Object::Object(
    glm::vec3 solidColor,
    glm::vec2 position,
    float angle, float width, float height) :
    m_isDragging(false),
    m_isHovering(false),
    m_width(width),
    m_height(height),
    m_clickOffset(0.0f)
{
    m_solidColor = solidColor;
    m_status.m_position = position;
    m_status.m_angle = angle;
    isSolid = true;
}

Object::Object(
    const std::vector<std::string>& texturePaths,
    glm::vec2 position,
    float angle,
    float width,
    float height) :
    m_isDragging(false),
    m_isHovering(false),
    m_width(width),
    m_height(height),
    m_clickOffset(0.0f)
{
    
    for (size_t i  = 0; i < texturePaths.size(); i++)
    {
        m_textureId[i] = AssetManger::LoadTexture(texturePaths[i]);
    }
    m_status.m_position = position;
    m_status.m_angle = angle;
    isSolid = false;
}

void Object::SetHoverTexture(unsigned int index)
{
    if (index < 3 && m_textureId[index] != 0) {
        m_activeHoverTextureIndex = index;
    }
}

void Object::Draw()
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
        //Renderer::DrawCircle(
        //    m_status.m_position,
        //    color,
        //    20.0f,
        //    1.0f
        //);
    }
    else
    {
        // Apply tint when hovering for texture
        if (m_isHovering)
        {
            if (m_textureId[m_activeHoverTextureIndex] == 0)
            {
                glm::vec3 tint = m_isHovering ? glm::vec3(1.2f, 1.2f, 1.2f) : glm::vec3(1.0f);
                OpenglRenderer::Render2dTexture(
                    m_textureId[m_activeTextureIndex],
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
                    m_textureId[m_activeTextureIndex],
                    m_status.m_position,
                    width,
                    height,
                    m_status.m_angle
                );
            }

        }
        else
        {
            //printf("problem here!");
            OpenglRenderer::Render2dTexture(
                m_textureId[m_activeTextureIndex],
                m_status.m_position,
                width,
                height,
                m_status.m_angle
            );
        }


    }
}

bool Object::IsInsideObject(glm::vec2 position)
{
    // Get the center position of the object
    glm::vec2 centerPos = glm::vec2(m_status.m_position.x + m_width / 2, m_status.m_position.y + m_height / 2);

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

void Object::SetPosition(const glm::vec2& position) {
    m_status.m_position = position;
}

void Object::SetRotation(const float& rotate)
{

}

glm::vec2 Object::GetPosition() const {
    return m_status.m_position;
}

glm::vec2 Object::GetRotation() const
{
    return glm::vec2();
}

void Object::SetAngle(float angle) {
    m_status.m_angle = angle;
}

float Object::GetAngle() const {
    return m_status.m_angle;
}

void Object::SetScale(float width, float height) {
    m_width = width;
    m_height = height;
}

glm::vec2 Object::GetScale() const {
    return glm::vec2(m_width, m_height);
}

void Object::SetColor(const glm::vec3& color) {
    m_solidColor = color;
}

void Object::SetActiveTexture(unsigned int index) {
    if (index < 3 && m_textureId[index] != 0) {
        m_activeTextureIndex = index;
    }
}

void Object::SetDragging(bool dragging) {
    m_isDragging = dragging;
}

bool Object::IsDragging() const {
    return m_isDragging;
}

void Object::SetHovering(bool hovering) {
    m_isHovering = hovering;
}

bool Object::IsHovering() const {
    return m_isHovering;
}

void Object::SetClickOffset(const glm::vec2& offset) {
    m_clickOffset = offset;
}

glm::vec2 Object::GetClickOffset() const {
    return m_clickOffset;
}

void Object::SetId(int id) {
    m_id = id;
}

int Object::GetId() const {
    return m_id;
}

void Object::SetTag(const std::string& tag) {
    m_tag = tag;
}

const std::string& Object::GetTag() const {
    return m_tag;
}