#include "..\LApplication.h"
#include "..\LWindow.h"
#include "..\LVForwardPlusRenderCore.h"

#include <chrono>
#include <iostream>

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

	while (!m_Window->ShouldClose()) {
		m_Window->PollEvents();

		Update();
		m_Renderer->Draw();
	}

	m_Window->Close();

}

void LApplication::Update() {

}