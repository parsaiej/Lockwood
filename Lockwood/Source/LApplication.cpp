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
{}

LApplication::~LApplication()
{}

void LApplication::Initialize(int argc, const char *argv[]) {
	m_Window->RegisterCallbackCursorPosition();
	m_Window->RegisterCallbackMouseButton();
	m_Window->RegisterCallbackKeyboard();

}

void LApplication::Run() {

	float dt;
	auto start = std::chrono::high_resolution_clock::now();
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
	auto end = std::chrono::high_resolution_clock::now();
	double duration = std::chrono::duration<double>(end - start).count();
	printf("FPS: %G", totalFrames / duration);

	m_Renderer->Cleanup();
	m_Window->Close();

}

void LApplication::OnGUI() {

}

void LApplication::Update(float _dt) {
	m_Renderer->Update();
}