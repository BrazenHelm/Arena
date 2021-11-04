#pragma once

#include <SFML/Graphics.hpp>

struct Ray
{
	sf::Vector2f origin;

	// dir should be normalised
	sf::Vector2f dir;
};

struct LineSegment
{
	sf::Vector2f start;
	sf::Vector2f end;
};


struct Circle
{
	sf::Vector2f centre;
	float radius;
};

// Check for collision between the given ray and line segment
// Returns true if there was a collision and false if there wasn't
// If there was a collision, t becomes the distance along the ray
// at which the collision occurred
// If there was not a collision, t may still be changed
bool CheckCollision(Ray ray, LineSegment line, float& t);


// Check for collision between the given ray and circle
// Returns true if there was a collision and false if there wasn't
// If there was a collision, t becomes the distance along the ray
// at which the collision occurred
// If there was not a collision, t may still be changed
// If there are two collisions the closer one is returned
bool CheckCollision(Ray ray, Circle circle, float& t);

