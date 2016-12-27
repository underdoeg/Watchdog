#include <sstream>

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
	stream->rdbuf()->kill(SIGTERM);
}

void AppWatcher::init(){
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

			stream = std::make_shared<redi::ipstream>("cd "+getDirectory(config.path)+" && "+config.path+" "+config.arguments);
		}
	}else if(stream){
		if(!bShouldRun){
			// kill process
			stream->rdbuf()->kill(SIGTERM);
		}
	}

//	if(stream){
//		std::string str;
//		while (*stream >> str) {
//			std::cout << str << std::endl;
//		}
	//	}
}

const std::string &AppWatcher::getPath(){
	return config.path;
}

