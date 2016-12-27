#include <chrono>
#include <iostream>
#include <csignal>
#include <thread>

#include "NoGui.h"
#include "Config.h"

static bool ngKeepRunning = true;
static std::function<void()> ngShutdownFunc;

void intHandler(int status) {
	std::cout << "==================================================================> EXIT <==================================================================" << std::endl;
	ngKeepRunning = false;
	ngShutdownFunc();
}

int runNoGui(std::function<void ()> processFunction, std::function<void()> shutdownFunction){
	signal(SIGINT, intHandler);
	signal(SIGTERM, intHandler);
	signal(SIGKILL, intHandler);

	ngShutdownFunc = shutdownFunction;


	while(ngKeepRunning){
		auto now = std::chrono::high_resolution_clock::now();
		auto endTime = now + std::chrono::milliseconds(int(Config::get().getProcessRate()*1000));

		processFunction();

		if(endTime > std::chrono::high_resolution_clock::now())
			std::this_thread::sleep_until(endTime);
	}

	return 0;
}