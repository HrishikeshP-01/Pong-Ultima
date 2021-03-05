// A single player pong game

#include "Game.h"
#include "MultiPlayer.h"
#include <iostream>

int main(int argc, char** argv) {
	std::cout << "Enter your choice: \n1. For Single Player \n2. For Multi Player \n";
	int ch;
	std::cin >> ch;
	bool success;
	Game game;
	MultiPlayer multi;
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
	}
	return 0;
}