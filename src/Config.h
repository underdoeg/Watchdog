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

	struct Ftp{
		Ftp(){
			enabled = false;
			rate = 4;
			port = 21;
		}

		bool enabled;
		float rate;
		std::string host, user, password, folder;
		int port;
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

	Ftp getFtpSettings(){
		Ftp ftp;

		ftp.enabled = false;

		const auto& root = cfg.getRoot();
		try{
			auto& ftpRoot = root["ftp"];
			ftpRoot.lookupValue("enabled", ftp.enabled);
			ftpRoot.lookupValue("host", ftp.host);
			ftpRoot.lookupValue("user", ftp.user);
			ftpRoot.lookupValue("password", ftp.password);
			ftpRoot.lookupValue("folder", ftp.folder);
			ftpRoot.lookupValue("rate", ftp.rate);
			ftpRoot.lookupValue("port", ftp.port);
		}catch(const libconfig::SettingNotFoundException &nfex){
			std::cout << "Error loading ftp settings" << std::endl;
		}

		return ftp;
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
