#ifndef CONTEXT_H
#define CONTEXT_H

#include <memory>

#include "Config.h"
#include "AppWatcher.h"

class Context{
public:
    Context();

	void add(Config::AppWatcher awc);

	void process();

	void shutdown();

private:
    std::vector<std::shared_ptr<AppWatcher>> appWatchers;
};

#endif // CONTEXT_H
