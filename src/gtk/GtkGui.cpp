#include "GtkGui.h"

#include <gtkmm.h>

class AppWatcherGui: public Gtk::VBox{
public:
	AppWatcherGui(std::shared_ptr<AppWatcher> appWatcher):app(appWatcher){
		runSwitch.set_active(true);

		int spacing = 10;

		set_margin_top(spacing);
		set_margin_bottom(spacing);
		set_margin_left(spacing);
		set_margin_right(spacing);

		set_spacing(spacing);

		auto file = Gtk::manage(new Gtk::HBox());
		file->pack_start(appChooser, true, true);
		file->pack_end(runSwitch, false, false);
		file->set_spacing(spacing);
		pack_start(*file, false, false);


		auto argsContainer = Gtk::manage(new Gtk::HBox());
		auto argsLabel = Gtk::manage(new Gtk::Label("Arguments"));
		argsContainer->pack_start(*argsLabel, false, false);
		argsContainer->pack_end(args, true, true);
		argsContainer->set_spacing(spacing);
		pack_start(*argsContainer, false, false);


		appChooser.set_title("select app to run");
		appChooser.set_filename(appWatcher->getPath());
		appChooser.set_sensitive(false);


		args.set_text(appWatcher->getArgs());
		args.set_sensitive(false);

		//auto spacer = Gtk::manage(new Gtk::Alignment());
		pack_end(logViewScroll, true, true);

		logViewScroll.add(logView);

		show_all();


		logView.get_buffer()->signal_insert().connect([&](const Gtk::TextBuffer::iterator& pos,const Glib::ustring& text, int size){
			//auto buf = logView.get_buffer();
			auto buf = logView.get_buffer();
			while(buf->get_line_count() > 2048){
				auto end = buf->begin();
				if(!end.forward_line())
					break;
				buf->erase(buf->begin(), end);
			}
		});

		appWatcher->onLog = [&](const std::string& msg){
			auto buf = logView.get_buffer();
			auto end = buf->end();
			buf->insert(end, msg);
			//end = buf->end();
			//logView.scroll_to(end);
		};


		///////////
		runSwitch.signal_state_changed().connect([&](Gtk::StateType state){
			if(!runSwitch.get_active()){
				app->stop();
			}else{
				app->start();
			}
		});
	}

private:
	std::shared_ptr<AppWatcher> app;
	Gtk::Label appPath;
	Gtk::Entry args;
	Gtk::FileChooserButton appChooser;
	Gtk::Switch runSwitch;
	Gtk::TextView logView;
	Gtk::ScrolledWindow logViewScroll;
};

class GtkGui: public Gtk::Window{
public:
	GtkGui(Context& c):ctx(c){
		set_default_size(600, 800);

		add(root);
		root.add(apps);

		for(auto& w: ctx.getWatchers()){
			addWatcher(w);
		}

		show_all();
	}

	void addWatcher(std::shared_ptr<AppWatcher> watcher){
		appWatchers.push_back(std::make_shared<AppWatcherGui>(watcher));
		apps.append_page(*appWatchers.back(), watcher->getName());
	}

private:
	Context& ctx;
	Gtk::VBox root;
	Gtk::Notebook apps;

	std::vector<std::shared_ptr<AppWatcherGui>> appWatchers;
};

/////////////////////////////////

static std::function<void()> gtkShutdownFunc;

void intHandlerGtk(int status) {
	gtkShutdownFunc();
	Gtk::Main::quit();
}

int runGtkGui(Context &ctx){
	signal(SIGINT, intHandlerGtk);
	signal(SIGTERM, intHandlerGtk);
	signal(SIGKILL, intHandlerGtk);

	gtkShutdownFunc = [&]{
		ctx.shutdown();
	};

	auto app = Gtk::Application::create("ch.undef.Watchdog");

	GtkGui gui(ctx);
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
