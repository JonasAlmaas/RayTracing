#include <Walnut/Application.h>
#include <Walnut/EntryPoint.h>

#include "RayTracing/MainLayer.h"


Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Traceing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<RayTracing::MainLayer>();
	app->SetMenubarCallback([app]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}
