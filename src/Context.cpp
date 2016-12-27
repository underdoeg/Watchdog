#include "Context.h"

Context::Context(){
	auto appWatchersConf = Config::get().getAppWatchers();

	ftpSettings = Config::get().getFtpSettings();

	nextFtpUpload = std::chrono::system_clock::now();

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

	// check if ftp upload is due
	auto now = std::chrono::system_clock::now();
	if(ftpSettings.enabled && nextFtpUpload < now){
		// ftp upload
	}
}

void Context::shutdown(){
	std::cout << "==================================================================> EXIT <==================================================================" << std::endl;
	for(auto& aw: appWatchers){
		aw->stop();
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

std::vector<std::shared_ptr<AppWatcher> > Context::getWatchers(){
	return appWatchers;
}
