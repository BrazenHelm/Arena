#include "SpaceConversion.h"

#include <cmath>


using namespace sf;


int SpaceConversion::SCREEN_WIDTH = 800;
int SpaceConversion::SCREEN_HEIGHT = 600;

float SpaceConversion::WORLD_WIDTH = 29.f;
float SpaceConversion::WORLD_HEIGHT = 14.f;

float SpaceConversion::PixelsPerWorldUnit = 50.f;

Vector2f SpaceConversion::CameraWorldPosition = Vector2f(0.f, 0.f);
float SpaceConversion::minX = SCREEN_WIDTH / PixelsPerWorldUnit / 2.f;
float SpaceConversion::minY = SCREEN_HEIGHT / PixelsPerWorldUnit / 2.f;
float SpaceConversion::maxX = WORLD_WIDTH - minX;
float SpaceConversion::maxY = WORLD_HEIGHT - minY;


void SpaceConversion::SetCameraPosition(sf::Vector2f newPos)
{
	float x = newPos.x;
	x = (x > minX) ? x : minX;
	x = (x < maxX) ? x : maxX;

	float y = newPos.y;
	y = (y > minY) ? y : minY;
	y = (y < maxY) ? y : maxY;

	CameraWorldPosition = Vector2f(x, y);
}


Vector2f ScreenToWorldPosition(Vector2f screenPos)
{
	// Number of pixels from centre of screen
	float x = screenPos.x - SpaceConversion::SCREEN_WIDTH / 2.f;
	float y = SpaceConversion::SCREEN_HEIGHT / 2.f - screenPos.y;	// invert y direction
	
	// Number of world units from camera position
	x /= SpaceConversion::PixelsPerWorldUnit;
	y /= SpaceConversion::PixelsPerWorldUnit;

	return SpaceConversion::CameraWorldPosition + Vector2f(x, y);
}


Vector2f WorldToScreenPosition(Vector2f worldPos)
{
	// Number of world units from camera position
	float x = worldPos.x - SpaceConversion::CameraWorldPosition.x;
	float y = SpaceConversion::CameraWorldPosition.y - worldPos.y;	// invert y direction

	// Number of pixels from centre of screen
	x *= SpaceConversion::PixelsPerWorldUnit;
	y *= SpaceConversion::PixelsPerWorldUnit;

	// Add centre of screen co-ordinates
	x += SpaceConversion::SCREEN_WIDTH / 2.f;
	y += SpaceConversion::SCREEN_HEIGHT / 2.f;
	
	return Vector2f(x, y);
}


float ScreenToWorldLength(float screenLength)
{
	return screenLength / SpaceConversion::PixelsPerWorldUnit;
}


float WorldToScreenLength(float worldLength)
{
	return worldLength * SpaceConversion::PixelsPerWorldUnit;
}

