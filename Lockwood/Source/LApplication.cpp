#include "..\LApplication.h"
#include "..\LWindow.h"
#include "..\LVForwardPlusRenderCore.h"

#include <chrono>
#include <iostream>

#include <imgui.h>

LApplication::LApplication()
	: m_Window(std::make_unique<LWindow>(LUtility::ScreenWidth, LUtility::ScreenHeight, "Lockwood")),
	  m_Renderer(std::make_unique<LVForwardPlusRenderCore>(m_Window->GetHandle()))
{
	//For demonstration purposes we modify the renderer's clear color
	//through the GUI interface.
	m_ClearR = 1.0f;
	m_ClearG = 1.0f;
	m_ClearB = 1.0f;
	m_TextureSet = 0;
	m_HeightMag = 0.1f;
}

LApplication::~LApplication()
{}

void LApplication::Initialize(int argc, const char *argv[]) {
	m_Renderer->RegisterCallbackGUI(std::bind(&LApplication::OnGUI, this));
}

void LApplication::OnGUI() {
	ImGui::Begin("Application");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat3("Background", &m_ClearR, 0.0f, 1.0f);
	ImGui::SliderInt("Texture Set", &m_TextureSet, 0, 5);
	ImGui::SliderFloat("Displacement", &m_HeightMag, 0.0f, 1.0f);
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
	m_Renderer->SetClearColor(m_ClearR, m_ClearG, m_ClearB);
	m_Renderer->UpdateTextureSet(m_TextureSet);
	m_Renderer->SetHeightMagnitude(m_HeightMag);
}