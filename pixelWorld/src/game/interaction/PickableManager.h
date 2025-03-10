#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "game/types/Object.h"

class PickableManager {
public:
    // Singleton pattern (optional)
    static PickableManager& GetInstance();

    // Add objects to be managed for picking/dragging
    void RegisterObject(Object* object);
    void UnregisterObject(Object* object);

    // Custom callbacks for drag events
    using DragStartCallback = std::function<void(Object*, const glm::vec2&)>;
    using DragUpdateCallback = std::function<void(Object*, const glm::vec2&, const glm::vec2&)>;
    using DragEndCallback = std::function<void(Object*, const glm::vec2&)>;

    // Set callbacks for specific objects
    void SetDragCallbacks(Object* object,
        DragStartCallback onStart = nullptr,
        DragUpdateCallback onUpdate = nullptr,
        DragEndCallback onEnd = nullptr);

    // Update all managed objects
    void Update();

    //draw pickiable object
    void Draw();

    // Check if any object is being dragged
    bool IsAnyObjectDragging() const;

    // Get currently dragged object (if any)
    Object* GetDraggedObject() const;

    // Configuration options
    void SetDragEnabled(bool enabled);
    void SetZOrderDragging(bool bringToFront);

    std::vector<Object*>& GetManagedObjects() {
        return m_managedObjects;
    }

    // Find topmost object under position
    Object* FindObjectAtPosition(const glm::vec2& position);
    int FindObjectIndex(Object* object);

private:
    static PickableManager* m_Instanc;
    std::vector<Object*> m_managedObjects;
    Object* m_currentDraggedObject = nullptr;
    Object* m_hoveredObject = nullptr;
    bool m_dragEnabled = true;
    bool m_bringToFrontWhenDragged = true;

    // Callback maps
    struct ObjectCallbacks {
        DragStartCallback onDragStart;
        DragUpdateCallback onDragUpdate;
        DragEndCallback onDragEnd;
    };

    std::unordered_map<Object*, ObjectCallbacks> m_objectCallbacks;

    // Internal drag state
    glm::vec2 m_dragStartPos;
    glm::vec2 m_lastDragPos;



    // Singleton implementation
    PickableManager() = default;
    ~PickableManager() = default;
    PickableManager(const PickableManager&) = delete;
    PickableManager& operator=(const PickableManager&) = delete;
};
