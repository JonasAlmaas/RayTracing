#include "RayTracing/MainLayer.h"

#include <Walnut/Timer.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>


namespace RayTracing {
	
	MainLayer::MainLayer()
		: m_Camera(45.0f, 0.1f, 100.0f)
	{
		{
			Sphere sphere;
			sphere.Albedo = { 1.0f, 0.0f, 1.0f };
			sphere.Position = { -1.0f, 0.0f, 0.0f };
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Albedo = { 0.2f, 0.3f, 0.9f };
			sphere.Position = { 0.0f, 0.0f, -5.0f };
			sphere.Radius = 1.5f;
			m_Scene.Spheres.push_back(sphere);
		}
	}

	void MainLayer::OnUpdate(float ts)
	{
		m_Camera.OnUpdate(ts);
	}

	void MainLayer::OnUIRender()
	{
		RenderPanelSettings();
		RenderPanelViewport();

		RenderImage();
	}

	void MainLayer::RenderPanelViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));


		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), {(float)image->GetWidth(), (float)image->GetHeight()}, { 0, 1 }, { 1, 0 });

		ImGui::End();

		ImGui::Begin("Scene");

		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);

			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.01f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.01f);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(sphere.Albedo), 0.01f);

			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::End();


		ImGui::PopStyleVar();
	}

	void MainLayer::RenderPanelSettings()
	{
		ImGui::Begin("Settings");

		ImGui::Text("Last Render: %.3fms", m_LastRenderTime);

		if (ImGui::Button("Render"))
		{
			RenderImage();
		}
		
		ImGui::Separator();

		ImGui::DragInt("Bounces", &m_Renderer.m_Bounces, 1.0f, 1, 10);

		ImGui::End();
	}

	void MainLayer::RenderImage()
	{
		Walnut::Timer renderTimer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = renderTimer.ElapsedMillis();
	}

}
