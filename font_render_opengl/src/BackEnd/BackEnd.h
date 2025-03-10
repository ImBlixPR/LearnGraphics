#pragma once

#include "Enums/MiskEnums.h"


struct GLFWwindow;
namespace BackEnd
{
	
	//core
	void Init();
	void StartFrame();
	void EndFrame();
	void UpdateSubSystem();
	void HandleSubSystem();



	//window
	void CreateWindow(const WindowMode& mode);
	GLFWwindow* GetWindowPointer();
	bool WindowIsOpen();
	void ForceWindowClose();





}