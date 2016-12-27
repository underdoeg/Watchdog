#include "Config.h"
#include "AppWatcher.h"
#include "NoGui.h"

#ifdef WITH_GTK_GUI
#include "gtk/GtkGui.h"
#endif

char* getCmdOption(char ** begin, char ** end, const std::string & option){
	char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end){
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option){
	return std::find(begin, end, option) != end;
}

/////////////////////

static std::vector<std::shared_ptr<AppWatcher>> appWatchers;

int main(int argc, char** argv){

	bool useGui = false;
#ifdef WITH_GTK_GUI
	useGui = true;
#endif

	auto appWatchersConf = Config::get().getAppWatchers();

	for(auto awc: appWatchersConf){
		appWatchers.push_back(std::make_shared<AppWatcher>(awc));
	}

	auto shutdownFunction = [&]{
		appWatchers.clear();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	};

	auto processFunction = [&]{
		for(auto& aw: appWatchers){
			aw->process();
		}
	};

	int res = 0;

#ifdef WITH_GTK_GUI
	if(useGui)
		res = runGtkGui(processFunction, shutdownFunction);
#endif

	if(!useGui)
		res = runNoGui(processFunction, shutdownFunction);

	appWatchers.clear();

	return res;
}
