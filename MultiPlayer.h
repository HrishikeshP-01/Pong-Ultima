#pragma once
#include "SDL.h";

struct Vector2Multi {
	float x;
	float y;
};

class MultiPlayer {
public:
	MultiPlayer();
	bool Initialize();
	void RunLoop();
	void Shutdown();
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	Uint32 mTicksCount;
	bool mIsRunning;

	int aPaddleDir;
	Vector2Multi aPaddlePos;
	int bPaddleDir;
	Vector2Multi bPaddlePos;
	Vector2Multi ballPos;
	Vector2Multi ballVel;
};
