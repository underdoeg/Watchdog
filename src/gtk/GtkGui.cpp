#include "GtkGui.h"

#include <gtkmm.h>


class GtkGui: public Gtk::Window{
public:
	GtkGui(){

	}
};

/////////////////////////////////
int runGtkGui(Context &ctx){
	auto app = Gtk::Application::create("ch.undef.Watchdog");

	GtkGui gui;
	Glib::signal_timeout().connect([&]{
		ctx.process();
		return true;
	}, unsigned(Config::get().getProcessRate()*1000));

	gui.signal_delete_event().connect([&](GdkEventAny* evt){
		ctx.shutdown();
		return false;
	});

	return app->run(gui);
}
