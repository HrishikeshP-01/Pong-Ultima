#include "BallFollowingAI.h";
const int thickness = 15;
const float paddleH = 100.0f;

BallFollowingAI::BallFollowingAI() {
	mWindow = nullptr;
	mRenderer = nullptr;
	mIsRunning = false;
	mTicksCount = 0;
	hPaddleDir = 0;
	aPaddleDir = 0;
}

bool BallFollowingAI::Initialize() {
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0) {
		SDL_Log("Error initializing SDL %s", SDL_GetError());
		return false;
	}
	mWindow = SDL_CreateWindow(
		"Following AI",
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

	aPaddlePos.x = 15.0;
	aPaddlePos.y = 768.0f / 2.0f;
	hPaddlePos.x = 1024.0f - 15.0f - thickness;
	hPaddlePos.y = 768.0f / 2.0f;
	ballPos.x = 1024.0f / 2.0f;
	ballPos.y = 768.0f / 2.0f;
	ballVel.x = -200.0f;
	ballVel.y = 235.0f;

	mIsRunning = true;
	return true;
}

void BallFollowingAI::RunLoop() {
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void BallFollowingAI::ProcessInput() {
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
	hPaddleDir = 0;
	if (state[SDL_SCANCODE_W]) {
		hPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S]) {
		hPaddleDir += 1;
	}
}

void BallFollowingAI::UpdateGame() {
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f) {
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	if (hPaddleDir != 0) {
		hPaddlePos.y += 300.0f * deltaTime * hPaddleDir;
		if (hPaddlePos.y < paddleH / 2.0f + thickness) {
			hPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (hPaddlePos.y > 768.0f - paddleH / 2.0f - thickness) {
			hPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	FollowingAI(deltaTime);

	ballPos.x += ballVel.x * deltaTime;
	ballPos.y += ballVel.y * deltaTime;
	float diff_h = ballPos.y - hPaddlePos.y;
	float diff_a = ballPos.y - aPaddlePos.y;
	diff_h = (diff_h > 0.0f) ? diff_h : -diff_h;
	diff_a = (diff_a > 0.0f) ? diff_a : -diff_a;
	if (
		diff_h <= paddleH / 2.0f &&
		ballPos.x <= 1024.0f-20.0f && ballPos.x >= 1024.0f-25.0f &&
		ballVel.x > 0.0f
		) {
		ballVel.x *= -1;
	}
	else if (
		diff_a <= paddleH / 2.0f &&
		ballPos.x >= 20.0f && ballPos.x <= 25.0f &&
		ballVel.x < 0.0f
		) {
		ballVel.x *= -1;
	}
	else if (ballPos.y <= thickness && ballVel.y < 0.0f) {
		ballVel.y *= -1;
	}
	else if (ballPos.y >= 768.0f - thickness && ballVel.y > 0.0f) {
		ballVel.y *= -1;
	}
	else if(ballPos.x < 0.0f) {
		SDL_Log("AI Lost. Human Won!");
		ballPos.x = 1024.0f / 2.0f;
		ballPos.y = 768.0f / 2.0f;
	}
	else if (ballPos.x > 1024.0f) {
		SDL_Log("Human Lost. AI Won!");
		ballPos.x = 1024.0f / 2.0f;
		ballPos.y = 768.0f / 2.0f;
	}
}

void BallFollowingAI::FollowingAI(float& deltaTime) {
	aPaddleDir = 0;
	if (aPaddlePos.y > ballPos.y) {
		aPaddleDir -= 1;
	}
	else if (aPaddlePos.y + paddleH < ballPos.y) {
		aPaddleDir += 1;
	}
	if (aPaddleDir != 0) {
		aPaddlePos.y += 300.0f* deltaTime * aPaddleDir;
		// Always make sure that the detaTime is capable of making the position break out of these conditions else the change will be so small it gets reset to the same value i.e. paddle gets stuck in a place
		if (aPaddlePos.y < paddleH / 2.0f + thickness) {
			aPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (aPaddlePos.y > 768.0f - paddleH / 2.0f - thickness) {
			aPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}
}

void BallFollowingAI::GenerateOutput() {
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
		static_cast<int>(hPaddlePos.x),
		static_cast<int>(hPaddlePos.y - paddleH/2.0f),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &Paddle);
	Paddle.x = static_cast<int>(aPaddlePos.x);
	Paddle.y = static_cast<int>(aPaddlePos.y - paddleH/2.0f);
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

void BallFollowingAI::Shutdown() {
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}