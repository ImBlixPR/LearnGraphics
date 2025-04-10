#include "Game.h"
#include "Input/Input.h"
#include "BackEnd/BackEnd.h"
#include "scene/Scene.h"
#include "game/types/PickableObject.h"
#include "game/types/Object.h"
#include "game/interaction/PickableManager.h"
#include "game/interaction/DeleteObjectHandler.h"
#include "util/LerpUtil.h"
#include "utility/MeskUtil.h"
#include <vector>
#include <random>
#include <ctime>

#include "Opengl/GL_renderer.h"
#include "AssetManger/AssetManger.h"
#include "renderer/Renderer.h"

namespace Game
{
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> sizeDistribution;
    std::uniform_real_distribution<float> speedDistribution;
    std::uniform_real_distribution<float> angleDistribution;
    std::uniform_real_distribution<float> posDistribution;
    std::uniform_real_distribution<float> fruitSpawnPosition;
    std::uniform_int_distribution<int> spawnSideDistribution;
    std::uniform_int_distribution<int> spawnFruitTypeDistribution;

    // Asteroid struct to manage asteroid properties
    struct Asteroid {
        Object object;
        glm::vec2 velocity;
        float rotationSpeed;
        float size;
        bool active;
        bool init;

        Asteroid() : velocity(0.0f), rotationSpeed(0.0f), size(0.0f), active(false), init(false) {}

        ~Asteroid() {

        }
    };

    //callectable fruit struct
    struct Fruit
    {
        Object object;
        bool active;
        bool init;

        Fruit() :active(false), init(false) {}
        ~Fruit()
        {

        }
    };

    const int MAX_HEALTH = 5;     // Maximum health capacity
    int playerHealth = MAX_HEALTH;         // Player starts with 3 health points
    // Health struct to manage healthbar properties
    struct HealthBar
    {
        Object m_healthbar;
        Object LifeCount[MAX_HEALTH]; //red solid object behinde the m_healthbar 
        bool LifeCountActive[MAX_HEALTH] = { false }; //red solid object behinde the m_healthbar 
        bool init = false;
        
    };
    HealthBar healthBar;          // Health bar instance
    bool invulnerable = false;    // Invulnerability after taking damage
    float invulnerabilityTimer = 0.0f;  // Timer for invulnerability period
    const float INVULNERABILITY_DURATION = 2.0f;  // Duration of invulnerability in seconds
    bool gameOver = false;        // Game over state

    // Global variables for the game
    Object monkeyShip;
    // Dash ability parameters
    const float DASH_FORCE = 1500.0f;        // Maximum dash speed
    const float DASH_DURATION = 0.3f;        // How long the dash lasts (seconds)
    const float DASH_COOLDOWN = 1.5f;        // Time until dash can be used again (seconds)
    bool isDashing = false;                  // Whether the monkey is currently dashing
    float dashTimer = 0.0f;                  // Timer for current dash
    float dashCooldownTimer = 0.0f;          // Timer for dash cooldown
    bool dashReady = true;                   // Is dash ability ready to use
    glm::vec2 dashDirection;                 // Direction of the current dash
    glm::vec2 dashStartPosition;             // Position when dash started
    glm::vec2 dashTargetPosition;            // Target position at end of dash
    float prevCooldownPercentage = 1.0f;

    unsigned int defeatedId;
    unsigned int victoryId;

    //ui staff
    Object ui_start;
    Object  ui_exit;
    float ui_sizeWidth = 500;
    float ui_sizeHeight = ui_sizeWidth / 2;
    bool running = false;
    glm::vec2 startPos = glm::vec2(BackEnd::GetWindowCurrentWidth() / 2 - ui_sizeHeight,
        BackEnd::GetWindowCurrentHeight() / 2 - 200);
    glm::vec2 exitPos = glm::vec2(BackEnd::GetWindowCurrentWidth() / 2 - ui_sizeHeight,
        BackEnd::GetWindowCurrentHeight() / 2 + 50);
    unsigned int ui_spaceMonkey;
    unsigned int ui_spaceMonkeyBaner;
    bool isTransitioning = false;
    float transitionTimer = 0.0f;
    const float TRANSITION_DURATION = 1.5f; // Total duration of the transition
    const float GROW_DURATION = 0.6f;      // Duration of square growth
    const float HOLD_DURATION = 0.3f;      // Duration to hold the full screen
    const float FADE_DURATION = 0.6f;      // Duration of fade out

    unsigned int fruitListId[2];
    float pulseTimer = 0.0f;

    //fruit constants
    const int WIN_FRUIT_NUMBER = 30;
    const int MAX_FRUIT = 6;          // Maximum number of fruit at once
    const float FRUIT_SPAWN_TIMER_MAX = 1.0f; // Seconds between fruit spawns
    float fruitSpawnTimer = 0.0f;       // Timer for asteroid spawning
    //fruit vector
    const int FRUIT_POOL_SIZE = MAX_FRUIT + 5; // A bit larger than MAX_FRUIT to have some buffer
    Fruit fruitPool[FRUIT_POOL_SIZE];
    int activeFruitCount = 0;
    
    std::vector<Asteroid> asteroids;
    unsigned int collectedFruit = 0; //number of collected fruit
    glm::vec2 monkeyPosition;
    glm::vec2 monkeyVelocity = glm::vec2(0.0f, 0.0f); // Add velocity vector

    bool firstlunch = true;

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
        spawnFruitTypeDistribution = std::uniform_int_distribution<int>(0, 1); // 0=bannan, 1=pary
        fruitSpawnPosition = std::uniform_real_distribution<float>(0.1f, 0.9f);
        

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
            if (!asteroid.init) {
                asteroid.object = { "res/textures/asteroid.png", position, 0.0f, size, size };
                asteroid.init = true;
            }

            
            asteroid.object.SetPosition(position);
            asteroid.object.SetScale(size, size);

            asteroid.velocity = direction * speed;
            asteroid.rotationSpeed = rotationSpeed;
            asteroid.size = size;
            asteroid.active = true;
        }
    }

    //fruit spawn logic
    void SpawnFruit() {
        // Only spawn if we haven't reached the maximum active fruits
        if (activeFruitCount >= MAX_FRUIT)
            return;

        // Find an inactive fruit in the pool
        int inactiveIndex = -1;
        for (int i = 0; i < FRUIT_POOL_SIZE; i++) {
            if (!fruitPool[i].active) {
                inactiveIndex = i;
                break;
            }
        }

        // If no inactive fruit found, don't spawn
        if (inactiveIndex == -1)
            return;

        // Determine spawn position
        float windowWidth = static_cast<float>(BackEnd::GetWindowCurrentWidth());
        float windowHeight = static_cast<float>(BackEnd::GetWindowCurrentHeight());
        glm::vec2 position = glm::vec2(fruitSpawnPosition(rng) * windowWidth,
            fruitSpawnPosition(rng) * windowHeight);

        // Select fruit type
        int typeOfFruit = spawnFruitTypeDistribution(rng);

        // Initialize or reuse the fruit object
        Fruit& fruit = fruitPool[inactiveIndex];

        // Clean up previous object if exists
        if (!fruit.init) {
            // Create new object
            fruit.object = { std::vector<std::string>{"res/textures/fruit1.png", "res/textures/fruit2.png"}, position, 0.0f, 50.0f, 50.0f };
            fruit.init = true;
        }

        if (typeOfFruit) {
            fruit.object.SetActiveTexture(0);
        }
        else {
            fruit.object.SetActiveTexture(1);
        }
        fruit.object.SetPosition(position);
        fruit.active = true;
        activeFruitCount++;
    }

    // Check if asteroid is outside the screen with a margin
    bool IsAsteroidOutOfBounds(const Asteroid& asteroid) {
        if (!asteroid.active || !asteroid.init) return false;

        glm::vec2 pos = asteroid.object.GetPosition();
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

        if (!healthBar.init)
        {
            for (int i = 0; i < MAX_HEALTH; i++) {
                healthBar.LifeCount[i] = { glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec2(padding + 15.0f + (i * healthWidth) + (i * 5.0f),
                        padding + 40.0f),
                    0.0f, healthWidth, healthHeight - barHeight / 2 };
                healthBar.LifeCountActive[i] = true;
                }
            healthBar.init = true;
        }
        for (int i = 0; i < MAX_HEALTH; i++) {
            healthBar.LifeCountActive[i] = true;
        }


    }

    void UpdateHealthBar() {
        // Draw active health units
        for (int i = 0; i < playerHealth; i++) {
            if(healthBar.LifeCountActive[i])
                healthBar.LifeCount[i].Draw();
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
            if (asteroid.active && asteroid.init) {
                // Update position
                glm::vec2 position = asteroid.object.GetPosition();
                position += asteroid.velocity * dt;
                asteroid.object.SetPosition(position);

                // Update rotation
                float rotation = asteroid.object.GetAngle();
                rotation += asteroid.rotationSpeed * dt;
                asteroid.object.SetAngle(rotation);

                // Check collision with player
                if (!invulnerable && !gameOver) {
                    float asteroidRadius = asteroid.size / 2.0f;
                    if (CheckCollision(monkeyPosition, playerRadius, position, asteroidRadius)  /*imblix*/) {
                        PlayerTakeDamage();
                    }
                }

                // Check if out of bounds
                if (IsAsteroidOutOfBounds(asteroid)) {
                    asteroid.active = false;
                }

                // Draw if active
                if (asteroid.active) {
                    asteroid.object.Draw();
                }
            }
        }
    }

    void UpdateFruits(float dt) {
        // Update spawn timer
        fruitSpawnTimer -= dt;
        if (activeFruitCount < MAX_FRUIT && fruitSpawnTimer <= 0.0f) {
            SpawnFruit();
            fruitSpawnTimer = FRUIT_SPAWN_TIMER_MAX;
        }

        // Get player collision radius
        float playerRadius = shipWidth / 2.5f;

        // Update each fruit
        for (int i = 0; i < FRUIT_POOL_SIZE; i++) {
            Fruit& fruit = fruitPool[i];

            if (fruit.active && fruit.init) {
                // Check for collision with player
                if (!invulnerable && !gameOver) {
                    if (CheckCollision(monkeyPosition, playerRadius,
                        fruit.object.GetPosition(),
                        fruit.object.GetScale().x / 2.0f)) {
                        // Deactivate fruit instead of deleting
                        fruit.active = false;
                        activeFruitCount--;
                        collectedFruit++;
                    }
                }


                // Draw the fruit
                fruit.object.Draw();
            }
        }
    }

    void RenderDashCooldown(float pulseTimer) {
        // Calculate current cooldown percentage
        float targetPercentage = dashReady ? 1.0f : (1.0f - (dashCooldownTimer / DASH_COOLDOWN));

        // Smoothly lerp to target percentage for a nicer visual
        prevCooldownPercentage = Lerp::smoothLerp(prevCooldownPercentage, targetPercentage, 0.1f);

        // Draw a small indicator in the corner
        float barWidth = 100.0f;
        float barHeight = 10.0f;
        float padding = 10.0f;
        float windowHeight = static_cast<float>(BackEnd::GetWindowCurrentHeight());

        // Background bar (empty)
        OpenglRenderer::Render2dQuad(
            glm::vec2(padding, windowHeight - padding - barHeight),
            glm::vec3(0.2f, 0.2f, 0.2f),
            barWidth,
            barHeight
        );

        // Determine color based on readiness
        glm::vec3 fillColor;
        if (dashReady) {
            fillColor = glm::vec3(0.0f, 1.0f, 0.4f); // Green when ready
        }
        else {
            // Transition from orange to green as it fills
            fillColor = glm::mix(
                glm::vec3(1.0f, 0.5f, 0.0f), // Orange
                glm::vec3(0.0f, 1.0f, 0.4f), // Green
                prevCooldownPercentage
            );
        }

        // Filled portion (available dash)
        OpenglRenderer::Render2dQuad(
            glm::vec2(padding, windowHeight - padding - barHeight),
            fillColor,
            barWidth * prevCooldownPercentage,
            barHeight
        );

        // Add a pulsing effect when ready
        if (dashReady) {
            // Create a subtle pulsing effect using sin wave
            float pulseScale = 0.1f * (sinf(pulseTimer * 5.0f) * 0.5f + 0.5f);


            OpenglRenderer::Render2dQuad(
                glm::vec2(padding - 2.0f, windowHeight - padding - barHeight - 2.0f),
                fillColor,
                barWidth + 4.0f,
                barHeight + 4.0f
            );
        }
    }

    void DrawDashEffect() {
        if (isDashing) {
            // Calculate how far into the dash we are (0 to 1)
            float dashProgress = dashTimer / DASH_DURATION;

            // Number of trail segments
            const int trailCount = 5;

            for (int i = 1; i <= trailCount; i++) {
                // Calculate position along the dash path
                float trailFactor = dashProgress - (i * 0.1f);

                // Only draw trail segments that are within the dash path
                if (trailFactor >= 0.0f) {
                    // Use the same interpolation as the dash movement for consistency
                    glm::vec2 trailPos = Lerp::smootherLerp(dashStartPosition, dashTargetPosition, trailFactor);

                    // Calculate alpha and size based on position in trail
                    float alpha = 0.6f * (1.0f - (float)i / trailCount);
                    float size = shipWidth * (1.0f - (0.15f * i));

                    //// Draw a semi-transparent quad at trail position
                    OpenglRenderer::Render2dCircle(
                        glm::vec2(trailPos.x +size/2,trailPos.y + size/2),
                        glm::vec4(0.8f, 0.8f, 1.0f, alpha),//add alph in future
                        size/2
                    );
                }
            }

            // Add a burst effect at the start of the dash
            if (dashProgress < 0.2f) {
                float burstSize = shipWidth * (1.0f + dashProgress * 2.0f);
                float burstAlpha = 0.5f * (1.0f - dashProgress * 5.0f);


                OpenglRenderer::Render2dCircle(
                    glm::vec2(dashStartPosition.x + burstSize/2,
                        dashStartPosition.y+burstSize/2),
                    glm::vec4(1.0f, 1.0f, 1.0f, burstAlpha), // add burstAlpha future
                    burstSize/2
                );
            }
        }
    }


    void UpdateTransition(float dt) {
        if (!isTransitioning) {
            return;
        }


        // Calculate the center of the screen
        float centerX = BackEnd::GetWindowCurrentWidth() / 2.0f;
        float centerY = BackEnd::GetWindowCurrentHeight() / 2.0f;


        transitionTimer += dt;
        //printf("delta time = %.4f\n", transitionTimer);
        // Determine which phase of the transition we're in
        if (transitionTimer < GROW_DURATION) {
            // Phase 1: Grow square from center
            float progress = transitionTimer / GROW_DURATION;
            progress = Lerp::smootherLerp(0.0f, 1.0f, progress); // Use smoother interpolation

            // Calculate the square size that grows from center to screen size
            float maxSize = std::max(BackEnd::GetWindowCurrentWidth(), BackEnd::GetWindowCurrentHeight()) * 1.5f;
            float size = progress * maxSize;

            // Render the growing square
            OpenglRenderer::Render2dQuad(
                glm::vec2(centerX - size / 2, centerY - size / 2),
                glm::vec3(0.0f, 0.0f, 0.0f), // Black
                size,
                size
            );
        }
        else if (transitionTimer < GROW_DURATION + HOLD_DURATION) {
            // Phase 2: Hold full screen black
            OpenglRenderer::Render2dQuad(
                glm::vec2(0, 0),
                glm::vec3(0.0f, 0.0f, 0.0f), // Black
                static_cast<float>(BackEnd::GetWindowCurrentWidth()),
                static_cast<float>(BackEnd::GetWindowCurrentHeight())
            );
            running = true; // Start the game during the hold period
        }
        else if (transitionTimer < TRANSITION_DURATION) {
            // Phase 3: Fade out
            float fadeProgress = (transitionTimer - GROW_DURATION - HOLD_DURATION) / FADE_DURATION;
            fadeProgress = Lerp::smootherLerp(0.0f, 1.0f, fadeProgress);

            // Determine alpha based on fade progress
            float alpha = 1.0f - fadeProgress;

            // Render the fading square
            OpenglRenderer::Render2dQuad(
                glm::vec2(0, 0),
                glm::vec4(0.0f, 0.0f, 0.0f, alpha), // Black with fading alpha
                static_cast<float>(BackEnd::GetWindowCurrentWidth()),
                static_cast<float>(BackEnd::GetWindowCurrentHeight())
            );
        }
        else {
            // Transition complete
            isTransitioning = false;
        }
    }

    void Menu()
    {
        BackEnd::EnableCursor(true);
        if (!isTransitioning)
        {
            //start chick checker
            if (CheckCollision(
                glm::vec2(startPos.x + ui_sizeWidth / 2, startPos.y + ui_sizeHeight / 2),
                ui_sizeHeight / 2,
                Input::GetMousePos(), 3.0f
            ))
            {
                ui_start.SetHovering(true);
                if (Input::MouseButtonPressed(MK_MOUSE_BUTTON_LEFT)) {
                    // Start the transition instead of immediately setting running = true
                    isTransitioning = true;
                    transitionTimer = 0.0f;
                }
            }
            else
                ui_start.SetHovering(false);

            if (ui_start.IsHovering())
                ui_start.SetScale(ui_sizeWidth * 1.05, ui_sizeHeight * 1.05);
            else
                ui_start.SetScale(ui_sizeWidth, ui_sizeHeight);

            //exit chick checker
            if (CheckCollision(
                glm::vec2(exitPos.x + ui_sizeWidth / 2, exitPos.y + ui_sizeHeight / 2),
                ui_sizeHeight / 2,
                Input::GetMousePos(), 3.0f
            ))
            {
                ui_exit.SetHovering(true);
                if (Input::MouseButtonPressed(MK_MOUSE_BUTTON_LEFT))
                    BackEnd::ForceWindowClose();
            }
            else
                ui_exit.SetHovering(false);

            if (ui_exit.IsHovering())
                ui_exit.SetScale(ui_sizeWidth * 1.05, ui_sizeHeight * 1.05);
            else
                ui_exit.SetScale(ui_sizeWidth, ui_sizeHeight);


            Renderer::DrawCircle(glm::vec2(0.0f, BackEnd::GetWindowCurrentHeight()),
                glm::vec3(MiskUtil::GetNormalizeRGBColor(118.0f, 66.0f, 138.0f)), 700.0f, 20.0f);
            Renderer::DrawCircle(glm::vec2(BackEnd::GetWindowCurrentWidth(), BackEnd::GetWindowCurrentHeight()/ 2),
                glm::vec3(MiskUtil::GetNormalizeRGBColor(203.0f, 211.0f, 252.0f)), 400.0f, 20.0f);
            Renderer::DrawCircle(glm::vec2(BackEnd::GetWindowCurrentWidth() / 2, BackEnd::GetWindowCurrentHeight()),
                glm::vec3(1.0f, 1.0f, 1.0f), 400.0f, 20.0f);
            OpenglRenderer::Render2dTexture(
                ui_spaceMonkeyBaner, glm::vec2(BackEnd::GetWindowCurrentWidth() / 2 - 300.0f, 5.0f),
                600.0f, 300.0f
            );

            ui_start.Draw();
            ui_exit.Draw();
        }
        //UpdateTransitionFadeIn(BackEnd::GetDeltaTime());
    }

    bool Init() {
        if (firstlunch) {
            // Initialize the ship
            monkeyShip = { "res/textures/monkeySpaceShip.png", glm::vec2(100.0f, 100.0f), 0.0f, 100.0f, 100.0f };

            // Load textures
            defeatedId = AssetManger::LoadTexture("res/textures/defeated.png");
            victoryId = AssetManger::LoadTexture("res/textures/victory.png");
            fruitListId[0] = AssetManger::LoadTexture("res/textures/fruit1.png");
            fruitListId[1] = AssetManger::LoadTexture("res/textures/fruit2.png");

            //ui initialize variable
            startPos = glm::vec2(BackEnd::GetWindowCurrentWidth() / 2 - ui_sizeHeight,
                BackEnd::GetWindowCurrentHeight() / 2 - 130);
            exitPos = glm::vec2(BackEnd::GetWindowCurrentWidth() / 2 - ui_sizeHeight,
                BackEnd::GetWindowCurrentHeight() / 2 + 100);
            ui_start = { "res/textures/start.png" , startPos, 0.0f, ui_sizeWidth, ui_sizeHeight };
            ui_exit = { "res/textures/exit.png" , exitPos, 0.0f, ui_sizeWidth, ui_sizeHeight };
            ui_spaceMonkey = AssetManger::LoadTexture("res/textures/monkeySpaceShip.png");
            ui_spaceMonkeyBaner = AssetManger::LoadTexture("res/textures/ui_baner2.png");
            // Initialize RNG
            InitRNG();

            // Reset dash state
            isDashing = false;
            dashTimer = 0.0f;
            dashCooldownTimer = 0.0f;
            dashReady = true;
            dashStartPosition = glm::vec2(0.0f);
            dashTargetPosition = glm::vec2(0.0f);
            prevCooldownPercentage = 1.0f;

            pulseTimer = 0.0f;

            // Reserve asteroid container
            asteroids.reserve(MAX_ASTEROIDS);

            //isTransitioning = false;
            //transitionTimer = 0.0f;

            firstlunch = false;
        }

        // Reset ship position
        monkeyPosition = glm::vec2(100.0f, 100.0f);
        monkeyShip.SetPosition(monkeyPosition);
        monkeyVelocity = glm::vec2(0.0f, 0.0f);

        // Initialize health bar (do this every time)
        InitHealthBar();

        // Initialize fruit pool
        for (int i = 0; i < FRUIT_POOL_SIZE; i++) {
            fruitPool[i].active = false;
        }

        // Reset game state
        playerHealth = MAX_HEALTH;
        invulnerable = false;
        invulnerabilityTimer = 0.0f;
        gameOver = false;
        collectedFruit = 0;
        activeFruitCount = 0;

        return true;
    }

    void PlayAgain() {
        if (Input::KeyPressed(MK_KEY_R)) {
            Clear();
            Init();
        }
    }

    // Helper function to keep positions within screen bounds
    void ClampPositionToScreen(glm::vec2& position) {
        float windowWidth = static_cast<float>(BackEnd::GetWindowCurrentWidth());
        float windowHeight = static_cast<float>(BackEnd::GetWindowCurrentHeight());

        // Wrap around screen edges (tiling)
        if (position.x < -shipWidth)
            position.x = windowWidth;
        if (position.y < -shipHeight)
            position.y = windowHeight;
        if (position.x > windowWidth + shipWidth)
            position.x = -shipWidth;
        if (position.y > windowHeight + shipHeight)
            position.y = -shipHeight;
    }

    void Run(float dt) {

        BackEnd::EnableCursor(false);
        if (isTransitioning) {
            if (transitionTimer < GROW_DURATION + HOLD_DURATION) return;
        }
        //return to the menu
        if (Input::KeyPressed(MK_KEY_M))
        {
            running = false;
            Clear();
            Init();
            return;
        }
        // Skip game logic if game over
        if (gameOver) {
            // Display game over message or screen
            float messageWidth = 400;
            float messageHeight = 200;

            OpenglRenderer::Render2dTexture(defeatedId, glm::vec2(BackEnd::GetWindowCurrentWidth() / 2 - messageWidth / 2,
                BackEnd::GetWindowCurrentHeight() / 2 - messageHeight / 2), messageWidth, messageHeight);
            PlayAgain();
            return;
        }

        if (collectedFruit >= WIN_FRUIT_NUMBER)
        {
            // Display game over message or screen
            float messageWidth = 400;
            float messageHeight = 200;

            OpenglRenderer::Render2dTexture(victoryId, glm::vec2(BackEnd::GetWindowCurrentWidth() / 2 - messageWidth / 2,
                BackEnd::GetWindowCurrentHeight() / 2 - messageHeight / 2), messageWidth, messageHeight);
            PlayAgain();
            return;
        }



        //trik the time
        pulseTimer += dt;
        if (pulseTimer > 6.28f) // Reset after 2π
            pulseTimer = 0.0f;
        
        // Update invulnerability timer
        if (invulnerable) {
            invulnerabilityTimer -= dt;
            if (invulnerabilityTimer <= 0.0f) {
                invulnerable = false;
            }
        }

        // Update dash cooldown
        if (!dashReady) {
            dashCooldownTimer -= dt;
            if (dashCooldownTimer <= 0.0f) {
                dashReady = true;
            }
        }

        // Handle dash ability
        if (isDashing) {
            // Update dash timer
            dashTimer += dt;

            if (dashTimer >= DASH_DURATION) {
                // End dash
                isDashing = false;
                // Set the final position and resume normal movement
                monkeyPosition = dashTargetPosition;
            }
            else {
                // Calculate interpolation factor (0 to 1)
                float t = dashTimer / DASH_DURATION;

                // Use smoother step interpolation for more dynamic dash
                // Start slow, accelerate in the middle, then decelerate at the end
                monkeyPosition = Lerp::smootherLerp(dashStartPosition, dashTargetPosition, t);

                // Reset velocity at end of dash to avoid sudden changes
                if (t > 0.9f) {
                    // Gradually blend to a velocity in the dash direction
                    monkeyVelocity = dashDirection * MAX_SPEED * 0.7f;
                }
            }
        }
        else {
            // Check for dash input
            if (dashReady && Input::KeyPressed(MK_KEY_SPACE)) {
                // Get current movement direction, or use the facing direction if not moving
                glm::vec2 dashDir = glm::vec2(0.0f, 0.0f);

                // If there's input, dash in that direction
                if (Input::KeyDown(MK_KEY_W)) dashDir.y -= 1.0f;
                if (Input::KeyDown(MK_KEY_S)) dashDir.y += 1.0f;
                if (Input::KeyDown(MK_KEY_A)) dashDir.x -= 1.0f;
                if (Input::KeyDown(MK_KEY_D)) dashDir.x += 1.0f;

                // If no direction input, dash in the direction of current velocity
                if (glm::length(dashDir) < 0.1f && glm::length(monkeyVelocity) > 0.1f) {
                    dashDir = glm::normalize(monkeyVelocity);
                }

                // If we have a valid direction, start dash
                if (glm::length(dashDir) > 0.1f) {
                    dashDirection = glm::normalize(dashDir);

                    // Calculate dash start and end positions
                    dashStartPosition = monkeyPosition;

                    // Determine dash distance (could be a constant or based on screen size)
                    float dashDistance = 300.0f; // Adjust as needed
                    dashTargetPosition = dashStartPosition + (dashDirection * dashDistance);

                    // Start dash
                    isDashing = true;
                    dashReady = false;
                    dashTimer = 0.0f;
                    dashCooldownTimer = DASH_COOLDOWN;

                    // Optional: Give a short burst of invulnerability during dash
                    invulnerable = true;
                    invulnerabilityTimer = DASH_DURATION + 0.2f; // Slightly longer than dash
                }
            }

            // Normal movement controls (only process if not dashing)
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
        }

        // Boundary checks for dash target position and current position
        // This ensures the dash target doesn't go off-screen
        ClampPositionToScreen(dashTargetPosition);
        ClampPositionToScreen(monkeyPosition);

        // Update visual representation of ship
        monkeyShip.SetPosition(monkeyPosition);


        //update the fruit and draw
        UpdateFruits(dt);



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
       
        // Draw dash effect if dashing
        DrawDashEffect();

        // Update and draw asteroids (includes collision detection)
        UpdateAsteroids(dt);

        

        // Update and draw health bar
        UpdateHealthBar();

        // Add visual feedback for dash cooldown
        RenderDashCooldown(pulseTimer);

        //printf("number of fruit collected: %d\n", collectedFruit);
        //printf("number of fruit created: %d\n", sizeof(fruitPool) / sizeof(Fruit));
        

    }

    void Clear() {
        // Clean up all asteroids
        for (auto& asteroid : asteroids) {
            if (asteroid.init) {
                //asteroid.init = false;
            }
            asteroid.active = false;
        }
        // Don't clear the vector itself since we've reserved it
        // Just mark them as inactive

        // Clean up the fruit pool
        for (int i = 0; i < FRUIT_POOL_SIZE; i++) {
            if (fruitPool[i].init) {
                //fruitPool[i].init = false;
            }
            fruitPool[i].active = false;
        }
        activeFruitCount = 0;

        // Clean up health bar
        for (int i = 0; i < MAX_HEALTH; i++) {
            if (healthBar.LifeCountActive[i]) {
                //healthBar.LifeCountActive[i] = false;
            }
        }
    }
    bool IsRunning()
    {
        return running;
    }

}