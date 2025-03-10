#pragma once
#include <glm/glm.hpp>


struct bufferObject {
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
};

struct Vertex {
    Vertex() = default;
    Vertex(glm::vec3 pos) {
        position = pos;
    }
    Vertex(glm::vec3 pos, glm::vec3 norm) {
        position = pos;
        normal = norm;
    }
    Vertex(glm::vec3 _pos, glm::vec3 _norm, glm::vec2 _uv) {
        position = _pos;
        normal = _norm;
        uv = _uv;
    }
    Vertex(glm::vec3 _position, glm::vec3 _normal, glm::vec2 _uv, glm::vec3 _tangent) {
        position = _position;
        normal = _normal;
        uv = _uv;
        tangent = _tangent;
    }
    glm::vec3 position = glm::vec3(0);
    glm::vec3 normal = glm::vec3(0);
    glm::vec2 uv = glm::vec2(0);
    glm::vec3 tangent = glm::vec3(0);
    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};

struct ShapeVertex {
	glm::vec2 Position;
};


struct TextureData {
    std::string m_filepath;
    std::string m_fileExtenction;
    int m_width = 0;
    int m_height = 0;
    int m_numChannels = 0;
    void* m_data = nullptr;
};


//positioning status

struct PositionStatus
{
    PositionStatus(glm::vec3 position)
    {
        m_position = position;
    }
    PositionStatus(glm::vec3 position, float angle)
    {
        m_position = position;
        m_angle = angle;
    }
    PositionStatus(glm::vec3 position, float angle, glm::vec3 direction)
    {
        m_position = position;
        m_angle = angle;
        m_direction = direction;
    }

    glm::vec3 m_position = glm::vec3(0.0f);
    float m_angle = 0.0f;
    glm::vec3 m_direction = glm::vec3(0.0f, 1.0f, 0.0f);
    bool operator==(const PositionStatus& other) const {
        return m_position == other.m_position && m_angle == other.m_angle && m_direction == other.m_direction;
    }
};