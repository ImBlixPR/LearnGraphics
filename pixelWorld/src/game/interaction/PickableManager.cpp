#include "PickableManager.h"


#include "Input/Input.h"
#include "BackEnd/Log.h"



int PickableManager::FindObjectIndex(Object* object)
{
    if (!object) {
        return -1;  // Return -1 if the object pointer is null
    }

    // Find the object in the managed objects vector
    for (size_t i = 0; i < m_managedObjects.size(); ++i) {
        if (m_managedObjects[i] == object) {
            return static_cast<int>(i);  // Return the index when found
        }
    }

    // Return -1 if the object was not found in the managed objects
    return -1;
}

// Static instance getter implementation
PickableManager& PickableManager::GetInstance()
{
    static PickableManager instance;
    return instance;
}

// Register an object to be managed
void PickableManager::RegisterObject(Object* object)
{
    if (object) {
        // Check if object is already registered
        auto it = std::find(m_managedObjects.begin(), m_managedObjects.end(), object);
        if (it == m_managedObjects.end()) {
            m_managedObjects.push_back(object);
        }
    }
}

// Unregister an object
void PickableManager::UnregisterObject(Object* object)
{
    if (object) {
        auto it = std::find(m_managedObjects.begin(), m_managedObjects.end(), object);
        if (it != m_managedObjects.end()) {
            // If this is the currently dragged object, release it
            if (m_currentDraggedObject == object) {
                m_currentDraggedObject->SetDragging(false);
                m_currentDraggedObject = nullptr;
            }

            // If this is the hovered object, clear it
            if (m_hoveredObject == object) {
                m_hoveredObject = nullptr;
            }

            // Remove any callbacks
            m_objectCallbacks.erase(object);

            // Remove from managed objects
            m_managedObjects.erase(it);
        }
    }
}

void PickableManager::Update() {
    // Get input state
    glm::vec2 mousePos = Input::GetMousePos();
    bool mouseLeftPressed = Input::MouseButtonPressed(MK_MOUSE_BUTTON_LEFT);
    bool mouseLeftDown = Input::MouseButtonDown(MK_MOUSE_BUTTON_LEFT);
    bool mouseLeftReleased = Input::MouseButtonReleased(MK_MOUSE_BUTTON_LEFT);

    // Find object under cursor
    Object* objectUnderCursor = FindObjectAtPosition(mousePos);

    // Update hover state
    if (m_hoveredObject != objectUnderCursor) {
        if (m_hoveredObject) m_hoveredObject->SetHovering(false);
        m_hoveredObject = objectUnderCursor;
        if (m_hoveredObject) m_hoveredObject->SetHovering(true);
    }

    // Start dragging
    if (mouseLeftPressed && objectUnderCursor && m_dragEnabled && !m_currentDraggedObject) {
        m_currentDraggedObject = objectUnderCursor;
        m_dragStartPos = mousePos;
        m_lastDragPos = mousePos;

        // Calculate click offset relative to object center
        glm::vec2 objPos = glm::vec2(m_currentDraggedObject->GetPosition());
        glm::vec2 clickOffset = mousePos - objPos;
        m_currentDraggedObject->SetClickOffset(clickOffset);
        m_currentDraggedObject->SetDragging(true);

        // Bring to front if needed
        if (m_bringToFrontWhenDragged) {
            // Implementation depends on your rendering order system
            int index = FindObjectIndex(m_currentDraggedObject);
            if (index >= 0 && index < static_cast<int>(m_managedObjects.size()) - 1) {
                // Remove the object from its current position
                Object* draggedObj = m_managedObjects[index];
                m_managedObjects.erase(m_managedObjects.begin() + index);

                // Add it to the end of the vector (top of the render order)
                m_managedObjects.push_back(draggedObj);
            }
        }

        // Call start callback if registered
        auto it = m_objectCallbacks.find(m_currentDraggedObject);
        if (it != m_objectCallbacks.end() && it->second.onDragStart) {
            it->second.onDragStart(m_currentDraggedObject, mousePos);
        }
    }

    // Update dragging
    if (m_currentDraggedObject && mouseLeftDown) {
        // Calculate new position
        glm::vec2 dragOffset = mousePos - m_lastDragPos;
        glm::vec2 objPos = glm::vec2(m_currentDraggedObject->GetPosition());
        glm::vec2 newPos = objPos + dragOffset;

        // Update position
        m_currentDraggedObject->SetPosition(glm::vec3(newPos, 0.0f));

        // Call update callback if registered
        auto it = m_objectCallbacks.find(m_currentDraggedObject);
        if (it != m_objectCallbacks.end() && it->second.onDragUpdate) {
            it->second.onDragUpdate(m_currentDraggedObject, mousePos, dragOffset);
        }

        m_lastDragPos = mousePos;
    }

    // End dragging
    if (m_currentDraggedObject && mouseLeftReleased) {
        // Call end callback if registered
        auto it = m_objectCallbacks.find(m_currentDraggedObject);
        if (it != m_objectCallbacks.end() && it->second.onDragEnd) {
            it->second.onDragEnd(m_currentDraggedObject, mousePos);
        }

        m_currentDraggedObject->SetDragging(false);
        m_currentDraggedObject = nullptr;
    }
}

void PickableManager::Draw()
{
    for (auto* obj : m_managedObjects) {
        obj->Draw();
    }
}

Object* PickableManager::FindObjectAtPosition(const glm::vec2& position) {
    // Check objects in reverse order (top to bottom)
    for (auto it = m_managedObjects.rbegin(); it != m_managedObjects.rend(); ++it) {
        if ((*it)->IsInsideObject(position)) {
            return *it;
        }
    }
    return nullptr;
}

// Implementation for IsAnyObjectDragging
bool PickableManager::IsAnyObjectDragging() const
{
    return m_currentDraggedObject != nullptr;
}

// Implementation for GetDraggedObject
Object* PickableManager::GetDraggedObject() const
{
    return m_currentDraggedObject;
}

// Implementation for SetDragEnabled
void PickableManager::SetDragEnabled(bool enabled)
{
    m_dragEnabled = enabled;

    // If disabling dragging while an object is being dragged,
    // release the currently dragged object
    if (!enabled && m_currentDraggedObject) {
        m_currentDraggedObject->SetDragging(false);
        m_currentDraggedObject = nullptr;
    }
}

// Implementation for SetZOrderDragging
void PickableManager::SetZOrderDragging(bool bringToFront)
{
    m_bringToFrontWhenDragged = bringToFront;
}