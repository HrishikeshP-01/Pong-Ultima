#pragma once
#include "SDL.h";

struct PVector2 {
	float x;
	float y;
};

class PositionPredictingAI {
public:
	PositionPredictingAI();
	bool Initialize();
	void RunLoop();
	void Shutdown();
private:
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	Uint8 mTicksCount;
	bool mIsRunning;
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	int hPaddleDir;
	int aPaddleDir;
	PVector2 hPaddlePos;
	PVector2 aPaddlePos;
	PVector2 ballPos;
	PVector2 ballVel;
};