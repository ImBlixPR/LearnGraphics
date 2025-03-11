#pragma once

#include <glm/glm.hpp>



namespace Lerp
{
    // Smooth step function (cubic)
    float smoothStep(float t) {
        // Clamp t to 0-1 range
        t = glm::clamp(t, 0.0f, 1.0f);
        // Apply cubic smoothing: 3t² - 2t³
        return t * t * (3.0f - 2.0f * t);
    }

    // Smoother step function (quintic)
    float smootherStep(float t) {
        // Clamp t to 0-1 range
        t = glm::clamp(t, 0.0f, 1.0f);
        // Apply quintic smoothing: 6t⁵ - 15t⁴ + 10t³
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    // Smooth lerp using cubic easing
    float smoothLerp(float a, float b, float t) {
        return glm::mix(a, b, smoothStep(t));
    }
    glm::vec2 smoothLerp(glm::vec2 a, glm::vec2 b, float t) {
        return glm::mix(a, b, smoothStep(t));
    }

    // Smoother lerp using quintic easing
    float smootherLerp(float a, float b, float t) {
        return glm::mix(a, b, smootherStep(t));
    }
    glm::vec2 smootherLerp(glm::vec2 a, glm::vec2 b, float t) {
        return glm::mix(a, b, smootherStep(t));
    }

    // Time-based smooth easing function with customizable curve
    float timeEase(float currentTime, float duration, float easingPower = 2.0f) {
        // Get normalized time (0 to 1)
        float t = glm::clamp(currentTime / duration, 0.0f, 1.0f);

        // Apply easing based on power (higher = sharper curve)
        if (easingPower == 1.0f) {
            return t; // Linear (no easing)
        }
        else if (easingPower == 2.0f) {
            return smoothStep(t); // Cubic easing
        }
        else if (easingPower == 3.0f) {
            return smootherStep(t); // Quintic easing
        }
        else if (easingPower > 0.0f) {
            // Custom power-based easing for ease-in
            return powf(t, easingPower);
        }
        else {
            // Negative power for ease-out
            return 1.0f - powf(1.0f - t, -easingPower);
        }
    }

    // Time-based lerp with various easing options
    float timeLerp(float start, float end, float currentTime, float duration, float easingPower = 2.0f) {
        return glm::mix(start, end, timeEase(currentTime, duration, easingPower));
    }
}
