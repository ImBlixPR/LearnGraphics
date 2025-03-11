#include "Game.h"
#include "Input/Input.h"
#include "BackEnd/BackEnd.h"
#include "scene/Scene.h"
#include "game/types/PickableObject.h"
#include "game/types/Object.h"
#include "game/interaction/PickableManager.h"
#include "game/interaction/DeleteObjectHandler.h"
#include <vector>
#include <random>
#include <ctime>

#include "Opengl/GL_renderer.h"
#include "AssetManger/AssetManger.h"

namespace Game
{
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> sizeDistribution;
    std::uniform_real_distribution<float> speedDistribution;
    std::uniform_real_distribution<float> angleDistribution;
    std::uniform_real_distribution<float> posDistribution;
    std::uniform_int_distribution<int> spawnSideDistribution;

    // Asteroid struct to manage asteroid properties
    struct Asteroid {
        Object* object;
        glm::vec2 velocity;
        float rotationSpeed;
        float size;
        bool active;

        Asteroid() : object(nullptr), velocity(0.0f), rotationSpeed(0.0f), size(0.0f), active(false) {}

        ~Asteroid() {
            if (object) {
                delete object;
                object = nullptr;
            }
        }
    };

    // Health struct to manage healthbar properties
    struct HealthBar
    {
        Object m_healthbar;
        Object* LifeCount[4]; //red solid object behinde the m_healthbar 
        
    };
    int playerHealth = 3;         // Player starts with 3 health points
    const int MAX_HEALTH = 3;     // Maximum health capacity
    HealthBar healthBar;          // Health bar instance
    bool invulnerable = false;    // Invulnerability after taking damage
    float invulnerabilityTimer = 0.0f;  // Timer for invulnerability period
    const float INVULNERABILITY_DURATION = 2.0f;  // Duration of invulnerability in seconds
    bool gameOver = false;        // Game over state

    // Global variables for the game
    Object monkeyShip;
    unsigned int defeatedId;
    
    std::vector<Asteroid> asteroids;
    glm::vec2 monkeyPosition;
    glm::vec2 monkeyVelocity = glm::vec2(0.0f, 0.0f); // Add velocity vector

    const float MAX_SPEED = 600.0f;       // Maximum speed
    const float ACCELERATION = 800.0f;    // How quickly ship accelerates
    const float DECELERATION = 400.0f;    // How quickly ship slows down when no input
    float shipWidth = 100.0f;  // Adjust based on your sprite size
    float shipHeight = 100.0f;

    // Asteroid constants
    const int MAX_ASTEROIDS = 60;          // Maximum number of asteroids at once
    const float ASTEROID_MIN_SIZE = 30.0f; // Minimum asteroid size
    const float ASTEROID_MAX_SIZE = 120.0f; // Maximum asteroid size
    const float ASTEROID_MIN_SPEED = 500.0f; // Minimum asteroid speed
    const float ASTEROID_MAX_SPEED = 200.0f; // Maximum asteroid speed
    const float ASTEROID_SPAWN_TIMER_MAX = 1.0f; // Seconds between asteroid spawns
    float asteroidSpawnTimer = 0.0f;       // Timer for asteroid spawning

    // Initialize RNG with proper seeding
    void InitRNG() {
        rng.seed(static_cast<unsigned int>(time(nullptr)));
        sizeDistribution = std::uniform_real_distribution<float>(ASTEROID_MIN_SIZE, ASTEROID_MAX_SIZE);
        speedDistribution = std::uniform_real_distribution<float>(ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED);
        angleDistribution = std::uniform_real_distribution<float>(0.0f, 2.0f * 3.14159f); // 0 to 2π
        posDistribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
        spawnSideDistribution = std::uniform_int_distribution<int>(0, 3); // 0=top, 1=right, 2=bottom, 3=left
    }

    // Create a new asteroid with random properties
    void SpawnAsteroid() {
        // Find an inactive asteroid slot
        int inactiveIndex = -1;
        for (size_t i = 0; i < asteroids.size(); i++) {
            if (!asteroids[i].active) {
                inactiveIndex = static_cast<int>(i);
                break;
            }
        }

        // If no inactive slot, create a new one if below max
        if (inactiveIndex == -1 && asteroids.size() < MAX_ASTEROIDS) {
            asteroids.push_back(Asteroid());
            inactiveIndex = static_cast<int>(asteroids.size() - 1);
        }

        // If we have a valid index, initialize the asteroid
        if (inactiveIndex != -1) {
            Asteroid& asteroid = asteroids[inactiveIndex];

            // Determine size
            float size = sizeDistribution(rng);

            // Determine spawn position (outside screen)
            float windowWidth = static_cast<float>(BackEnd::GetWindowCurrentWidth());
            float windowHeight = static_cast<float>(BackEnd::GetWindowCurrentHeight());
            glm::vec2 position;

            // Decide which side to spawn from
            int spawnSide = spawnSideDistribution(rng);
            switch (spawnSide) {
            case 0: // Top
                position = glm::vec2(posDistribution(rng) * windowWidth, -size);
                break;
            case 1: // Right
                position = glm::vec2(windowWidth + size, posDistribution(rng) * windowHeight);
                break;
            case 2: // Bottom
                position = glm::vec2(posDistribution(rng) * windowWidth, windowHeight + size);
                break;
            case 3: // Left
                position = glm::vec2(-size, posDistribution(rng) * windowHeight);
                break;
            }

            // Calculate direction toward center-ish with some randomness
            glm::vec2 centerPoint = glm::vec2(
                windowWidth * (0.3f + posDistribution(rng) * 0.4f),
                windowHeight * (0.3f + posDistribution(rng) * 0.4f)
            );
            glm::vec2 direction = glm::normalize(centerPoint - position);

            // Create speeds and rotation
            float speed = speedDistribution(rng);
            float rotationSpeed = angleDistribution(rng) * 0.5f - 3.14159f; // -π to +π

            // Create or update the asteroid object
            if (asteroid.object) {
                delete asteroid.object;
            }

            asteroid.object = new Object("res/textures/asteroid.png", position, 0.0f, size, size);
            asteroid.velocity = direction * speed;
            asteroid.rotationSpeed = rotationSpeed;
            asteroid.size = size;
            asteroid.active = true;
        }
    }

    // Check if asteroid is outside the screen with a margin
    bool IsAsteroidOutOfBounds(const Asteroid& asteroid) {
        if (!asteroid.active || !asteroid.object) return false;

        glm::vec2 pos = asteroid.object->GetPosition();
        float size = asteroid.size;
        float windowWidth = static_cast<float>(BackEnd::GetWindowCurrentWidth());
        float windowHeight = static_cast<float>(BackEnd::GetWindowCurrentHeight());
        float margin = size * 2.0f; // Give some extra margin

        return (pos.x < -margin ||
            pos.x > windowWidth + margin ||
            pos.y < -margin ||
            pos.y > windowHeight + margin);
    }

    void InitHealthBar() {
        // Initialize the health bar background
        float windowWidth = static_cast<float>(BackEnd::GetWindowCurrentWidth());
        float barWidth = 200.0f;
        float barHeight = 100.0f;
        float padding = 5.0f;

        // Create health bar background at top-right corner
        healthBar.m_healthbar = Object("res/textures/healthbar.png",
            glm::vec2(padding, padding),
            0.0f, barWidth, barHeight);

        // Initialize individual health indicators
        float healthWidth = (barWidth - 40.0f) / MAX_HEALTH;
        float healthHeight = barHeight - 10.0f;

        for (int i = 0; i < MAX_HEALTH; i++) {
            healthBar.LifeCount[i] = new Object(glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec2(padding + 15.0f + (i * healthWidth) + (i * 5.0f),
                    padding + 40.0f),
                0.0f, healthWidth, healthHeight - barHeight/2);
        }
    }

    void UpdateHealthBar() {
        // Draw active health units
        for (int i = 0; i < playerHealth; i++) {
            healthBar.LifeCount[i]->Draw();
        }
        // Draw the health bar background
        healthBar.m_healthbar.Draw();


    }

    void PlayerTakeDamage() {
        if (!invulnerable && !gameOver) {
            playerHealth--;

            // Check if player is dead
            if (playerHealth <= 0) {
                gameOver = true;
                // You might want to add game over logic here
            }
            else {
                // Activate invulnerability period
                invulnerable = true;
                invulnerabilityTimer = INVULNERABILITY_DURATION;
            }
        }
    }

    bool CheckCollision(const glm::vec2& pos1, float radius1, const glm::vec2& pos2, float radius2) {
        // Calculate distance between centers
        float dx = pos1.x - pos2.x;
        float dy = pos1.y - pos2.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        // Check if circles overlap
        return distance < (radius1 + radius2);
    }

    // Update all asteroids
    void UpdateAsteroids(float dt) {
        // Update spawn timer
        asteroidSpawnTimer -= dt;
        if (asteroidSpawnTimer <= 0.0f) {
            SpawnAsteroid();
            asteroidSpawnTimer = ASTEROID_SPAWN_TIMER_MAX;
        }

        // Get player collision radius (approximate)
        float playerRadius = shipWidth / 2.5f;  // Slightly smaller than ship for forgiving collisions

        // Update each asteroid
        for (auto& asteroid : asteroids) {
            if (asteroid.active && asteroid.object) {
                // Update position
                glm::vec2 position = asteroid.object->GetPosition();
                position += asteroid.velocity * dt;
                asteroid.object->SetPosition(position);

                // Update rotation
                float rotation = asteroid.object->GetAngle();
                rotation += asteroid.rotationSpeed * dt;
                asteroid.object->SetAngle(rotation);

                // Check collision with player
                if (!invulnerable && !gameOver) {
                    float asteroidRadius = asteroid.size / 2.0f;
                    if (CheckCollision(monkeyPosition, playerRadius, position, asteroidRadius)) {
                        PlayerTakeDamage();
                    }
                }

                // Check if out of bounds
                if (IsAsteroidOutOfBounds(asteroid)) {
                    asteroid.active = false;
                }

                // Draw if active
                if (asteroid.active) {
                    asteroid.object->Draw();
                }
            }
        }
    }

    bool Init() {
        // Initialize the ship
        monkeyShip = { "res/textures/monkeySpaceShip.png", glm::vec2(100.0f, 100.0f) };
        monkeyPosition = monkeyShip.GetPosition();

        defeatedId = AssetManger::LoadTexture("res/textures/defeated.png");
        // Initialize random number generator
        InitRNG();

        // Initialize asteroid container
        asteroids.reserve(MAX_ASTEROIDS);

        // Initialize health bar
        InitHealthBar();

        // Reset game state
        playerHealth = MAX_HEALTH;
        invulnerable = false;
        invulnerabilityTimer = 0.0f;
        gameOver = false;

        return true;
    }

    void PlayAgain()
    {
        if (Input::KeyPressed(MK_KEY_R))
        {
            Init();
            gameOver = false;
        }
    }

    bool Run(float dt) {
        // Skip game logic if game over
        if (gameOver) {
            // Display game over message or screen
            float messageWidth = 400;
            float messageHeight = 200;

            OpenglRenderer::Render2dTexture(defeatedId, glm::vec2(BackEnd::GetWindowCurrentWidth() / 2 - messageWidth / 2,
                BackEnd::GetWindowCurrentHeight() / 2 - messageHeight / 2), messageWidth, messageHeight);
            PlayAgain();
            return false;
        }

        // Update invulnerability timer
        if (invulnerable) {
            invulnerabilityTimer -= dt;
            if (invulnerabilityTimer <= 0.0f) {
                invulnerable = false;
            }
        }

        // Input handling - now adjusts velocity instead of position directly
        glm::vec2 inputDirection(0.0f, 0.0f);

        if (Input::KeyDown(MK_KEY_W)) {
            inputDirection.y -= 1.0f;
        }
        if (Input::KeyDown(MK_KEY_S)) {
            inputDirection.y += 1.0f;
        }
        if (Input::KeyDown(MK_KEY_A)) {
            inputDirection.x -= 1.0f;
        }
        if (Input::KeyDown(MK_KEY_D)) {
            inputDirection.x += 1.0f;
        }

        // Normalize input direction if moving diagonally
        if (glm::length(inputDirection) > 0.0f) {
            inputDirection = glm::normalize(inputDirection);

            // Accelerate in the input direction
            monkeyVelocity += inputDirection * ACCELERATION * dt;

            // Clamp velocity to maximum speed
            if (glm::length(monkeyVelocity) > MAX_SPEED) {
                monkeyVelocity = glm::normalize(monkeyVelocity) * MAX_SPEED;
            }
        }
        else {
            // No input, decelerate (slow down)
            if (glm::length(monkeyVelocity) > 0.0f) {
                glm::vec2 deceleration = glm::normalize(monkeyVelocity) * DECELERATION * dt;

                // Make sure we don't overshoot zero
                if (glm::length(deceleration) > glm::length(monkeyVelocity)) {
                    monkeyVelocity = glm::vec2(0.0f, 0.0f);
                }
                else {
                    monkeyVelocity -= deceleration;
                }
            }
        }

        // Update position based on velocity
        monkeyPosition += monkeyVelocity * dt;

        // Boundary checks to keep ship on screen (tiling)
        if (monkeyPosition.x < -shipWidth)
            monkeyPosition.x = BackEnd::GetWindowCurrentWidth();
        if (monkeyPosition.y < -shipHeight)
            monkeyPosition.y = BackEnd::GetWindowCurrentHeight();
        if (monkeyPosition.x > BackEnd::GetWindowCurrentWidth() + shipWidth)
            monkeyPosition.x = -shipWidth;
        if (monkeyPosition.y > BackEnd::GetWindowCurrentHeight() + shipHeight)
            monkeyPosition.y = -shipHeight;

        // Update visual representation of ship
        monkeyShip.SetPosition(monkeyPosition);

        // Make the ship blink when invulnerable
        if (invulnerable) {
            // Only draw the ship every other frame when invulnerable
            if (static_cast<int>(invulnerabilityTimer * 10.0f) % 2 == 0) {
                monkeyShip.Draw();
            }
        }
        else {
            monkeyShip.Draw();
        }

        // Update and draw asteroids (includes collision detection)
        UpdateAsteroids(dt);

        // Update and draw health bar
        UpdateHealthBar();

        return true;
    }

    void Clear() {
        // Clean up all asteroids
        for (auto& asteroid : asteroids) {
            if (asteroid.object) {
                delete asteroid.object;
                asteroid.object = nullptr;
            }
        }
        asteroids.clear();

        // Clean up health bar
        for (int i = 0; i < MAX_HEALTH; i++) {
            if (healthBar.LifeCount[i]) {
                delete healthBar.LifeCount[i];
                healthBar.LifeCount[i] = nullptr;
            }
        }
    }
}