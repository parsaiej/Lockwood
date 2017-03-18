#include "..\LApplication.h"
#include "..\LWindow.h"
#include "..\LVForwardPlusRenderCore.h"

#include <chrono>
#include <iostream>

#include <imgui.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

LApplication::LApplication()
	: m_Window(std::make_unique<LWindow>(SCREEN_WIDTH, SCREEN_HEIGHT, "Lockwood")),
	  m_Renderer(std::make_unique<LVForwardPlusRenderCore>(m_Window->GetHandle()))
{
	m_TestFloat = 0.0f;
}

LApplication::~LApplication()
{}

void LApplication::Initialize(int argc, const char *argv[]) {
	m_Renderer->RegisterCallbackGUI(std::bind(&LApplication::OnGUI, this));
}

void LApplication::OnGUI() {
	ImGui::Begin("Application Metrics");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Some Other Window");
	ImGui::SliderFloat("float", &m_TestFloat, 0.0f, 1.0f);
	ImGui::End();
}

void LApplication::Run() {

	float dt;
	auto curr  = std::chrono::high_resolution_clock::now();
	auto prev  = std::chrono::high_resolution_clock::now();
	
	int totalFrames = 0;
	while (!m_Window->ShouldClose()) {
		m_Window->PollEvents();

		curr = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float>(curr - prev).count();

		if (dt >= 1.0f / 60.0f) {
			prev = curr;
			Update(dt);
		}

		m_Renderer->Draw();
		totalFrames++;
	}

	m_Renderer->Cleanup();
	m_Window->Close();

}

void LApplication::Update(float _dt) {
	m_Renderer->Update();
}