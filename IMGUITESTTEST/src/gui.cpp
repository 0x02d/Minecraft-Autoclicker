#include "gui.h"
#include "Globals.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

LRESULT CALLBACK WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
		case WM_SIZE: {
			if (gui::device && wideParameter != SIZE_MINIMIZED)
			{
				gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
				gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
				gui::ResetDevice();
			}
		}return 0;

		case WM_SYSCOMMAND: {
			if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
		}break;

		case WM_DESTROY: {
			PostQuitMessage(0);
		}return 0;

		case WM_LBUTTONDOWN: {
			gui::position = MAKEPOINTS(longParameter); // set click points
		}return 0;

		case WM_MOUSEMOVE: {
			if (wideParameter == MK_LBUTTON)
			{
				const auto points = MAKEPOINTS(longParameter);
				auto rect = ::RECT{ };

				GetWindowRect(gui::window, &rect);

				rect.left += points.x - gui::position.x;
				rect.top += points.y - gui::position.y;

				if (gui::position.x >= 0 &&
					gui::position.x <= gui::WIDTH &&
					gui::position.y >= 0 && gui::position.y <= 19)
					SetWindowPos(
						gui::window,
						HWND_TOPMOST,
						rect.left,
						rect.top,
						0, 0,
						SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
					);
			}

		}return 0;

	}

	return DefWindowProc(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName, const char* className) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassEx(&windowClass);

	window = CreateWindowA(
		className,
		windowName,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);

}

void gui::DestroyHWindow() noexcept {
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

// Handle Device Creation and Destruction
bool gui::CreateDevice() noexcept {

	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d) {
		return false;
	}

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // V-Sync

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0) {
		return false;
	}

	return true;

}

void gui::ResetDevice() noexcept {

	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL) {
		IM_ASSERT(0);
	}

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept {

	if (device) {
		device->Release();
		device = nullptr;
	}

	if (d3d) {
		d3d->Release();
		d3d = nullptr;
	}
}

// Handle ImGui Creation and Destruction
void gui::CreateImGui() noexcept {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	//io.Fonts->AddFontDefault();
	ImFont* mainFont = io.Fonts->AddFontFromFileTTF("C:\\Users\\User\\source\\repos\\IMGUITESTTEST\\IMGUITESTTEST\\ext\\Fonts\\arial.ttf", 18.f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowTitleAlign = ImVec2(0.5, 0.5);
	style.WindowRounding = 12;
	style.FrameRounding = 12;
	style.GrabRounding = 12;
	style.GrabMinSize = 20;

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept {

	// Cleanup
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// Render Functions
void gui::BeginRender() noexcept {

	MSG msg;

	while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

}

void gui::EndRender() noexcept {

	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0); // Clear DX Buffer

	if (device->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 Device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ResetDevice();
	}
}

void gui::Render() noexcept {

	ImGui::SetNextWindowPos({ 0 ,0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin(
		"TEST",
		&running,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove
	);

	/*
	ImGui::InputText("Auto Clicker Toggle Key Bind", &globals::acKeyBind, sizeof(char));
	ImGui::InputText("Blockhitting Toggle Key Bind", &globals::bhKeyBind, sizeof(char));
	ImGui::InputText("Jitter Toggle Key Bind", &globals::jKeyBind, sizeof(char));
	*/

	ImGui::Text("AC Bind -- DELETE");
	ImGui::Text("BH Bind -- HOME");
	ImGui::Text("Jitter Bind -- INSERT");
	ImGui::Text("Self Destruct -- END");

	ImGui::Checkbox("Toggle Clicker", &globals::toggle);
	//ImGui::SliderInt("CPS", &globals::TARGET_CPS, 0, 20);
	ImGui::SliderInt("Minimum CPS", &globals::MIN_CPS, 0, 20);
	ImGui::SliderInt("Maximum CPS", &globals::MAX_CPS, 0, 20);

	ImGui::Spacing();

	ImGui::Checkbox("Toggle Blockhitting", &globals::bhToggle);
	ImGui::SliderInt("Blockhit Chance", &globals::bhChance, 0, 100);

	ImGui::Spacing();

	ImGui::Checkbox("Toggle jittering", &globals::jToggle);
	ImGui::SliderInt("Jitter strength - X", &globals::jStrengthx, 0, 100);
	ImGui::SliderInt("Jitter strength - Y", &globals::jStrengthy, 0, 100);
	

	//ImGui::ShowStyleEditor();

	ImGui::End();
}