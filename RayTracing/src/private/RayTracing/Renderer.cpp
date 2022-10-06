#include "RayTracing/Renderer.h"

#include <Walnut/Random.h>


namespace RayTracing {

	namespace Utils {

		static uint32_t ConvertToRGBA8(const glm::vec4& color)
		{
			uint8_t r = (uint8_t)(color.r * 255);
			uint8_t g = (uint8_t)(color.g * 255);
			uint8_t b = (uint8_t)(color.b * 255);
			uint8_t a = (uint8_t)(color.a * 255);

			return r | (g << 8) | (b << 16) | (a << 24);
		}

	}

	void Renderer::OnResize(uint32_t width, uint32_t height)
	{
		if (m_FinalImage)
		{
			if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
				return;

			m_FinalImage->Resize(width, height);
		}
		else
		{
			m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
		}

		delete[] m_ImageData;
		m_ImageData = new uint32_t[width * height];
	}

	void Renderer::Render(const Scene& scene, const Camera& camera)
	{
		m_ActiveScene = &scene;
		m_ActiveCamera = &camera;

		// Y firt to save on CPU cache
		for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
		{
			for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
			{		
				glm::vec4 colorFloat = RayGen(x, y);

				colorFloat = glm::clamp(colorFloat, glm::vec4(0.0f), glm::vec4(1.0f));
				m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA8(colorFloat);
			}
		}

		m_FinalImage->SetData(m_ImageData);
	}

	glm::vec4 Renderer::RayGen(uint32_t x, uint32_t y)
	{
		Ray ray;
		ray.Origin = m_ActiveCamera->GetPosition();
		ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

		glm::vec3 color(0.0f);

		float multiplier = 1.0f;

		uint32_t bounces = 2;
		for (uint32_t i = 0; i < bounces; i++)
		{
			HitPayload payload = TraceRay(ray);

			if (payload.HitDistace < 0.0f)
			{
				glm::vec3 skyColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				color += skyColor * multiplier;
				break;
			}

			glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
			float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f);

			const Sphere& sphere = m_ActiveScene->Spheres[payload.OjectIndex];
		 
			glm::vec3 sphereColor = sphere.Albedo;
			sphereColor *= lightIntensity;
			//sphereColor = payload.WorldNormal * 0.5f + 0.5f;

			color += sphereColor * multiplier;
			multiplier *= 0.7f;

			ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
			ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal);
		}

		return glm::vec4(color, 1.0f);
	}

	Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
	{
		float hitDistance = FLT_MAX;
		int objectIndex = -1;

		for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
		{
			const Sphere& sphere = m_ActiveScene->Spheres[i];
			glm::vec3 origin = ray.Origin - sphere.Position;

			float a = glm::dot(ray.Direction, ray.Direction);
			float b = 2 * glm::dot(origin, ray.Direction);
			float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

			float discriminant = b * b - 4.0f * a * c;
			if (discriminant < 0.0f)
				continue;

			float closestT = (-b - glm::sqrt(discriminant)) / 2.0f * a;
			//float t0 = (-b + glm::sqrt(discriminant)) / 2.0f * a;

			if (closestT > 0.0f && closestT < hitDistance)
			{
				objectIndex = (int)i;
				hitDistance = closestT;
			}
		}

		if (objectIndex < 0)
			return Miss(ray);

		return ClosestHit(ray, hitDistance, (uint32_t)objectIndex);
	}

	Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, uint32_t objectIndex)
	{
		HitPayload payload;
		payload.HitDistace = hitDistance;
		payload.OjectIndex = objectIndex;

		const Sphere& sphere = m_ActiveScene->Spheres[objectIndex];

		glm::vec3 origin = ray.Origin - sphere.Position;

		payload.WorldPosition = origin + ray.Direction * hitDistance;
		payload.WorldNormal = glm::normalize(payload.WorldPosition);

		payload.WorldPosition += sphere.Position;

		return payload;
	}

	Renderer::HitPayload Renderer::Miss(const Ray& ray)
	{
		HitPayload payload;
		payload.HitDistace = -1;

		return payload;
	}

}
