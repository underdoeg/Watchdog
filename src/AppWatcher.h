#pragma once

#include <thread>
#include <memory>
#include <chrono>
#include <atomic>
#include <pstreams/pstream.h>

#include "Config.h"


class AppWatcher{
public:
	AppWatcher();
	AppWatcher(Config::AppWatcher conf);
	~AppWatcher();

	void start();
	void stop();
	void process();

	const std::string& getPath();
	std::string getName();
	const std::string& getArgs();

private:
	void init();

	Config::AppWatcher config;
	std::shared_ptr<redi::ipstream> stream;
	bool bShouldRun;
};
