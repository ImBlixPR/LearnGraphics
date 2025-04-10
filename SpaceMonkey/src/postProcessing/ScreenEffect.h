#pragma once


#include <shader/GL_Shader.h>
class ScreenEffect
{
protected:
	Shader FrameBufferShader;
	float startTime = 0.0f;
	int screenWidth;
	int screenHeight;


public:
	ScreenEffect(std::string vertShader, std::string fragShader,
		int width, int height) 
		: screenWidth(width), screenHeight(height)
	{
		FrameBufferShader.Load(vertShader, fragShader);
	}

	virtual void HotReload() = 0;
	virtual void ApplyEffect() = 0;
};
