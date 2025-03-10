#include "Scene.h"


#include "BackEnd/BackEnd.h"
#include "Opengl/GL_renderer.h"
#include <glm/gtx/rotate_vector.hpp>

#include "renderer/Renderer.h"




namespace Scene
{
//	void DotedBackground(glm::vec3 color, float size, int verticalGap, int horizantialGap)
//	{
//		//temp
//		int gap1 = BackEnd::GetWindowCurrentHeight() / verticalGap;
//		int gap2 = BackEnd::GetWindowCurrentWidth() / horizantialGap;
//		for (int row = gap1; row < BackEnd::GetWindowCurrentHeight(); row += gap1)
//		{
//			for (int col = gap2; col < BackEnd::GetWindowCurrentWidth(); col += gap2)
//			{
//				OpenglRenderer::Render2dQuad(
//					glm::vec2(col, row),
//					color, size, size
//				);
//			}
//		}
//	}
//	
//
//
//// line Draw call
//    void DrawLine(
//        glm::vec2 start,
//        glm::vec2 end,
//        float quadSize,
//        float gap,
//        glm::vec3 color
//    )
//    {
//        // Calculate dx & dy  
//        float dx = end.x - start.x;
//        float dy = end.y - start.y;
//
//        // Calculate line length
//        float lineLength = std::sqrt(dx * dx + dy * dy);
//
//        // Calculate unit vector (direction)
//        glm::vec2 direction = glm::normalize(glm::vec2(dx, dy));
//
//        // Calculate the total step size (quad + gap)
//        float stepSize = quadSize + gap;
//
//        // Calculate number of steps based on line length and step size
//        int steps = std::ceil(lineLength / stepSize);
//
//        // Put pixel for each step  
//        glm::vec2 current = start;
//
//        for (int i = 0; i <= steps; i++) {
//            // Render quad at current position
//            OpenglRenderer::Render2dQuad(
//                current,          // Current position
//                color,            // Color
//                quadSize,         // Quad size
//                quadSize          // Quad size
//            );
//
//            // Move to next position along the line direction
//            current += direction * stepSize;
//
//            // Optional: Break if we've passed the end point
//            if (glm::length(current - start) > lineLength) {
//                break;
//            }
//        }
//
//        // Ensure the last point is drawn at the exact end
//        OpenglRenderer::Render2dQuad(
//            end,                  // End position
//            color,                // Color
//            quadSize,             // Quad size
//            quadSize              // Quad size
//        );
//    }
//    void OneWayArrow(
//        glm::vec2 start,
//        glm::vec2 end,
//        float quadSize,
//        float gap,
//        glm::vec3 color,
//        float tipLength,  // Negative value uses auto-scaling
//        float tipAngle,   // Tip wing angle in degrees
//        float tipThickness // Negative value uses quadSize
//    )
//    {
//        // Draw the main line of the arrow
//        DrawLine(start, end, quadSize, gap, color);
//
//        // Calculate dx & dy  
//        float dx = end.x - start.x;
//        float dy = end.y - start.y;
//        glm::vec2 direction = glm::normalize(glm::vec2(dx, dy));
//
//        // Perpendicular vector (rotate 90 degrees)
//        glm::vec2 perpendicular(-direction.y, direction.x);
//
//        // Auto-scale tip length if not specified
//        if (tipLength < 0) {
//            tipLength = quadSize * 2.0f;
//        }
//
//        // Use quadSize for tip thickness if not specified
//        float actualTipThickness = (tipThickness < 0) ? quadSize : tipThickness;
//
//        // Convert tip angle to radians
//        float angleRad = glm::radians(tipAngle);
//
//        // Calculate arrow tip points
//        glm::vec2 tipBase = end +direction * glm::vec2(quadSize);
//        glm::vec2 tipLeft = tipBase -
//            glm::rotate(direction * tipLength, angleRad) +
//            glm::rotate(perpendicular * tipLength, angleRad);
//        glm::vec2 tipRight = tipBase -
//            glm::rotate(direction * tipLength, -angleRad) -
//            glm::rotate(perpendicular * tipLength, -angleRad);
//
//        // Draw arrow tip
//
//        
//            // Outline method
//        DrawLine(tipBase, tipLeft, actualTipThickness, 0.0f, color);
//        DrawLine(tipBase, tipRight, actualTipThickness, 0.0f, color);
//        //DrawLine(tipLeft, tipRight, actualTipThickness, 0.0f, color);
//       
//    }
    void AnimateArrowAroundCircle(
        glm::vec2* tipPostion,
        glm::vec2 originPostion,
        float radius,float angle, float time
    )
    {
        float lerpAngle = glm::mix(0.0f, angle, time);
        float x = radius * cosf(lerpAngle);
        float y = radius * sinf(lerpAngle);
        *tipPostion = originPostion + glm::vec2(x, y);
    }
    void AnimateArrowAroundCircle(
        glm::vec2* tipPosition,
        glm::vec2 originPosition,
        float radius,
        float time,
        const ArrowAnimationParams& params)
    {
        // Clamp time to 0-1 if not looping, or wrap around if looping
        float currentTime = params.loop ? fmodf(time * params.speed, 1.0f) :
            glm::clamp(time * params.speed, 0.0f, 1.0f);

        // Apply easing if needed
        if (params.easingPower != 1.0f) {
            if (params.easingPower > 0) {
                // Power easing (e.g., quadratic, cubic)
                currentTime = powf(currentTime, params.easingPower);
            }
            else {
                // Inverse power easing (ease out)
                currentTime = 1.0f - powf(1.0f - currentTime, -params.easingPower);
            }
        }

        // Calculate the angle difference
        float angleDiff = params.endAngle - params.startAngle;

        // Adjust direction if moving counter-clockwise
        if (!params.clockwise && angleDiff > 0) {
            angleDiff -= 2.0f * M_PI;
        }
        else if (params.clockwise && angleDiff < 0) {
            angleDiff += 2.0f * M_PI;
        }

        // Calculate current angle
        float currentAngle = params.startAngle + angleDiff * currentTime;

        // Calculate position on circle
        float x = radius * cosf(currentAngle);
        float y = radius * sinf(currentAngle);

        // Set the position
        *tipPosition = originPosition + glm::vec2(x, y);


    }
    void DrawArrowFollowingCircle(glm::vec2 circleCenter, float radius,
        float time, glm::vec3 color, float quadSize, float gap, const ArrowAnimationParams& params)
    {
        glm::vec2 tipPosition;
        glm::vec2 tipDirection;

        // Calculate position and direction
        AnimateArrowAroundCircle(&tipPosition, circleCenter, radius, time, params);

        // Calculate arrow start point (slightly behind the tip)
        float arrowLength = radius * 0.3f; // Adjust as needed
        glm::vec2 arrowStart = tipPosition - tipDirection * arrowLength;

        // Draw the arrow
        Renderer::OneWayArrow(arrowStart, tipPosition, quadSize, gap, color);

        // Optionally draw the circle
        Renderer::DrawCircle(circleCenter, color * 0.7f, radius, quadSize * 0.5f, gap);
    }
    void AnimatedCircleDraw(glm::vec2 originPosition, glm::vec3 color, float radius, float lineThickness, float lineGap, int segments, float animationTime, float totalDuration)
    {
        
    }
}

