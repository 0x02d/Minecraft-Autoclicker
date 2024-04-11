#pragma once

#include <cstdio>
#include <thread>
#include <chrono>
#include <random>
#include <Windows.h>
#include <iostream>
#include <random>

#include "Globals.h"
#include "gui.h"

namespace clicker {
	void ClickThread();
	void blockhitThread();
	void moveMouse(int x, int y);
	int jitterx(int lower, int upper);
	int jittery(int lower, int upper);

	void jitterThread();
};
