#include "Ray.h"

HitPayload Ray::Hit(const Sphere& sphere) const
{
	HitPayload hitPayload;

	// calculating if the camera ray intersects with the sphere
	float a = glm::dot(Direction, Direction);
	float b = 2.0f * (glm::dot(Origin, Direction) - glm::dot(Direction, sphere.Origin));
	float c = glm::dot(Origin, Origin) + glm::dot(sphere.Origin, sphere.Origin) - 2.0f * glm::dot(Origin, sphere.Origin) - sphere.Radius * sphere.Radius;

	float discriminant = b * b - 4.0f * a * c;

	if (discriminant > 0.0f)
	{
		// calculate the two hit points
		float t0 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		float t1 = (-b + glm::sqrt(discriminant)) / (2.0f * a);

		if (t0 < 0 && t1 < 0)
		{
			hitPayload.IsHit = false;
		}
		else if (t0 < 0 && t1 > 0)
		{
			hitPayload.IsHit = true;
			hitPayload.IsFrontFace = false;
			hitPayload.HitDistance = t1;
		}
		else
		{
			hitPayload.IsHit = true;
			hitPayload.IsFrontFace = true;
			hitPayload.HitDistance = t0;
		}
	}

	return hitPayload;
}