#include "gui.h"
#include "Clicker.h"

int APIENTRY wWinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	PWSTR args,
	int commandShow
) {

	if (globals::debug == true){
		FILE* fDummy;
		AllocConsole();
		freopen_s(&fDummy, "CONOUT$", "w", stdout);
	}

	std::thread(clicker::ClickThread).detach();
	std::thread(clicker::blockhitThread).detach();
	std::thread(clicker::jitterThread).detach();

	gui::CreateHWindow("Test Menu", "Test Class");
	gui::CreateDevice();
	gui::CreateImGui();

	while (gui::running) {
		gui::BeginRender();
		gui::Render();
		gui::EndRender();

		if (GetAsyncKeyState(VK_END)) {
			gui::running = false;
		}
		if (gui::running == false) {
			break;
		}
	}
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();
}