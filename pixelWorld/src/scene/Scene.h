#pragma once


#include <glm/glm.hpp>


#define M_PI 3.14159f

#define M_PI_2 1.57079632679489661923f

namespace Scene{

    struct ArrowAnimationParams {
        float speed = 1.0f;            // Speed multiplier
        float startAngle = 0.0f;       // Starting angle (radians)
        float endAngle = 2.0f * M_PI;  // Ending angle (radians)
        bool clockwise = true;         // Direction of rotation
        float easingPower = 1.0f;      // 1.0 = linear, 2.0 = quadratic, etc.
        bool loop = false;             // Whether to loop the animation
        float tangentAngleOffset = 0.0f; // Offset for tangent calculation (radians)
    };

    void AnimateArrowAroundCircle(
        glm::vec2* tipPosition,          // Output: position of arrow tip
        glm::vec2 originPosition,        // Center of the circle
        float radius,                    // Circle radius
        float time,                      // Current time (0.0 to 1.0)
        const ArrowAnimationParams& params = ArrowAnimationParams()
    );

    // Example usage function to draw an arrow following a circle
    void DrawArrowFollowingCircle(
        glm::vec2 circleCenter,
        float radius,
        float time,
        glm::vec3 color,
        float quadSize = 1.0f,
        float gap = 0.5f,
        const ArrowAnimationParams& params = ArrowAnimationParams()
    );

    void AnimatedCircleDraw(
        glm::vec2 originPosition,
        glm::vec3 color,
        float radius,
        float lineThickness,
        float lineGap,
        int segments,
        float animationTime,    // Current time in seconds
        float totalDuration     // Total animation duration in seconds
    );
}
