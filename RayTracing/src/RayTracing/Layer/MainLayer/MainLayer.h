#pragma once

#include <Walnut/Layer.h>

#include "RayTracing/Renderer/Renderer.h"


namespace RayTracing {

	class MainLayer : public Walnut::Layer
	{
	public:
		virtual void OnUIRender() override;

	private:
		void RenderPanelSettings();
		void RenderPanelViewport();
		void RenderImage();

	private:
		Renderer m_Renderer;

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		float m_LastRenderTime = 0.0f;
	};

}
