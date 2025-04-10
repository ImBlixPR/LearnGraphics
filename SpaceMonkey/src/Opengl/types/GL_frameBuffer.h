#pragma once

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


#include <BackEnd/BackEnd.h>
#include <shader/GL_Shader.h>


struct OpenglFrameBuffer
{

private:
	unsigned int FBO;
	unsigned int textureColorId;
	unsigned int RBO;
    unsigned int quadVAO;
    Shader frameBufferShader;
    std::string vertexShaderPath, fragmentShaderPath;
    float startTime = 0.0f;


public:


	void ConfigFrameBuffer() {
        //vertex buffer configration for screen quad
        float quadVertices[] = { 
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
        unsigned int quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        fragmentShaderPath = "GL_frameBuffer.frag";
        vertexShaderPath = "GL_frameBuffer.vert";
         
        frameBufferShader.Load(vertexShaderPath, fragmentShaderPath);

        unsigned int SCR_WIDTH = BackEnd::GetWindowCurrentWidth();
        unsigned int SCR_HEIGHT = BackEnd::GetWindowCurrentHeight();

        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        // create a color attachment texture
        glGenTextures(1, &textureColorId);
        glBindTexture(GL_TEXTURE_2D, textureColorId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorId, 0);
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        startTime = glfwGetTime();
	}

    void HotReloadShader(std::string vertexShader, std::string fragmentShader)
    {
        frameBufferShader.Load(vertexShader, fragmentShader);
    }
    void HotReloadShader()
    {
        frameBufferShader.Load(vertexShaderPath, fragmentShaderPath);
    }
    
    void Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glEnable(GL_DEPTH_TEST);
    }
    void Draw()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); 
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);

        //frameBufferShader.Use();
        //float currentTime = glfwGetTime() - startTime;
        //frameBufferShader.SetFloat("time", currentTime);
        //frameBufferShader.SetVec2("resolution", glm::vec2(BackEnd::GetWindowCurrentWidth(),
        //    BackEnd::GetWindowCurrentHeight()));
        
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorId);	
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void ApplyNormalShader()
    {
        frameBufferShader.Use();
    }

};