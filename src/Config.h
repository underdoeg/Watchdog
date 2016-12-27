#pragma once

#include <libconfig.h++>
#include <vector>
#include <iostream>

#include "utils.h"

class Config{
public:
	struct AppWatcher{
		std::string path;
		std::string arguments;
	};

	static Config& get(){
		static Config* config = new Config();
		return *config;
	}


	std::vector<AppWatcher> getAppWatchers(){
		std::vector<AppWatcher> ret;
		const auto& root = cfg.getRoot();
		try{
			for(const auto& app: root["apps"]){
				std::string appPath, appArgs;
				app.lookupValue("path", appPath);
				app.lookupValue("args", appArgs);
				ret.push_back({appPath, appArgs});
			}

		}catch(const libconfig::SettingNotFoundException &nfex){
			// Ignore.
		}

		if(ret.empty())
			return getDefaultAppWatchers();

		return ret;
	}

	float getProcessRate(){
		const auto& root = cfg.getRoot();
		try{
			return root["processRate"];
		}catch(...){
			return 1;
		}
	}

private:
	Config(){
		auto configPath = getexepath() + "config.cfg";
		try {
			cfg.readFile(configPath.c_str());
		} catch (libconfig::FileIOException &fioEx) {
			std::cerr << "Error loading config file: " << configPath << std::endl;
		} catch (libconfig::ParseException &pEx){
			std::cerr << "Error parsing config file: " << configPath << std::endl;
		}
	}

	libconfig::Config cfg;

	std::vector<AppWatcher> getDefaultAppWatchers(){
		return {{"", ""}};
	}
};
