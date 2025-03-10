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