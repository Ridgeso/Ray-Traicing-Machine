#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GLFW/glfw3.h>

#include "Application.h"
#include "Time.h"
#include "Renderer.h"
#include <iostream>

namespace RT
{

	uint32_t pcg_hash(uint32_t input)
	{
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	float FastRandom(uint32_t& seed)
	{
		seed = pcg_hash(seed);
		return (float)seed / std::numeric_limits<uint32_t>::max();
	}

	Application* Application::MainApp = nullptr;

	Application::Application(ApplicationSpecs specs)
		: specs(specs),
		shouldRun(true),
		lastFrameDuration(0.f),
		appFrameDuration(0),
		viewportSize(),
		mainWindow(),
		camera(45.0f, 0.01f, 100.0f)
	{
		MainApp = this;
		uint32_t seed = 93262352u;

		WindowSpecs winSpecs = { specs.name, 1280, 720, false };
		mainWindow.Init(winSpecs);

		glm::ivec2 windowSize = mainWindow.GetSize();
		Render::RenderSpecs renderSpecs = { windowSize.x, windowSize.y, false };
		renderer.Init(renderSpecs);

		scene.materials.emplace_back(Render::Material{ { 0.0f, 0.0f, 0.0f }, 0.0, { 0.0f, 0.0f, 0.0f }, 0.0f,  0.0f, 0.0f, 1.0f });
		scene.materials.emplace_back(Render::Material{ { 1.0f, 1.0f, 1.0f }, 0.0, { 1.0f, 1.0f, 1.0f }, 0.7f,  0.8f, 0.0f, 1.5f });
		scene.materials.emplace_back(Render::Material{ { 0.2f, 0.5f, 0.7f }, 0.0, { 0.2f, 0.5f, 0.7f }, 0.05f, 0.3f, 0.0f, 1.0f });
		scene.materials.emplace_back(Render::Material{ { 0.8f, 0.6f, 0.5f }, 0.0, { 0.8f, 0.6f, 0.5f }, 0.0f,  0.3f, 1.0f, 1.0f });
		scene.materials.emplace_back(Render::Material{ { 0.4f, 0.3f, 0.8f }, 0.0, { 0.8f, 0.6f, 0.5f }, 0.0f,  0.3f, 0.0f, 1.0f });
		
		scene.spheres.emplace_back(Render::Sphere{ { 0.0f, 0.0f, -2.0f }, 1.0f, 1 });
		scene.spheres.emplace_back(Render::Sphere{ { 0.0f, -2001.0f, -2.0f }, 2000.0f, 2 });
		scene.spheres.emplace_back(Render::Sphere{ { 2.5f, 0.0f, -2.0f }, 1.0f, 3 });
		scene.spheres.emplace_back(Render::Sphere{ { -2.5f, 0.0f, -2.0f }, 1.0f, 4 });

		auto getRandPos = [&seed](float rad) { return FastRandom(seed)* rad - rad / 2; };

		for (int i = 0; i < 70; i++)
		{
			scene.materials.emplace_back(Render::Material{ });
			scene.materials[scene.materials.size() - 1].albedo = { FastRandom(seed), FastRandom(seed), FastRandom(seed) };
			scene.materials[scene.materials.size() - 1].emissionColor = { FastRandom(seed), FastRandom(seed), FastRandom(seed) };
			scene.materials[scene.materials.size() - 1].roughness = FastRandom(seed) > 0.9 ? 0.f : FastRandom(seed);
			scene.materials[scene.materials.size() - 1].emissionPower = FastRandom(seed) > 0.9 ? FastRandom(seed) : 0.f;
			scene.materials[scene.materials.size() - 1].refractionRatio = 1.0f;
		
			scene.spheres.emplace_back(Render::Sphere{ });
			scene.spheres[scene.spheres.size() - 1].position = { getRandPos(10.0f), -0.75, getRandPos(10.0f) - 2 };
			scene.spheres[scene.spheres.size() - 1].radius = 0.25;
			scene.spheres[scene.spheres.size() - 1].materialId = scene.materials.size() - 1;
		}
	
		lastMousePos = windowSize / 2;
	}

	Application::~Application()
	{
		mainWindow.ShutDown();
		renderer.ShutDown();
	}

	void Application::Run()
	{
		while (shouldRun)
		{
			Timer appTimer;

			Render();

			mainWindow.BeginUI();
            Layout();
			mainWindow.EndUI();

			shouldRun &= mainWindow.Update();
			shouldRun &= mainWindow.PullEvents();

			appFrameDuration = appTimer.Ellapsed();
		}
	}

    void Application::Layout()
    {
		ImGui::Begin("Settings");
		ImGui::Text("App frame took: %.3fms", appFrameDuration);
		ImGui::Text("Last render took: %.3fms", lastFrameDuration);
		ImGui::Text("Frames: %d", renderer.GetFrames());
		ImGui::DragInt("Bounces Limit", (int32_t*)&renderer.MaxBounces(), 1, 1, 15);
		ImGui::DragInt("Precalculated Frames Limit", (int32_t*)&renderer.MaxFrames(), 1, 1, 15);
		if (ImGui::Button("Reset"))
			renderer.ResetFrame();
		ImGui::Checkbox("Accumulate", &renderer.Accumulation());
		ImGui::Checkbox("Draw Environment", &renderer.DrawEnvironment());
		if (ImGui::Button("Add Material"))
			scene.materials.emplace_back(Render::Material{ { 0.0f, 0.0f, 0.0f }, 0.0, { 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f, 0.0f });
		if (ImGui::Button("Add Sphere"))
			scene.spheres.emplace_back(Render::Sphere{ { 0.0f, 0.0f, -2.0f }, 1.0f, 0 });
		ImGui::End();

		ImGui::Begin("Scene");
		
		ImGui::Text("Materials:");
		for (size_t i = 1; i < scene.materials.size(); i++)
		{
			ImGui::PushID((int32_t)i);
			Render::Material& material = scene.materials[i];

			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.albedo));
			ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.emissionColor));
			ImGui::DragFloat("Roughness", &material.roughness, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Metalic", &material.metalic, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Emission Power", &material.emissionPower, 0.005f, 0.0f, std::numeric_limits<float>::max());
			ImGui::DragFloat("Refraction Index", &material.refractionRatio, 0.005f, 1.0f, 32.0f);

			ImGui::Separator();
			ImGui::PopID();
		}
		
		ImGui::Separator();

		ImGui::Text("Spheres:");
		for (size_t i = 0; i < scene.spheres.size(); i++)
		{
			ImGui::PushID((int32_t)i);
			Render::Sphere& sphere = scene.spheres[i];

			ImGui::DragFloat3("Position", glm::value_ptr(sphere.position), 0.1f);
			ImGui::DragFloat("Radius", &sphere.radius, 0.01f, 0.0f, std::numeric_limits<float>::max());
			ImGui::SliderInt("Material", &sphere.materialId, 1, scene.materials.size() - 1);

			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		ImVec2 viewPort = ImGui::GetContentRegionAvail();
		if (viewPort.x != viewportSize.x || viewPort.y != viewportSize.y)
		{
			viewportSize = viewPort;
			renderer.ResetFrame();
		}

		ImGui::Image(
			(ImTextureID)renderer.GetDescriptor(),
			viewportSize,
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::End();
		ImGui::PopStyleVar();

		//static bool demo = true;
		//ImGui::ShowDemoWindow(&demo);
	}

	void Application::Render()
	{
		UpdateView(appFrameDuration / 1000.0f);
		glm::ivec2 winSize = mainWindow.GetSize();
		renderer.RecreateRenderer(winSize.x, winSize.y);

		Timer timeit;
		camera.ResizeCamera((int32_t)viewportSize.x, (int32_t)viewportSize.y);
		renderer.Render(camera, scene);
		lastFrameDuration = timeit.Ellapsed();
	}

	void Application::UpdateView(float ts)
	{
		const float speed = 5.0f;
		const float mouseSenisity = 0.003f;
		const float rotationSpeed = 0.3f;
		const glm::vec3 up = glm::vec3(0, 1, 0);
		const glm::vec3& forward = camera.GetDirection();

		glm::vec3 right = glm::cross(forward, up);
		bool moved = false;

		glm::vec2 newMousePos = mainWindow.GetMousePos();
		glm::vec2 mouseDelta = (newMousePos - lastMousePos) * mouseSenisity;
		lastMousePos = newMousePos;

		if (mainWindow.IsKeyPressed(GLFW_KEY_W))
		{
			glm::vec3 step = camera.GetPosition() + forward * speed * ts;
			camera.SetPosition(step);
			moved = true;
		}
		if (mainWindow.IsKeyPressed(GLFW_KEY_S))
		{
			glm::vec3 step = camera.GetPosition() - forward * speed * ts;
			camera.SetPosition(step);
			moved = true;
		}

		if (mainWindow.IsKeyPressed(GLFW_KEY_D))
		{
			glm::vec3 step = camera.GetPosition() + right * speed * ts;
			camera.SetPosition(step);
			moved = true;
		}
		if (mainWindow.IsKeyPressed(GLFW_KEY_A))
		{
			glm::vec3 step = camera.GetPosition() - right * speed * ts;
			camera.SetPosition(step);
			moved = true;
		}

		if (mainWindow.IsKeyPressed(GLFW_KEY_Q))
		{
			glm::vec3 step = camera.GetPosition() + up * speed * ts;
			camera.SetPosition(step);
			moved = true;
		}
		if (mainWindow.IsKeyPressed(GLFW_KEY_E))
		{
			glm::vec3 step = camera.GetPosition() - up * speed * ts;
			camera.SetPosition(step);
			moved = true;
		}

		if (mainWindow.IsMousePressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			mainWindow.CursorMode(GLFW_CURSOR_DISABLED);
			if (mouseDelta != glm::vec2(0.0f))
			{
				mouseDelta *= rotationSpeed;
				glm::quat q = glm::normalize(glm::cross(
					glm::angleAxis(-mouseDelta.y, right),
					glm::angleAxis(-mouseDelta.x, up)
				));
				camera.SetDirection(glm::rotate(q, camera.GetDirection()));
				moved = true;
			}
		}
		else
		{
			mainWindow.CursorMode(GLFW_CURSOR_NORMAL);
		}

		if (moved)
		{
			camera.RecalculateInvView();
			renderer.ResetFrame();
		}
	}

}
