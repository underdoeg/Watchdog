#include <sstream>
#include <fstream>
#include <iomanip>
#include <ios>
#include "AppWatcher.h"

AppWatcher::AppWatcher(){
	init();
}

AppWatcher::AppWatcher(Config::AppWatcher conf){
	config = conf;
	init();
	start();
}

AppWatcher::~AppWatcher(){
	if(!stream)
		return;
	stop();
	process();
	//stream->rdbuf()->kill(SIGTERM);
}

void AppWatcher::init(){
}

void AppWatcher::sendLog(const std::string &m){
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%X");

	std::string msg = "\n[" + ss.str() + "]\n" + m;

	//std::cout << msg << std::endl;
	{
		std::ofstream outfile;
		outfile.open(getLogFilePath(), std::ios_base::app);
		outfile << msg;
	}

	if(onLog)
		onLog(msg);
}

std::string AppWatcher::getLogFilePath(){
	auto path = getexepath()+"logs/";
	createDirectory(path);

	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");

	path += ss.str();
	path += "-";
	path += getName();
	path += ".txt";
	return path;
}

void AppWatcher::start(){
	bShouldRun = true;
}

void AppWatcher::stop(){
	bShouldRun = false;
}

void AppWatcher::process(){
	// check if the stream is running;
	if(!stream || !stream->is_open() || stream->rdbuf()->exited()){
		if(bShouldRun){

			//start again
			if(config.path.empty())
				return;

			std::string path = getDirectory(config.path);
			if(path.empty())
				path = "/usr/bin/";

			sendLog("start "+getName()+"\n----------------------------------------------------------------------------------------------------------------\n");

			stream = std::make_shared<redi::ipstream>("cd "+ path +" && "+config.path+" "+config.arguments);
		}
	}else if(stream){
		if(!bShouldRun){

			// get pending messages
			if(stream->rdbuf()->in_avail()){
				std::string msg;
				std::string str;
				while (*stream >> str) {
					msg += str+"\n";
				}
				sendLog(msg);
			}

			// kill process
			stream->rdbuf()->kill(SIGTERM);
			stream.reset();
		}
	}

	if(stream){

		std::string msg;
		int count = 0;
		while(stream->rdbuf()->in_avail() && count < 32){
			std::string line;
			std::getline(*stream, line);
			msg += line;
			msg += "\n";
			count ++;
		}

		if(!msg.empty())
			sendLog(msg);
	}
}

std::string AppWatcher::getPath(){
	if(!fileExists(config.path))
		return "/usr/bin/"+config.path;
	return config.path;
}

std::string AppWatcher::getName(){
	if(config.path.empty())
		return "Not set";
	return getFilename(config.path);
}

const std::string &AppWatcher::getArgs(){
	return config.arguments;
}

