#include "GtkGui.h"

#include <gtkmm.h>
#include "../Config.h"


class GtkGui: public Gtk::Window{
public:
	GtkGui(){

	}
};

/////////////////////////////////
int runGtkGui(std::function<void ()> processFunction, std::function<void ()> shutdownFunction){
	auto app = Gtk::Application::create("ch.undef.Watchdog");

	GtkGui gui;


	Glib::signal_timeout().connect([&]{
		processFunction();
		return true;
	}, unsigned(Config::get().getProcessRate()*1000));

	gui.signal_delete_event().connect([&](GdkEventAny* evt){
		shutdownFunction();
		return false;
	});

	return app->run(gui);
}
