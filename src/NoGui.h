#pragma once

#include <functional>

int runNoGui(std::function<void()> processFunction, std::function<void()> shutdownFunction);
