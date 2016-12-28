#include "Context.h"
extern "C"{
#include <curl/curl.h>
#include <sys/stat.h>
}

Context::Context(){
	auto appWatchersConf = Config::get().getAppWatchers();

	ftpSettings = Config::get().getFtpSettings();

	nextFtpUpload = std::chrono::system_clock::now();

	for(auto awc: appWatchersConf){
		add(awc);
	}


	system("xset s off");
}

Context::~Context(){
	if(ftpUploadThread.joinable()){
		std::cout << "Waiting for FTP upload to finish" << std::endl;
		ftpUploadThread.join();
	}
	system("xset s default");
}

void Context::add(Config::AppWatcher awc){
	appWatchers.push_back(std::make_shared<AppWatcher>(awc));
}

void Context::process(){
	for(auto& aw: appWatchers){
		aw->process();
	}

	// check for pending logs
	std::string log;
	mutex.lock();
	log = pendingLogs;
	pendingLogs.clear();
	mutex.unlock();

	if(!log.empty()){
		log = "[GLOBAL]\n"+log;
		for(auto& aw: appWatchers){
			aw->sendLog(log);
		}
	}

	// check if ftp upload is due
	auto now = std::chrono::system_clock::now();
	if(ftpSettings.enabled && nextFtpUpload < now){
		// ftp upload
		uploadLogs();
		nextFtpUpload = std::chrono::system_clock::now() + std::chrono::minutes(long(ftpSettings.rate*60));
	}
}

void Context::shutdown(){
	std::cout << "\n==================================================================> EXIT <==================================================================" << std::endl;
	for(auto& aw: appWatchers){
		aw->stop();
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

std::vector<std::shared_ptr<AppWatcher> > Context::getWatchers(){
	return appWatchers;
}

void Context::logGlobal(const std::string &msg){
	mutex.lock();
	pendingLogs += msg;
	mutex.unlock();
}

void Context::uploadLogs(){
	if(ftpUploadThread.joinable()){
		std::cout << "Cannot upload logs. An upload is already running";
		return;
	}

	std::vector<std::string> files;
	for(auto& aw: appWatchers){
		aw->sendLog("upload logs to " + ftpSettings.host);
		files.push_back(aw->getLogFilePath());
	}

	auto settings = ftpSettings;

	ftpUploadThread = std::thread([&, settings, files]{
		for(auto& f: files){
			auto fileName = getFilename(f);

			struct stat fileInfo;
			if(stat(f.c_str(), &fileInfo)) {
				std::cout << "could not get file info for " << fileName;
				continue;
			}

			auto fsize = (curl_off_t)fileInfo.st_size;

			/* get a FILE * of the same file */
			FILE* fileHandle = fopen(f.c_str(), "rb");

			auto curl = curl_easy_init();
			if(curl) {
				curl_slist *headerlist=NULL;
				headerlist = curl_slist_append(headerlist, std::string("RNFR "+settings.folder+"/"+fileName+".tmp").c_str());
				headerlist = curl_slist_append(headerlist, std::string("RNTO "+settings.folder+"/"+fileName).c_str());


				/* ask libcurl to show us the verbose output */
				//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

				/* we want to use our own read function */
				// curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
				//curl_easy_setopt(curl, CURLOPT_FTP_SSL, CURLFTPSSL_CONTROL);
				//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
				//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
				//curl_easy_setopt(curl, CURLOPT_FTP_SSL, CURLOPT_FTPSSLAUTH);
				//curl_easy_setopt(curl, CURLOPT_FTPPORT, '-');

				/* enable uploading */
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
				curl_easy_setopt(curl, CURLOPT_PORT, settings.port);
				curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

				/* specify target */
				curl_easy_setopt(curl, CURLOPT_USERNAME, settings.user.c_str());
				curl_easy_setopt(curl, CURLOPT_PASSWORD, settings.password.c_str());
				//curl_easy_setopt(curl, CURLOPT_USERPWD, (settings.user+":"+settings.password).c_str());

				std::string url = "FTP://"+settings.host+"/"+settings.folder+"/"+fileName+".tmp";
				//url = "ftp://"+settings.user+":"+settings.password+"@"+settings.host;
				curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

				/* pass in that last of FTP commands to run after the transfer */
				curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

				/* now specify which file to upload */
				curl_easy_setopt(curl, CURLOPT_READDATA, fileHandle);

				/* Set the size of the file to upload (optional).  If you give a *_LARGE
				  option you MUST make sure that the type of the passed-in argument is a
				  curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
				  make sure that to pass in a type 'long' argument. */
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, fsize);

				/* Now run off and do what you've been told! */
				auto res = curl_easy_perform(curl);
				/* Check for errors */
				if(res != CURLE_OK)
					logGlobal("FTP: "+std::string(curl_easy_strerror(res)) + "\n");

				/* clean up the FTP commands list */
				curl_slist_free_all(headerlist);

				/* always cleanup */
				curl_easy_cleanup(curl);
			}
			fclose(fileHandle); /* close the local file */
		}
		curl_global_cleanup();
	});
}
