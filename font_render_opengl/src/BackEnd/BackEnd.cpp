#include "BackEnd/BackEnd.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <string>


#include "Log.h"
#include "Input/Input.h"



namespace BackEnd
{

	/*
		           ,,                    ,,        ,,                                            ,,  ,,        ,,          
		         `7MM                   *MM      `7MM                                            db *MM      `7MM          
		           MM                    MM        MM                                                MM        MM          
		 .P"Ybmmm  MM  ,pW"Wq.   ,6"Yb.  MM,dMMb.  MM  .gP"Ya      `7M'   `MF',6"Yb.  `7Mb,od8 `7MM  MM,dMMb.  MM  .gP"Ya  
		:MI  I8    MM 6W'   `Wb 8)   MM  MM    `Mb MM ,M'   Yb       VA   ,V 8)   MM    MM' "'   MM  MM    `Mb MM ,M'   Yb 
		 WmmmP"    MM 8M     M8  ,pm9MM  MM     M8 MM 8M""""""        VA ,V   ,pm9MM    MM       MM  MM     M8 MM 8M"""""" 
		8M         MM YA.   ,A9 8M   MM  MM.   ,M9 MM YM.    ,         VVV   8M   MM    MM       MM  MM.   ,M9 MM YM.    , 
		 YMMMMMb .JMML.`Ybmd9'  `Moo9^Yo.P^YbmdP'.JMML.`Mbmmd'          W    `Moo9^Yo..JMML.   .JMML.P^YbmdP'.JMML.`Mbmmd' 
		6'     dP                                                                                                          
		Ybmmmd'                                                                                                            
	*/

	bool g_IsGLFWInitialize = false;
	GLFWmonitor* g_monitor;
	const GLFWvidmode* g_mode;
	int g_windowWidth = 0;
	int g_windowHeight = 0;
	int g_fullScreenWidth = 0;
	int g_fullScreenHeight = 0;
	WindowMode g_currentMode = WindowMode::UNDFIND;
	int g_currentWindowWidth = 0;
	int g_currentWindowHeight = 0;
	GLFWwindow* g_window;
	bool g_forceWindowClose = false;

//=========================================================================================================

	/*
		  .g8"""bgd   .g8""8q. `7MM"""Mq.  `7MM"""YMM  
		.dP'     `M .dP'    `YM. MM   `MM.   MM    `7  
		dM'       ` dM'      `MM MM   ,M9    MM   d    
		MM          MM        MM MMmmdM9     MMmmMM    
		MM.         MM.      ,MP MM  YM.     MM   Y  , 
		`Mb.     ,' `Mb.    ,dP' MM   `Mb.   MM     ,M 
		  `"bmmmd'    `"bmmd"' .JMML. .JMM..JMMmmmmMMM 

	*/

	void Init()
	{
		if (!g_IsGLFWInitialize)
		{
			int succes = glfwInit();
			MK_ASSERT(succes, "Could not intialize GLFW");
			//glfwSetErrorCallback(GLFWErrorCallBack);
			g_IsGLFWInitialize = true;
		}

		//window resulution in window mode
		int width = 1920 * 0.5;
		int height = 1080 * 0.5;
		//int width = 1920 * 0.75;
		//int height = 1080 * 0.75;

		// OpenGL version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		// Core Profile
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// Allow forward compatiblity
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		glfwSetErrorCallback([](int error, const char* description) { std::cout << "GLFW Error (" << std::to_string(error) << "): " << description << "\n"; });

		//context setting
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		//glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

				// Resolution and window size
		g_monitor = glfwGetPrimaryMonitor();
		g_mode = glfwGetVideoMode(g_monitor);
		glfwWindowHint(GLFW_RED_BITS, g_mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, g_mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, g_mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, g_mode->refreshRate);
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		g_fullScreenWidth = g_mode->width;
		g_fullScreenHeight = g_mode->height;
		g_windowWidth = width;
		g_windowHeight = height;

		//window
		CreateWindow(WindowMode::WINDOW);

		if (g_window == NULL)
		{
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(g_window);

		//initlize the sub-system
		Input::Init();

		glfwShowWindow(BackEnd::GetWindowPointer());
	}

	void StartFrame()
	{
		glfwPollEvents();
	}

	void EndFrame()
	{
		glfwSwapBuffers(g_window);
	}

	void UpdateSubSystem()
	{
		Input::Update();
	}

	void HandleSubSystem()
	{
		Input::HandleWindow();
	}


	//=========================================================================================================



	/*
		`7MMF'     A     `7MF'`7MMF'`7MN.   `7MF'`7MM"""Yb.     .g8""8q.`7MMF'     A     `7MF'
		  `MA     ,MA     ,V    MM    MMN.    M    MM    `Yb. .dP'    `YM.`MA     ,MA     ,V
		   VM:   ,VVM:   ,V     MM    M YMb   M    MM     `Mb dM'      `MM VM:   ,VVM:   ,V
			MM.  M' MM.  M'     MM    M  `MN. M    MM      MM MM        MM  MM.  M' MM.  M'
			`MM A'  `MM A'      MM    M   `MM.M    MM     ,MP MM.      ,MP  `MM A'  `MM A'
			 :MM;    :MM;       MM    M     YMM    MM    ,dP' `Mb.    ,dP'   :MM;    :MM;
			  VF      VF      .JMML..JML.    YM  .JMMmmmdP'     `"bmmd"'      VF      VF
	*/

	void CreateWindow(const WindowMode& mode)
	{
		if (mode == WindowMode::FULLSCREEN)
		{
			g_currentWindowWidth = g_fullScreenWidth;
			g_currentWindowHeight = g_fullScreenHeight;
			g_window = glfwCreateWindow(g_currentWindowWidth, g_currentWindowHeight,
										"", nullptr, nullptr);
		}
		else if (mode == WindowMode::WINDOW)
		{
			g_currentWindowWidth = g_windowWidth;
			g_currentWindowHeight = g_windowHeight;
			g_window = glfwCreateWindow(g_currentWindowWidth, g_currentWindowHeight,
				"", nullptr, nullptr);
			glfwSetWindowPos(g_window, 0, 0);
		}
	}
	GLFWwindow* GetWindowPointer()
	{
		return g_window;
	}

	bool WindowIsOpen()
	{
		return !(glfwWindowShouldClose(g_window) || g_forceWindowClose);
	}
	void ForceWindowClose()
	{
		g_forceWindowClose = true;
	}
}


//=========================================================================================================


