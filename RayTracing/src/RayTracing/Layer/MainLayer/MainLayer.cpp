#include "RayTracing/Layer/MainLayer/MainLayer.h"

#include <Walnut/Timer.h>
#include <imgui.h>


namespace RayTracing {

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
		ImGui::End();
	}

	void MainLayer::RenderImage()
	{
		Walnut::Timer renderTimer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render();

		m_LastRenderTime = renderTimer.ElapsedMillis();
	}

}
