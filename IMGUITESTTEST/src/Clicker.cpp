#include "Clicker.h"

// int Delay = 20 + rand() % cps;

std::random_device rd;
std::mt19937 gen(rd());

void clicker::ClickThread() {

	while (gui::running == true) {
		if (GetAsyncKeyState(VK_DELETE)) {
			globals::toggle = !globals::toggle;
			Sleep(100);
		}
		if (globals::toggle == true && GetAsyncKeyState(VK_LBUTTON) && !GetAsyncKeyState(VK_RBUTTON)) {
			int min = 500 / globals::MIN_CPS;
			int max = 500 / globals::MAX_CPS;
			std::uniform_int_distribution<> clickDistribution(max, min); // Random delay between 20 and (20 + cps)
			int firstDelay = clickDistribution(gen);
			int secondDelay = clickDistribution(gen);

			//int Delay = (500 / globals::MIN_CPS) + rand() % (500 / globals::MAX_CPS);

			//int ClickDelay = delay; // Generate a random delay for this click
			PostMessage(FindWindowA("LWJGL", nullptr), 0x201, 0, 0);
			Sleep(firstDelay);
			//printf("CLICK_DELAY: %i\n", firstDelay);
			PostMessage(FindWindowA("LWJGL", nullptr), 0x202, 0, 0);
			//int Delay = delay; // Delay after click (can be random or fixed, as per your requirement)
			Sleep(secondDelay);
			//printf("DELAY: %i\n", secondDelay);

			globals::swing = !globals::swing;
		}
		else {
			Sleep(10);
		}

	}
}


void clicker::blockhitThread() {

	while (gui::running == true) {
		if (GetAsyncKeyState(VK_HOME)) {
			globals::bhToggle = !globals::bhToggle;
			Sleep(100);
		}
		if (globals::bhToggle == true && GetAsyncKeyState(VK_LBUTTON) && !GetAsyncKeyState(VK_RBUTTON) && globals::swing == true) {

			int min = 500 / globals::MIN_CPS;
			int max = 500 / globals::MAX_CPS;
			std::uniform_int_distribution<> clickDistribution(max, min); // Random delay between 20 and (20 + cps)
			int firstDelay = clickDistribution(gen);

			std::uniform_int_distribution<> percentage(0, 100);

			globals::bhPerc = rand() % 101;

			int Delay = clickDistribution(gen);
			int secondDelay = clickDistribution(gen);

			int perc = globals::bhPerc;

			if (perc < globals::bhChance) {
				PostMessage(FindWindowA("LWJGL", nullptr), 0x204, 0, 0);

				Sleep(Delay);

				//printf("Block hit delay: %i\n", Delay);
				PostMessage(FindWindowA("LWJGL", nullptr), 0x205, 0, 0);

			}
			printf("bhPerc: % i --- bhChance: %i\n", globals::bhPerc, globals::bhChance);
			Sleep(50);
		}
		else {
			Sleep(10);
		}
	}
}

void clicker::moveMouse(int x, int y) {
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dx = x;
	input.mi.dy = y;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(INPUT));
}

int clicker::jitterx(int lower, int upper) {

	//int ret = (-1 * globals::jStrengthx) + (rand() % (globals::jStrengthx - -1 * globals::jStrengthx));

	/*
	int min = -1 * globals::jStrengthx;
	int max = globals::jStrengthx;
	std::uniform_int_distribution<> clickDistribution(min, max);

	int ret = clickDistribution(gen);
	*/

	//int ret = rand() % globals::jStrengthx;

	int ret = lower + (rand() % (upper - lower));

	return ret;
}

int clicker::jittery(int lower, int upper) {

	// int ret = (-1 * globals::jStrengthy) + (rand() % (globals::jStrengthy - -1 * globals::jStrengthy));

	/*
	int min = -1 * globals::jStrengthy;
	int max = globals::jStrengthy;
	std::uniform_int_distribution<> clickDistribution(min, max);

	int ret = clickDistribution(gen);
	*/

	//int ret = rand() % globals::jStrengthy;

	int ret = lower + (rand() % (upper - lower));

	return ret;
}

void clicker::jitterThread() {
	while (gui::running == true) {
		
		if (GetAsyncKeyState(VK_INSERT)) {
			globals::jToggle = !globals::jToggle;
			Sleep(100);
		}

		if (globals::jToggle == true) {
			if (GetAsyncKeyState(VK_LBUTTON) && !GetAsyncKeyState(VK_RBUTTON)) {
				int x = jitterx(-1 * globals::jStrengthx, globals::jStrengthx);
				int y = jittery(-1 * globals::jStrengthy, globals::jStrengthy);

				moveMouse(x, y);

				printf("X: %i --- Y: %i\n", x, y);

				Sleep(10);

			}
			else {
				Sleep(10);
			}
		}
		else {
			Sleep(10);
		}
	}
}