#pragma once

#include <Walnut/Layer.h>

#include "RayTracing/Camera.h"
#include "RayTracing/Renderer.h"


namespace RayTracing {

	class MainLayer : public Walnut::Layer
	{
	public:
		MainLayer();

		virtual void OnUpdate(float ts) override;
		virtual void OnUIRender() override;

	private:
		void RenderPanelSettings();
		void RenderPanelViewport();
		void RenderImage();

	private:
		Renderer m_Renderer;
		Scene m_Scene;
		Camera m_Camera;

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		float m_LastRenderTime = 0.0f;
	};

}
