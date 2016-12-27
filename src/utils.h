#pragma once

#include <string>
#include <vector>
#include <typeinfo>


//
std::string strReplace(std::string s, const std::string &toReplace, const std::string &replaceWith);

std::string loadStringFromFile(std::string path);

// paths
std::string getexepath();

std::vector<std::string> splitString(const std::string& str, char delimiter);


std::vector<std::string> listDirectory(const std::string& path, std::vector<std::string> extensions={});
bool fileExists(const std::string& name);
bool createDirectory(const std::string& path);
bool isAbsolutePath(const std::string& path);
std::string getDirectory(const std::string& file);
void appendTrailingSlash(std::string& path);
std::string getFilename(const std::string& path);
bool deleteFile(const std::string& path);

//
std::string getIpAddress();

std::string makeUUID();

void takeScreenshot(const std::string& file);

