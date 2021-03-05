#include "Game.h";

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game() {
	mWindow = nullptr;
	mRenderer = nullptr;
	mTicksCount = 0;
	mIsRunning = false;
	mPaddleDir = 0;
}

bool Game::Initialize() {
	// Initialze SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0) {
		SDL_Log("Unable to initialize SDL %s", SDL_GetError());
		return false;
	}

	// Create an SDL window
	mWindow = SDL_CreateWindow(
		"Pong - Single Player", // title
		100, // top-left x-coordinate of window
		100, // top-left y-coordinate of window
		1024, // width of window
		768, // height of window
		0 // Flags ( 0 for no flags )
	);
	if (!mWindow) {
		SDL_Log("Failed to create SDL window %s", SDL_GetError());
		return false;
	}

	// Create an SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create rendere for
		-1, // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!mRenderer) {
		SDL_Log("Failed to create SDL renderer %s", SDL_GetError());
		return false;
	}

	// Pong specific
	mPaddlePos.x = 10.0f; // Near to left side
	mPaddlePos.y = 768.0f / 0.2f; // Middle of height of screen
	mBallPos.x = 1024.0f / 2.0f; // Middle of width of screen
	mBallPos.y = 768.0f / 2.0f; // Middle of height of screen
	mBallVel.x = -200.0f; // Intial movement towards player
	mBallVel.y = 235.0f;

	mIsRunning = true;
	return true;
}

void Game::RunLoop() {
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			// if we get an SDL_Quit event, end loop
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed then too end the loop
	if (state[SDL_SCANCODE_ESCAPE]) {
		mIsRunning = false;
	}

	// Update paddle direction based on W/S keys
	mPaddleDir = 0; // Reset it else once pressed even if you take your hands off movement continues
	if (state[SDL_SCANCODE_W]) {
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S]) {
		mPaddleDir += 1;
	}
}

void Game::UpdateGame() {
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));
	// Delta time is the difference in ticks from last frame
	// converting to seconds
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	// Clamp maximum delta time value
	if (deltaTime > 0.05f) {
		deltaTime = 0.05f;
	}
	// Update ticks count for next frame
	mTicksCount = SDL_GetTicks();

	// Update paddle position based on direction
	if (mPaddleDir != 0) {
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		// Make sure the paddle doesn't move off screen
		if (mPaddlePos.y < (paddleH / 2.0f + thickness)) {
			mPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleH / 2.0f - thickness)) {
			mPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	// Update ball position based on ball velocity
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;

	// Bounce if needed
	// Did we intersect with the paddle
	float diff = mPaddlePos.y - mBallPos.y;
	// Take absolute value of difference
	diff = (diff > 0.0f) ? diff : -diff;
	if (
		// Our y difference is small enough so as to fit the paddle size
		diff <= paddleH / 2.0f &&
		// We are in the correct x-position
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// The ball is moving to the left
		// This is important else even if the ball is moving away from paddle the code is executed resulting in the ball being stuck at one place
		mBallVel.x < 0.0f
		) {
		mBallVel.x *= -1.0f;
	}
	// Did the ball go off-screen if so end game
	else if (mBallPos.x <= 0.0f) {
		SDL_Log("You Lost! Keep Trying");
		mBallPos.x = 1024.0f / 2.0f;
		mBallPos.y = 768.0f / 2.0f;
	}
	// Did the ball collide with the right wall
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f) {
		mBallVel.x *= -1.0f;
	}
	// Did the ball collide with the top wall
	else if (mBallPos.y <= thickness && mBallVel.y < 0.0f) {
		mBallVel.y *= -1;
	}
	// Did the ball collide with the bottom wall
	else if (mBallPos.y >= (768 - thickness) && mBallVel.y > 0.0f) {
		mBallVel.y *= -1;
	}
}

void Game::GenerateOutput() {
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0, //R
		0, //G
		255, //B
		255 //A
		);
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(
		mRenderer,
		255,
		255,
		255,
		255
	);
	// Draw top wall
	SDL_Rect wall{
		0, // top left x
		0, // top left y
		1024, // width 
		thickness // height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	// Draw right wall
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);
	// Draw paddle
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);
	// Draw ball
	SDL_Rect ball{
		static_cast<int>(mBallPos.x - thickness / 2),
		static_cast<int>(mBallPos.y - thickness / 2),
		thickness,
		thickness,
	};
	SDL_RenderFillRect(mRenderer, &ball);

	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown() {
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}