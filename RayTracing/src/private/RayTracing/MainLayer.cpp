#include "RayTracing/MainLayer.h"

#include <Walnut/Timer.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>


namespace RayTracing {
	
	MainLayer::MainLayer()
		: m_Camera(45.0f, 0.1f, 100.0f)
	{
		// Pink
		{
			Material& mat = m_Scene.Materials.emplace_back();
			mat.Albedo = { 1.0f, 0.0f, 1.0f };
			mat.Roughness = 0.1f;
		}

		// Blue
		{
			Material& mat = m_Scene.Materials.emplace_back();
			mat.Albedo = { 0.2f, 0.3f, 0.9f };
			mat.Roughness = 0.1f;
		}

		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 0;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 0.0f, -101.0f, 0.0f };
			sphere.Radius = 100.0f;
			sphere.MaterialIndex = 1;
			m_Scene.Spheres.push_back(sphere);
		}
	}

	void MainLayer::OnUpdate(float ts)
	{
		if (m_Camera.OnUpdate(ts))
			m_Renderer.ResetAccumulationFrame();
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

		bool resetAccumulationFrame = false;

		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);

			Sphere& sphere = m_Scene.Spheres[i];
			resetAccumulationFrame |= ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.01f);
			resetAccumulationFrame |= ImGui::DragFloat("Radius", &sphere.Radius, 0.01f);
			resetAccumulationFrame |= ImGui::DragInt("Material", &sphere.MaterialIndex, 1, 0, m_Scene.Materials.size() - 1);
			
			ImGui::Separator();
			ImGui::PopID();
		}

		for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		{
			ImGui::PushID(i);
			
			Material& mat = m_Scene.Materials[i];
			
			resetAccumulationFrame |= ImGui::ColorEdit3("Albedo", glm::value_ptr(mat.Albedo), 0.01f);
			resetAccumulationFrame |= ImGui::DragFloat("Roughness", &mat.Roughness, 0.05f, 0.0f, 1.0f);
			resetAccumulationFrame |= ImGui::DragFloat("Metallic", &mat.Metallic, 0.05f, 0.0f, 1.0f);
			
			ImGui::Separator();
			ImGui::PopID();
		}

		if (resetAccumulationFrame)
			m_Renderer.ResetAccumulationFrame();

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
		
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

		if (ImGui::Button("Reset Accumilation"))
		{
			m_Renderer.ResetAccumulationFrame();
		}

		ImGui::Separator();

		ImGui::DragInt("Bounces", &m_Renderer.m_Bounces, 1.0f, 1, 100);

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
