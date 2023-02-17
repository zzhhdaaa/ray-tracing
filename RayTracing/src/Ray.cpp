#include "Ray.h"

namespace RayMath
{
	bool Hit(const Ray& ray, const BoundingBox& box)
	{
		float t_min = 0.0f;
		float t_max = FLT_MAX;

		for (int i = 0; i < 3; i++)
		{
			float inD = 1.0f / ray.Direction[i];
			float t0;
			float t1;
			if (inD >= 0.0f)
			{
				t0 = (box.MinPos[i] - ray.Origin[i]) * inD;
				t1 = (box.MaxPos[i] - ray.Origin[i]) * inD;
			}
			else
			{
				t0 = (box.MaxPos[i] - ray.Origin[i]) * inD;
				t1 = (box.MinPos[i] - ray.Origin[i]) * inD;
			}

			t_min = t0 > t_min ? t0 : t_min;
			t_max = t1 < t_max ? t1 : t_max;
			if (t_max <= t_min)
				return false;
		}
		return true;
	}

	HitPayload Hit(const Ray& ray, const Sphere& sphere)
	{
		HitPayload hitPayload;

		// calculating if the camera ray intersects with the sphere
		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * (glm::dot(ray.Origin, ray.Direction) - glm::dot(ray.Direction, sphere.Origin));
		float c = glm::dot(ray.Origin, ray.Origin) + glm::dot(sphere.Origin, sphere.Origin) - 2.0f * glm::dot(ray.Origin, sphere.Origin) - sphere.Radius * sphere.Radius;

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

	HitPayload PayloadCompute(HitPayload& payload, const Ray& ray, const Sphere& sphere, const float& opacity)
	{
		payload.HitPosition = ray.Origin + ray.Direction * payload.HitDistance;
		payload.HitNormal = glm::normalize(payload.HitPosition - sphere.Origin);

		if (std::abs(Walnut::Random::Float()) > opacity)
		{
			payload.HitRefracted = true;
		}
		else
		{
			payload.HitRefracted = false;
		}

		// set the hit point back to a sphere of radius one, centered at the origin
		glm::vec3 point = (payload.HitPosition - sphere.Origin) * (1.0f / sphere.Radius);

		float theta = acos(-point.y);
		float phi = atan2(-point.z, point.x) + PI;

		payload.HitUV[0] = phi / (2 * PI);
		payload.HitUV[1] = theta / PI;

		return payload;
	}
}