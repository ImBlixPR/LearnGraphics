#pragma once



namespace Game
{
	void Menu();
	bool Init();
	void PlayAgain();
	void Run(float dt);
	void Clear();
	bool IsRunning();

	// New transition-related declarations
	void UpdateTransition(float dt);
}