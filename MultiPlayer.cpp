#include "MultiPlayer.h"

const int thickness = 15;
const float paddleH = 100.0f;

MultiPlayer::MultiPlayer() {
	mWindow = nullptr;
	mRenderer = nullptr;
	mIsRunning = false;
	mTicksCount = 0;
	aPaddleDir = 0;
	bPaddleDir = 0;
}

bool MultiPlayer::Initialize() {
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0) {
		SDL_Log("Error initializing SDL %s", SDL_GetError());
		return false;
	}
	mWindow = SDL_CreateWindow(
		"Pong Multi-Player",
		100,
		100,
		1024,
		768,
		0
	);
	if (!mWindow) {
		SDL_Log("Error creating window %s", SDL_GetError());
		return false;
	}
	mRenderer = SDL_CreateRenderer(
		mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!mRenderer) {
		SDL_Log("Error creating renderer %s", SDL_GetError());
		return false;
	}

	aPaddlePos.x = 15.0f;
	aPaddlePos.y = 768.0f / 0.2f;
	bPaddlePos.x = 1024.0f - 15.0f -thickness;
	bPaddlePos.y = 768.0f / 2.0f;
	ballPos.x = 1024.0f / 2.0f;
	ballPos.y = 768.0f / 2.0f;
	ballVel.x = -200.0f;
	ballVel.y = 235.0f;

	mIsRunning = true;
	return true;
}

void MultiPlayer::RunLoop() {
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void MultiPlayer::ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE]) {
		mIsRunning = false;
	}
	aPaddleDir = 0;
	bPaddleDir = 0;
	if (state[SDL_SCANCODE_W]) {
		aPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S]) {
		aPaddleDir += 1;
	}
	if (state[SDL_SCANCODE_I]) {
		bPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_K]) {
		bPaddleDir += 1;
	}
}

void MultiPlayer::UpdateGame() {
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f) {
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	if (aPaddleDir != 0) {
		aPaddlePos.y += 300.0f * aPaddleDir * deltaTime;
		if (aPaddlePos.y < paddleH / 2.0f + thickness) {
			aPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (aPaddlePos.y > 768.0f - paddleH / 2.0f - thickness) {
			aPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}
	if (bPaddleDir != 0) {
		bPaddlePos.y += 300.0f * bPaddleDir * deltaTime;
		if (bPaddlePos.y < paddleH / 2.0f + thickness) {
			bPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (bPaddlePos.y > 768.0f - paddleH / 2.0f - thickness) {
			bPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	ballPos.x += ballVel.x * deltaTime;
	ballPos.y += ballVel.y * deltaTime;
	float diff_a = ballPos.y - aPaddlePos.y;
	float diff_b = ballPos.y - bPaddlePos.y;
	diff_a = (diff_a < 0.0f) ? -diff_a : diff_a;
	diff_b = (diff_b < 0.0f) ? -diff_b : diff_b;
	if (
		diff_a <= paddleH / 2.0f &&
		ballPos.x >= 20.0f && ballPos.x <= 25.0f &&
		ballVel.x < 0.0f) {
		ballVel.x *= -1.0f;
	}
	else if (
		diff_b <= paddleH / 2.0f &&
		ballPos.x >= 1024 - 20.0f && ballPos.x <= 1024 - 10.0f &&
		ballVel.x > 0.0f) {
		ballVel.x *= -1.0f;
	}
	else if (ballPos.x <= 0.0f) {
		SDL_Log("Player A Lost. Player B Won!");
		ballPos.x = 1024.0f / 2.0f;
		ballPos.y = 768.0f / 2.0f;
	}
	else if (ballPos.x > 1024.0f) {
		SDL_Log("Player B Lost. Player A Won!");
		ballPos.x = 1024.0f / 2.0f;
		ballPos.y = 768.0f / 2.0f;
	}
	else if (ballPos.y <= thickness && ballVel.y < 0.0f) {
		ballVel.y *= -1.0f;
	}
	else if (ballPos.y > 768.0f - thickness && ballVel.y > 0.0f) {
		ballVel.y *= -1;
	}
}

void MultiPlayer::GenerateOutput() {
	SDL_SetRenderDrawColor(
		mRenderer,
		0,
		0,
		255,
		255
	);
	SDL_RenderClear(mRenderer);
	SDL_SetRenderDrawColor(
		mRenderer,
		255,
		255,
		255,
		255
	);
	SDL_Rect Wall{
		0,
		0,
		1024,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &Wall);
	Wall.y = 768.0f - thickness;
	SDL_RenderFillRect(mRenderer, &Wall);
	SDL_Rect Paddle{
		static_cast<int>(aPaddlePos.x),
		static_cast<int>(aPaddlePos.y - paddleH/2.0f),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &Paddle);
	Paddle.x = static_cast<int>(bPaddlePos.x);
	Paddle.y = static_cast<int>(bPaddlePos.y - paddleH/2.0f);
	SDL_RenderFillRect(mRenderer, &Paddle);
	SDL_Rect Ball{
		static_cast<int>(ballPos.x - thickness / 2.0f),
		static_cast<int>(ballPos.y - thickness / 2.0f),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &Ball);
	SDL_RenderPresent(mRenderer);
}

void MultiPlayer::Shutdown() {
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}