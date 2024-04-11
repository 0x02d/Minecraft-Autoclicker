#pragma once
#include <d3d9.h>

namespace gui{
	constexpr int WIDTH = 600;
	constexpr int HEIGHT = 400;

	inline bool running = true;

	// WinAPI Window Variables
	inline HWND window = nullptr;
	inline WNDCLASSEX windowClass = {};

	// Points for Window Movement
	inline POINTS position = {};

	// DirectX State Variables
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presentParameters = {};

	// Handle Window Creation and Destruction
	void CreateHWindow(
		const char* windowName,
		const char* className
	) noexcept;

	void DestroyHWindow() noexcept;

	// Handle Device Creation and Destruction
	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	// Handle ImGui Creation and Destruction
	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;

	// Render Functions
	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;
}