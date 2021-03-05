// A single player pong game

#include "Game.h"
#include "MultiPlayer.h"
#include "BallFollowingAI.h"
#include <iostream>

int main(int argc, char** argv) {
	std::cout << "Enter your choice: \n1. For Single Player \n2. For Multi Player \n3. For ball following AI \n";
	int ch;
	std::cin >> ch;
	bool success;
	Game game;
	MultiPlayer multi;
	BallFollowingAI bfa;
	switch (ch) {
	case 1:
		success = game.Initialize();
		if (success) {
			game.RunLoop();
		}
		game.Shutdown();
		break;
	case 2:
		success = multi.Initialize();
		if (success) {
			multi.RunLoop();
		}
		multi.Shutdown();
		break;
	case 3:
		success = bfa.Initialize();
		if (success) {
			bfa.RunLoop();
		}
		bfa.Shutdown();
		break;
	}
	return 0;
}