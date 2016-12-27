#include "Config.h"
#include "AppWatcher.h"
#include "NoGui.h"
#include "Context.h"

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



int main(int argc, char** argv){

	Context ctx;

#ifdef WITH_GTK_GUI
	if(cmdOptionExists(argv, argv+argc, "--no-gui"))
		return runNoGui(ctx);
	else
		return runGtkGui(ctx);
#else
	return runNoGui(ctx);
#endif
}
