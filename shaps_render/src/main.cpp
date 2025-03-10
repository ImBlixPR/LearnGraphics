
// Prevent accidentally selecting integrated GPU
extern "C" {
	__declspec(dllexport) unsigned __int32 AmdPowerXpressRequestHighPerformance = 0x1;
	__declspec(dllexport) unsigned __int32 NvOptimusEnablement = 0x1;
}

#include <iostream>


#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>


#include "BackEnd/BackEnd.h"
#include "BackEnd/Log.h"
#include "Opengl/GL_renderer.h"
#include "Opengl/types/GL_detachedMesh.h"
#include "AssetManger/AssetManger.h"


float ang = 0.0f;

int main()
{

	BackEnd::Init();
	MK_ASSERT(BackEnd::GetWindowPointer(), "the window do not initilize!");
	OpenglRenderer::Init();
	OpenglRenderer::HotLoadShaders();

	//temp
	OpenGLDetachedMesh myMesh;
	myMesh.UpdateVertexBuffer(
		std::vector<Vertex>
	{
		Vertex{ {0.5f, 0.3f, 0.0f} },    // top-right     (index 0)
			Vertex{ {0.8f, -0.4f, 0.0f} },    // top-left      (index 1)
			Vertex{ {-0.5f, 0.2f, 0.0f} },    // bottom-right  (index 2)
			Vertex{ {-0.5f, -0.5f, 0.0f} },     // bottom-left   (index 3)
			Vertex{ {0.0f, -0.8f, 0.0f} }     // bottom-left   (index 3)
	},

		std::vector<unsigned int>
	{
		0, 2, 1, // First triangle: top-right, top-left, bottom-right
			2, 3, 1,   // Second triangle: top-left, bottom-left, bottom-right
			1, 4, 3   // Second triangle: top-left, bottom-left, bottom-right
	}
	);

	//textured mesh
	unsigned int myTexture = AssetManger::LoadTexture("res/textures/monkeySpaceShip.png");



	while (BackEnd::WindowIsOpen())
	{
		//temp 
		ang += 1.0f;
		if (ang > 360.0f)
			ang = 0.0f;
		BackEnd::StartFrame();
		BackEnd::UpdateSubSystem();
		BackEnd::HandleSubSystem();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		OpenglRenderer::Render2dQuad(glm::vec2(100.0f, 100.0f), glm::vec3(1.0f, 0.0f, 0.0f), 100.0f,
			20.0f,ang, OriginPostion::Center, true);	
		OpenglRenderer::Render2dTriangle(glm::vec2(300.0f, 300.0f), glm::vec3(1.0f, 1.0f, 0.0f), 500.0f,
			200.0f,ang, true);
		OpenglRenderer::Render2dEquilateralTriangle(glm::vec2(400.0f, 200.0f), glm::vec3(0.0f, 1.0f, 1.0f), 300.0f,
			100.0f,ang, true);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		OpenglRenderer::Render2dCircle(glm::vec2(400.0f, 200.0f), ColorPreset::Lime, 100.0f
			, ang, 50, true);
		OpenglRenderer::Render2dCircle(glm::vec2(100.0f, 300.0f), ColorPreset::Navy, 30.0f
			, ang, 50, true);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		OpenglRenderer::Render2dMesh(&myMesh, glm::vec2(400.0f, 200.0f), glm::vec3(1.0f, 1.0f, 1.0f), 100.0f,
			100.0f, ang, true);

		// Basic texture rendering
		OpenglRenderer::Render2dTexture(myTexture, { 100.0f, 100.0f }, 200.0f, 200.0f);

		// Texture with rotation and specific origin
		OpenglRenderer::Render2dTexture(myTexture, { 200.0f, 200.0f }, 150.0f, 150.0f, 45.0f, OriginPostion::Center);

		// Texture with color tint
		OpenglRenderer::Render2dTexture(myTexture, { 300.0f, 300.0f }, { 1.0f, 0.5f, 0.5f }, 100.0f, 100.0f);
			
		BackEnd::EndFrame();
	}
	return false;


	return 0;
}



