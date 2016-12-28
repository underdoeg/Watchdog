#ifndef CONTEXT_H
#define CONTEXT_H

#include <memory>
#include <mutex>

#include "Config.h"
#include "AppWatcher.h"

class Context{
public:
    Context();
	~Context();

	void add(Config::AppWatcher awc);

	void process();

	void shutdown();

	std::vector<std::shared_ptr<AppWatcher>> getWatchers();

private:
	void logGlobal(const std::string& msg);

	void uploadLogs();

	std::vector<std::shared_ptr<AppWatcher>> appWatchers;
	Config::Ftp ftpSettings;
	std::chrono::system_clock::time_point nextFtpUpload;
	std::thread ftpUploadThread;

	//
	std::string pendingLogs;
	std::mutex mutex;
};

#endif // CONTEXT_H
