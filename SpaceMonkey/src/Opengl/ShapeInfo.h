#pragma once
#include <vector>

#include "types/MiskTypes.h"


namespace ShapeInfo
{
    namespace Quad2d
    {
        std::vector<Vertex> vertices
        {
            Vertex{{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},    // top-right     (index 0)
            Vertex{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},    // top-left      (index 1)
            Vertex{{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},    // bottom-right  (index 2)
            Vertex{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}     // bottom-left   (index 3)
        };

        std::vector<unsigned int> indices
        {
            0, 1, 2,  // First triangle: top-right, top-left, bottom-right
            1, 3, 2   // Second triangle: top-left, bottom-left, bottom-right
        };
    }

    namespace Triangle2d
    {
        // Define a triangle with top-left origin coordinate system
        // The triangle is positioned with one vertex at (0,0) and forms a right triangle
        std::vector<Vertex> vertices
        {
            Vertex{{0.0f, 0.0f, 0.0f}},     // top-left vertex     (index 0)
            Vertex{{1.0f, 0.0f, 0.0f}},     // top-right vertex    (index 1)
            Vertex{{0.0f, 1.0f, 0.0f}}      // bottom-left vertex  (index 2)
        };

        // For a triangle, indices are straightforward since it's already one triangle
        std::vector<unsigned int> indices
        {
            0, 1, 2  // One triangle: top-left, top-right, bottom-left
        };
    }

    // Alternative equilateral triangle centered at origin
    namespace EquilateralTriangle2d
    {
        std::vector<Vertex> vertices
        {
            Vertex{{0.0f, 0.0f, 0.0f}},     // top-left vertex     (index 0)
            Vertex{{1.0f, 0.0f, 0.0f}},     // top-right vertex    (index 1)
            Vertex{{0.5f, 1.0f, 0.0f}}      // bottom-left vertex  (index 2)
        };

        std::vector<unsigned int> indices
        {
            0, 1, 2  // One triangle: bottom, top-left, top-right
        };
    }

    namespace Circle2d
    {
        // Creates vertices and indices for a circle centered at (0,0)
        // segments: number of triangles used to approximate the circle
        std::vector<Vertex> GetVertices(int segments = 32)
        {
            std::vector<Vertex> vertices;

            // Add center vertex
            vertices.push_back(Vertex{ {0.0f, 0.0f, 0.0f} });

            // Add vertices around the circle
            float angleIncrement = 2.0f * 3.14159265359f / static_cast<float>(segments);
            for (int i = 0; i < segments; ++i)
            {
                float angle = i * angleIncrement;
                float x = std::cos(angle) * 0.5f;  // Radius of 0.5 to fit in unit square
                float y = std::sin(angle) * 0.5f;
                vertices.push_back(Vertex{ {x, y, 0.0f} });
            }

            return vertices;
        }

        std::vector<unsigned int> GetIndices(int segments = 32)
        {
            std::vector<unsigned int> indices;

            // Create triangles: center vertex (0) to pairs of adjacent points on the circle
            for (int i = 0; i < segments; ++i)
            {
                indices.push_back(0);  // Center vertex
                indices.push_back(1 + i);  // Current vertex on circumference
                indices.push_back(1 + ((i + 1) % segments));  // Next vertex on circumference
            }

            return indices;
        }

        // Default with 32 segments
        std::vector<Vertex> vertices = GetVertices();
        std::vector<unsigned int> indices = GetIndices();
    }
}