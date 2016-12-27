#pragma once

#include <thread>
#include <memory>
#include <array>
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

	std::__cxx11::string getPath();
	std::string getName();
	const std::string& getArgs();

	std::function<void(const std::string&)> onLog;

private:
	void init();
	void sendLog(const std::string& msg);

	std::string getLogFilePath();

	std::array<char, 2048> log;

	Config::AppWatcher config;
	std::shared_ptr<redi::ipstream> stream;
	bool bShouldRun;
};
