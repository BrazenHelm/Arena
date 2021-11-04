#include "Collisions.h"


bool CheckCollision(Ray ray, LineSegment line, float& t)
{
	float
		x1 = ray.origin.x,
		y1 = ray.origin.y,
		x2 = ray.origin.x + ray.dir.x,
		y2 = ray.origin.y + ray.dir.y,
		x3 = line.start.x,
		y3 = line.start.y,
		x4 = line.end.x,
		y4 = line.end.y;

	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	if (abs(d) < std::numeric_limits<float>::epsilon()) return false;

	float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / d;
	if (u < 0.f || u > 1.f) return false;

	t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / d;
	return (t > 0.f);
}


bool CheckCollision(Ray ray, Circle circle, float& t)
{
	float
		x1 = ray.origin.x,
		y1 = ray.origin.y,
		x2 = ray.origin.x + ray.dir.x,
		y2 = ray.origin.y + ray.dir.y,
		cx = circle.centre.x,
		cy = circle.centre.y,
		r = circle.radius;

	// distance of the centre of the circle along the ray
	float dot = (cx - x1) * (x2 - x1) + (cy - y1) * (y2 - y1);

	// closest point on the ray to the circle
	float
		closestX = x1 + dot * ray.dir.x,
		closestY = y1 + dot * ray.dir.y;

	// distance between the closest point and the centre of the circle
	float
		distX = closestX - cx,
		distY = closestY - cy,
		d = sqrtf(distX * distX + distY * distY);

	// circle is too far from ray
	if (d > r) return false;

	// distance along the ray from the closest point to the intersection points
	float dt = sqrtf(r * r - d * d);

	if (dot + dt < 0.f)
	{
		// circle is behind ray
		return false;
	}
	else if (dot - dt >= 0.f)
	{
		// two points of intersection
		t = dot - dt;
		return true;
	}
	else
	{
		// start point of ray is inside circle; one point of intersection
		t = dot + dt;
		return true;
	}
}

