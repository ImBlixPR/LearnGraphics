#pragma once

#include <glm/glm.hpp>


namespace Renderer
{


	void RenderFrame();



	//render 2d
	void DotedBackground(
		glm::vec3 color,
		float size = 2.0f,
		int verticalGap = 20,
		int horizantialGap = 40
	);

	void DrawLine(
		glm::vec2 start,
		glm::vec2 end,
		float quadSize = 1.0f,
		float gap = 0.0f,
		glm::vec3 color = glm::vec3(0.0f)
	);


	void OneWayArrow(
		glm::vec2 start,
		glm::vec2 end,
		float quadSize = 3.0f,
		float gap = 0.0f,
		glm::vec3 color = glm::vec3(0.0f),
		float tipLength = 10.0f,  // Negative value uses auto-scaling
		float tipAngle = 90.0f,   // Tip wing angle in degrees
		float tipThickness = 3.0f // Negative value uses quadSize
	);

	void DrawCircle(
		glm::vec2 originPosition,
		glm::vec3 color,
		float radius = 20.0f,
		float lineThickness = 5.0f,
		float lineGap = 0.0f, 
		int segments = 32 // more segments smooth cirle
	);

	void DrawArc(
		glm::vec2 originPosition,
		glm::vec3 color,
		float arcAngle,
		float radius = 20.0f,
		float lineThickness = 5.0f,
		float lineGap = 0.0f,
		int segments = 32 // more segments smooth cirle
	);
}
