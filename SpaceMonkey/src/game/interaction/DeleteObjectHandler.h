#pragma once

#include <vector>
#include <string>


#include "game/types/Object.h"
#include "game/interaction/PickableManager.h"

class PickableTrash
{
public:
	PickableTrash(std::string texturepath,
		glm::vec2 position,
		float width = 20.0f, float height = 20.0f);

	PickableTrash(glm::vec3 solidColor,
		glm::vec2 position,
		float width = 20.0f, float height = 20.0f);
	void DeleteByThrowInLocation(PickableManager* pickableManager);
	void Draw();
private:
	glm::vec2 m_trashPostion = glm::vec2(0.0f);
	glm::vec3 m_solidColor = glm::vec3(2.0f);
	float m_width = 20.0f, m_height = 20.0f;
	std::unique_ptr<Object> m_trash;
};