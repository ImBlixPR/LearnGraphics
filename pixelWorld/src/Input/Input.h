#pragma once

#include <glm/glm.hpp>


#include "KeyCode.h"


namespace Input {

	void Init();
	void Update();
	bool KeyPressed(unsigned int keycode);
	bool KeyDown(unsigned int keycode);
	//float GetMouseOffsetX();
	//float GetMouseOffsetY();
	bool MouseButtonPressed(int button);
	bool MouseButtonDown(int button);
	bool MouseButtonReleased(int button);
	//bool MouseWheelUp();
	//bool MouseWheelDown();
	//int GetMouseWheelValue();
	double GetMouseX();
	double GetMouseY();
	glm::vec2 GetMousePos();
	//void PreventRightMouseHold();
	//int GetScrollWheelYOffset();
	//void ResetScrollWheelYOffset();
	////int GetCursorX();
	////int GetCursorY();
	//void DisableCursor();
	//void HideCursor();
	//void ShowCursor();
	//int GetCursorScreenX();
	//int GetCursorScreenY();
	//int GetViewportMappedMouseX(int viewportWidth);
	//int GetViewportMappedMouseY(int viewportHeight);


	//window
	void HandleWindow();
}