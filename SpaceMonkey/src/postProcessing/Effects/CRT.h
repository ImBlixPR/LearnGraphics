#pragma once


#include <GLFW/glfw3.h>

#include "postProcessing/ScreenEffect.h"

#include "BackEnd/BackEnd.h"

class CRT : public ScreenEffect
{
public:
	CRT()
		:ScreenEffect("GL_CRT.vert", "GL_CRT.frag",
			BackEnd::GetWindowCurrentWidth(), BackEnd::GetWindowCurrentHeight())
	{
		startTime = glfwGetTime();
	}

	virtual void HotReload() override
	{
		FrameBufferShader.Load("GL_CRT.vert", "GL_CRT.frag");
	}

	virtual void ApplyEffect() override
	{
		FrameBufferShader.Use();
		float currentTime = glfwGetTime() - startTime;
		FrameBufferShader.SetFloat("time", currentTime);
		FrameBufferShader.SetVec2("resolution", glm::vec2(screenWidth,
			screenHeight));
	}
};