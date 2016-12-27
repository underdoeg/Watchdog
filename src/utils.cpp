#include <linux/limits.h>
#include <unistd.h>
#include <fstream>
#include <typeinfo>
#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>

#include "utils.h"


std::string loadStringFromFile(std::string path){
	std::ifstream t(path);
	return std::string((std::istreambuf_iterator<char>(t)),
					 std::istreambuf_iterator<char>());
}


std::string strReplace(std::string s, const std::string &toReplace, const std::string &replaceWith){
	auto pos = s.find(toReplace);
	if(pos == std::string::npos)
		return s;
	return(s.replace(pos, toReplace.length(), replaceWith));
}

std::string getexepath() {
	char result[PATH_MAX];
	ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
	std::string str( result, (count > 0) ? count : 0 );
	//std::reverse(str.begin(), str.end());
	return str.substr(0, str.find_last_of("/")+1);
}

std::vector<std::string> splitString(const std::string &str, char delimiter){
	std::vector<std::string> ret;
	std::istringstream stream(str);
	std::string s;
	while(std::getline(stream, s, delimiter)){
		ret.push_back(s);
	}
	return ret;
}


bool fileExists(const std::string &name) {
	return boost::filesystem::exists(name);
}

bool isAbsolutePath(const std::string &path){
	if(path.size() == 0)
		return false;
	return path[0] == '/';
}

std::vector<std::string> listDirectory(const std::string &path, std::vector<std::string> extensions){
	std::vector<std::string> list;

	for(auto f: boost::filesystem::directory_iterator(path)){
		if(!boost::filesystem::is_regular_file(f.status())) continue;

		if(extensions.size()){
			if(std::find(extensions.begin(), extensions.end(), f.path().extension().c_str()) != extensions.end())
				list.push_back(f.path().c_str());
		}else
			list.push_back(f.path().c_str());

	}

	//std::sort(list.begin(), list.end());

	std::sort(std::begin(list), std::end(list), [](std::string const &a, std::string const &b)
	{
		return std::lexicographical_compare(std::begin(a), std::end(a), std::begin(b), std::end(b), [](std::string::value_type a, std::string::value_type b)
		{
			return std::tolower(a) < std::tolower(b);
		});
	});

	return list;
}

bool createDirectory(const std::string &path){
	return boost::filesystem::create_directories(path);
}

std::string getDirectory(const std::string &file){
	return boost::filesystem::path(file).remove_filename().c_str();
}

std::string getFilename(const std::string &path){
	return boost::filesystem::path(path).filename().c_str();
}

bool deleteFile(const std::string &path){
	if(!fileExists(path))
		return false;
	return boost::filesystem::remove(path);
}


////////////////// MEDIA INFO

void appendTrailingSlash(std::string &path){
	if(path.size()==0)
		return;
	if(path[path.size()-1] != '/')
		path.append("/");
}


////////////////////////////////////////////////////////////////
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

std::string getIpAddress(){
	//TODO: this is not great and might return the wrong result
	struct ifaddrs * ifAddrStruct=NULL;
	struct ifaddrs * ifa=NULL;
	void * tmpAddrPtr=NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr) {
			continue;
		}

		if(strcmp(ifa->ifa_name, "lo") == 0)
			continue;

		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			//printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
			if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
			return addressBuffer;
		} else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
			// is a valid IP6 Address
			tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
			if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
			return addressBuffer;
		}
	}
	if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
	return "";
}


#include <boost/lexical_cast.hpp>
using boost::lexical_cast;
#include <boost/uuid/uuid.hpp>
using boost::uuids::uuid;
#include <boost/uuid/uuid_generators.hpp>
using boost::uuids::random_generator;
#include <boost/uuid/uuid_io.hpp>
std::string makeUUID(){
	return lexical_cast<std::string>((random_generator())());
}

///
void takeScreenshot(const std::string& file){
	const std::string com = "scrot "+file;
	system(com.c_str());
}
