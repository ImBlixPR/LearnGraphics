#pragma once

#include <GLFW/glfw3.h>

#include "postProcessing/ScreenEffect.h"

#include "BackEnd/BackEnd.h"

class VHS : public ScreenEffect
{
public:
	VHS()
		:ScreenEffect("GL_VHS.vert", "GL_VHS.frag", 
			BackEnd::GetWindowCurrentWidth(), BackEnd::GetWindowCurrentHeight())
	{
		startTime = glfwGetTime();
	}

	virtual void HotReload() override
	{
		FrameBufferShader.Load("GL_VHS.vert", "GL_VHS.frag");
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