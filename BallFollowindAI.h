#pragma once
#include "SDL.h"

struct FVector2 {
	float x;
	float y;
};

class BallFollowingAI {
public:
	BallFollowingAI();
	bool Initialize();
	void RunLoop();
	void Shutdown();
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	bool mIsRunning;
	Uint8 mTicksCount;

	int hPaddleDir;
	int aPadddleDir;
	FVector2 hPaddlePos;
	FVector2 aPaddlePos;
	FVector2 ballPos;
	FVector2 ballVel;
};