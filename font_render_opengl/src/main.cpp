#include <iostream>


#include <GLFW/glfw3.h>
#include <glad/glad.h>


#include "BackEnd/BackEnd.h"
#include "BackEnd/Log.h"


//initialize the glfw


int main()
{
    
	BackEnd::Init();

	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	MK_ASSERT(BackEnd::GetWindowPointer(), "the window do not initilize!")

		while (BackEnd::WindowIsOpen())
		{
			BackEnd::StartFrame();
			BackEnd::UpdateSubSystem();
			BackEnd::HandleSubSystem();
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


			glClear(GL_COLOR_BUFFER_BIT);  // Added clearing depth buffer
			BackEnd::EndFrame();
		}
	return false;
    

	return 0;
}



