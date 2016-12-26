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

	std::vector<AppWatcher> getAppWatchers(){
		auto configPath = getexepath() + "config.cfg";

		std::vector<AppWatcher> ret;

		libconfig::Config cfg;

		try {
			cfg.readFile(configPath.c_str());
		} catch (libconfig::FileIOException &fioEx) {
			std::cerr << "Error loading config file: " << configPath << std::endl;
			return getDefaultAppWatchers();
		} catch (libconfig::ParseException &pEx){
			std::cerr << "Error parsing config file: " << configPath << std::endl;
			return getDefaultAppWatchers();
		}

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

private:
	std::vector<AppWatcher> getDefaultAppWatchers(){
		return {{"", ""}};
	}
};
