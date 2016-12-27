#include "Context.h"

Context::Context(){
	auto appWatchersConf = Config::get().getAppWatchers();

	for(auto awc: appWatchersConf){
		add(awc);
	}
}

void Context::add(Config::AppWatcher awc){
	appWatchers.push_back(std::make_shared<AppWatcher>(awc));
}

void Context::process(){
	for(auto& aw: appWatchers){
		aw->process();
	}
}

void Context::shutdown(){
	appWatchers.clear();
	std::this_thread::sleep_for(std::chrono::milliseconds(int(Config::get().getProcessRate()*1000)));
}
