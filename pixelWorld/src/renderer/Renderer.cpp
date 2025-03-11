#include "Renderer.h"

#include "BackEnd/BackEnd.h"
#include "Opengl/GL_renderer.h"
#include <glm/gtx/rotate_vector.hpp>


#define M_PI 3.14159f

namespace Renderer
{
	//gloable veriable

	void RenderFrame()
	{

	}



	//render 2d
    void DotedBackground(glm::vec3 color, float size, int verticalGap, int horizantialGap)
    {
        //temp
        int gap1 = BackEnd::GetWindowCurrentHeight() / verticalGap;
        int gap2 = BackEnd::GetWindowCurrentWidth() / horizantialGap;
        for (int row = gap1; row < BackEnd::GetWindowCurrentHeight(); row += gap1)
        {
            for (int col = gap2; col < BackEnd::GetWindowCurrentWidth(); col += gap2)
            {
                OpenglRenderer::Render2dQuad(
                    glm::vec2(col, row),
                    color, size, size
                );
            }
        }
    }



    // line Draw call
    void DrawLine(
        glm::vec2 start,
        glm::vec2 end,
        float quadSize,
        float gap,
        glm::vec3 color
    )
    {
        // Calculate dx & dy  
        float dx = end.x - start.x;
        float dy = end.y - start.y;

        // Calculate line length
        float lineLength = std::sqrt(dx * dx + dy * dy);

        // Calculate unit vector (direction)
        glm::vec2 direction = glm::normalize(glm::vec2(dx, dy));

        // Calculate the total step size (quad + gap)
        float stepSize = quadSize + gap;

        // Calculate number of steps based on line length and step size
        int steps = std::ceil(lineLength / stepSize);

        // Put pixel for each step  
        glm::vec2 current = start;

        for (int i = 0; i <= steps; i++) {
            // Render quad at current position
            OpenglRenderer::Render2dQuad(
                current,          // Current position
                color,            // Color
                quadSize,         // Quad size
                quadSize          // Quad size
            );

            // Move to next position along the line direction
            current += direction * stepSize;

            // Optional: Break if we've passed the end point
            if (glm::length(current - start) > lineLength) {
                break;
            }
        }

        // Ensure the last point is drawn at the exact end
        OpenglRenderer::Render2dQuad(
            end,                  // End position
            color,                // Color
            quadSize,             // Quad size
            quadSize              // Quad size
        );
    }
    void OneWayArrow(
        glm::vec2 start,
        glm::vec2 end,
        float quadSize,
        float gap,
        glm::vec3 color,
        float tipLength,  // Negative value uses auto-scaling
        float tipAngle,   // Tip wing angle in degrees
        float tipThickness // Negative value uses quadSize
    )
    {
        // Draw the main line of the arrow
        DrawLine(start, end, quadSize, gap, color);

        // Calculate dx & dy  
        float dx = end.x - start.x;
        float dy = end.y - start.y;
        glm::vec2 direction = glm::normalize(glm::vec2(dx, dy));

        // Perpendicular vector (rotate 90 degrees)
        glm::vec2 perpendicular(-direction.y, direction.x);

        // Auto-scale tip length if not specified
        if (tipLength < 0) {
            tipLength = quadSize * 2.0f;
        }

        // Use quadSize for tip thickness if not specified
        float actualTipThickness = (tipThickness < 0) ? quadSize : tipThickness;

        // Convert tip angle to radians
        float angleRad = glm::radians(tipAngle);

        // Calculate arrow tip points
        glm::vec2 tipBase = end + direction * glm::vec2(quadSize);
        glm::vec2 tipLeft = tipBase -
            glm::rotate(direction * tipLength, angleRad) +
            glm::rotate(perpendicular * tipLength, angleRad);
        glm::vec2 tipRight = tipBase -
            glm::rotate(direction * tipLength, -angleRad) -
            glm::rotate(perpendicular * tipLength, -angleRad);

        // Draw arrow tip


            // Outline method
        DrawLine(tipBase, tipLeft, actualTipThickness, 0.0f, color);
        DrawLine(tipBase, tipRight, actualTipThickness, 0.0f, color);
        //DrawLine(tipLeft, tipRight, actualTipThickness, 0.0f, color);

    }
    void DrawCircle(
        glm::vec2 originPosition,
        glm::vec3 color,
        float radius,
        float lineThickness, float lineGap, int segments
    )
    {
        

        // Calculate angle increment
        const float angleIncrement = 2.0f * M_PI / segments;

        // Store the previous point to connect segments
        glm::vec2 prevPoint(
            originPosition.x + radius * std::cos(0),
            originPosition.y + radius * std::sin(0)
        );

        // Draw each segment of the circle
        for (int i = 1; i <= segments; ++i) {
            // Calculate angle for this segment
            float angle = i * angleIncrement;

            // Calculate point on the circle
            glm::vec2 point(
                originPosition.x + radius * std::cos(angle),
                originPosition.y + radius * std::sin(angle)
            );

            // Draw line from previous point to current point
            DrawLine(prevPoint, point, lineThickness, lineGap, color);

            // Update previous point
            prevPoint = point;
        }
    }

    void DrawArc(
        glm::vec2 originPosition,
        glm::vec3 color,
        float arcAngle,   // in degrees
        float radius,
        float lineThickness, float lineGap, int segments
    )
    {
        // Convert arc angle to radians
        float arcRadians = arcAngle * M_PI / 180.0f;

        // Calculate angle increment based on the arc angle
        const float angleIncrement = arcRadians / segments;

        // Store the previous point to connect segments
        glm::vec2 prevPoint(
            originPosition.x + radius * std::cos(0),
            originPosition.y + radius * std::sin(0)
        );

        // Draw each segment of the arc
        for (int i = 1; i <= segments; ++i) {
            // Calculate angle for this segment
            float angle = i * angleIncrement;

            // Calculate point on the arc
            glm::vec2 point(
                originPosition.x + radius * std::cos(angle),
                originPosition.y + radius * std::sin(angle)
            );

            // Draw line from previous point to current point
            DrawLine(prevPoint, point, lineThickness, lineGap, color);

            // Update previous point
            prevPoint = point;
        }
    }
    void DrawArcWithSrartAngle(glm::vec2 originPosition,
        glm::vec3 color,
        float startAngle,
        float arcAngle,
        float radius, float lineThickness, float lineGap, int segments)
    {
        // Convert angles to radians
        float startRadians = startAngle * M_PI / 180.0f;
        float arcRadians = arcAngle * M_PI / 180.0f;

        // Calculate angle increment based on the arc angle
        const float angleIncrement = arcRadians / segments;

        // Store the previous point to connect segments
        glm::vec2 prevPoint(
            originPosition.x + radius * std::cos(startRadians),
            originPosition.y + radius * std::sin(startRadians)
        );

        // Draw each segment of the arc
        for (int i = 1; i <= segments; ++i) {
            // Calculate angle for this segment
            float angle = startRadians + i * angleIncrement;

            // Calculate point on the arc
            glm::vec2 point(
                originPosition.x + radius * std::cos(angle),
                originPosition.y + radius * std::sin(angle)
            );

            // Draw line from previous point to current point
            DrawLine(prevPoint, point, lineThickness, lineGap, color);

            // Update previous point
            prevPoint = point;
        }
    }
}

