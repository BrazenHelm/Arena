#pragma once

#include <SFML/Graphics.hpp>


struct SpaceConversion
{
	static int SCREEN_WIDTH, SCREEN_HEIGHT;
	static float WORLD_WIDTH, WORLD_HEIGHT;

	static float PixelsPerWorldUnit;

	static sf::Vector2f CameraWorldPosition;
	static float minX, minY, maxX, maxY;	// bounds for CameraWorldPosition

	static void SetCameraPosition(sf::Vector2f newPos);
};

sf::Vector2f ScreenToWorldPosition(sf::Vector2f screenPos);
sf::Vector2f WorldToScreenPosition(sf::Vector2f worldPos);

float ScreenToWorldLength(float screenLength);
float WorldToScreenLength(float worldLength);
