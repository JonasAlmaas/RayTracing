#pragma once

#include "RayTracing/Camera.h"
#include "RayTracing/Ray.h"
#include "RayTracing/Objects.h"

#include <Walnut/Image.h>

#include <memory>
#include <glm/glm.hpp>


namespace RayTracing {

	class Renderer
	{
	private:
		struct HitPayload
		{
			float HitDistace;
			glm::vec3 WorldPosition;
			glm::vec3 WorldNormal;

			uint32_t OjectIndex;
		};

		struct Settings
		{
			bool Accumulate = true;
		};

	public:
		Renderer() = default;

		void OnResize(uint32_t width, uint32_t height);
		void Render(const Scene& scene, const Camera& camera);

		std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

		void ResetAccumulationFrame() { m_AccumulationFrame = 1; }

		Settings& GetSettings() { return m_Settings; }

	private:
		glm::vec4 RayGen(uint32_t x, uint32_t y);

		HitPayload TraceRay(const Ray& ray);
		HitPayload ClosestHit(const Ray& ray, float hitDistance, uint32_t objectIndex);
		HitPayload Miss(const Ray& ray);

	public:
		int m_Bounces = 4;

	private:
		std::shared_ptr<Walnut::Image> m_FinalImage;
		Settings m_Settings;

		const Scene* m_ActiveScene = nullptr;
		const Camera* m_ActiveCamera = nullptr;

		uint32_t* m_ImageData = nullptr;
		glm::vec4* m_AccumulationData = nullptr;
		
		uint32_t m_AccumulationFrame = 1;

	};

}
