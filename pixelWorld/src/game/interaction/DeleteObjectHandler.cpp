#include "DeleteObjectHandler.h"


#include <memory>

#include <glm/glm.hpp>







PickableTrash::PickableTrash(std::string texturepath, glm::vec2 position, float width, float height)
{
    m_trashPostion = position;
    m_width = width;
    m_height = height;
    m_trash = std::make_unique<Object>(texturepath, position, 0.0f, m_width, m_height);
}

PickableTrash::PickableTrash(glm::vec3 solidColor, glm::vec2 position, float width, float height)
{
    m_trashPostion = position;
    m_width = width;
    m_height = height;
    m_trash = std::make_unique<Object>(solidColor, position, 0.0f, m_width, m_height);
}
void PickableTrash::DeleteByThrowInLocation(PickableManager* pickableManager)
{
    Object* trashedObject = pickableManager->FindObjectAtPosition(m_trashPostion);
    if (trashedObject == nullptr) return;

    if (!trashedObject->IsDragging())
    {
        // First unregister from the manager to prevent any further interactions
        pickableManager->UnregisterObject(trashedObject);

        // Get the object ID for potential notification to other systems
        int objectId = trashedObject->GetId();

        // Play optional delete animation/sound
        //g_trash->SetColor(glm::vec3(1.0f, 0.3f, 0.3f)); // Briefly highlight trash

        // Notify any listeners that might need to know about this deletion
        // EventSystem::TriggerEvent("ObjectDeleted", objectId);

        // Finally, delete the object from memory
        delete trashedObject;

        // Reset trash color after a brief delay (could be handled in an Update method)
        // g_trashResetTimer = 0.3f; // 300ms
    }
}
void PickableTrash::Draw()
{
    m_trash->Draw();
}


