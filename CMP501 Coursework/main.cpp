#include <iostream>
#include <string>

#include "Game.h"
#include "NetworkManager.h"
#include "SpaceConversion.h"


int main()
{
	NetworkManager network;

	std::cout << "Would you like to Host or Join? (H/J) : ";
	char reply;
	std::cin >> reply;

	switch (reply)
	{
		case 'h': case 'H':
			network.Host();
			break;

		case 'j': case 'J':
			network.Join();
			break;

		default:
			std::cout << "Invalid Response" << std::endl;
			break;
	}

	// TODO: don't start if failed
	Game game(
		SpaceConversion::SCREEN_WIDTH,
		SpaceConversion::SCREEN_HEIGHT,
		"CMP501 Coursework",
		&network
	);

	game.SetUp();
	game.Run();

	system("pause");
	return 0;
}