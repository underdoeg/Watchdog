#include <chrono>
#include <thread>

#include "Config.h"
#include "AppWatcher.h"

static bool keepRunning = true;
static std::vector<std::shared_ptr<AppWatcher>> appWatchers;

void intHandler(int status) {
	std::cout << "EXIT " << std::endl;
	appWatchers.clear();
	keepRunning = false;
}

int main(int argc, char** argv){
	signal(SIGINT, intHandler);
	signal(SIGTERM, intHandler);
	signal(SIGKILL, intHandler);

	Config config;
	auto appWatchersConf = config.getAppWatchers();


	for(auto awc: appWatchersConf){
		appWatchers.push_back(std::make_shared<AppWatcher>(awc));
	}

	while(keepRunning){
		auto now = std::chrono::high_resolution_clock::now();
		auto endTime = now + std::chrono::seconds(1);

		for(auto& aw: appWatchers){
			aw->process();
		}

		if(endTime > std::chrono::high_resolution_clock::now())
			std::this_thread::sleep_until(endTime);
	}

	appWatchers.clear();

	return 0;
}
