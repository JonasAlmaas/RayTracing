#include "RayTracing/Renderer/Renderer.h"

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

	void Renderer::Render()
	{
		// Y firt to save on CPU cache
		for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
		{
			for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
			{
				glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
				coord = coord * 2.0f - 1.0f; // Remap from 0 -> 1 to -1 to 1;

				glm::vec4 colorFloat = PerPixel(coord);
				colorFloat = glm::clamp(colorFloat, glm::vec4(0.0f), glm::vec4(1.0f));
				m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA8(colorFloat);
			}
		}

		m_FinalImage->SetData(m_ImageData);
	}

	glm::vec4 Renderer::PerPixel(glm::vec2 coord)
	{
		uint8_t r = (uint8_t)(coord.x * 255.0f);
		uint8_t g = (uint8_t)(coord.y * 255.0f);

		glm::vec3 rayOrigin{ 0.0f, 0.0f, 2.0f };
		glm::vec3 rayDirection{ coord.x, coord.y, -1.0f };
		//rayDirection = glm::normalize(rayDirection);
		float radius = 0.5f;

		// (b.x^2 + b.y^2) * t^2 + (2(a.x * b.x + a.y * b.y)) * t + (a.x^2 + a.y^2 - r^2) = 0

		// a = ray origin
		// b = ray direction
		// r = radius on sphere
		// t = hit distance

		float a = glm::dot(rayDirection, rayDirection);
		float b = 2 * glm::dot(rayOrigin, rayDirection);
		float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

		// Quadratic formula discriminant:
		// b^2 - 4ac

		// (-b +- sqrt(discriminant)) / 2a

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0)
			return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		//float t0 = (-b + glm::sqrt(discriminant)) / 2.0f * a;
		float closestT = (-b - glm::sqrt(discriminant)) / 2.0f * a;

		glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;
		glm::vec3 normal = glm::normalize(hitPoint);

		glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

		float d = glm::max(glm::dot(normal, -lightDir), 0.0f);

		glm::vec3 sphereColor(0.8f, 0.3f, 0.2f);
		sphereColor *= d;
		//sphereColor = normal * 0.5f + 0.5f;

		return glm::vec4(sphereColor, 1.0f);
	}

}
