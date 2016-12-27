#pragma once

#include <functional>

int runGtkGui(std::function<void()> processFunction, std::function<void()> shutdownFunction);
