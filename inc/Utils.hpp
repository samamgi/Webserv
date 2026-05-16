#ifndef UTILS_HPP
#define UTILS_HPP

#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "PrintLog.hpp"

class ServerManager;

#define ERROR "📍"
#define RESET "\033[0m"
#define BLUE "\033[38;5;147m"
#define PINK "\033[38;5;207m"
#define GREEN "\033[32m"
#define PURPLE "\033[38;5;62m"
#define GREY "\033[38;5;236m"
#define RED "\033[31m"

#define CLIENT_HEADER_TIMEOUT 5
#define CLIENT_BODY_TIMEOUT 5

// cookies
struct CookieData
{
		std::string username;
		bool isAuthenticated;
		time_t lastAccessTime;
};

// listen
bool isInt(std::string const &value);
bool isLocal(std::string const &value);
bool isSocket(std::string const &value);
int strToInt(std::string const &value);
bool checkSocketAddress(std::string const &value);
std::string socketToIp(std::string const &value);
int socketToPort(std::string const &value);

// server_name
bool checkDns(std::vector<std::string> &tmp);
bool checkLabel(std::string const &str);

// body_size
unsigned int strToSize(std::string const &value);

// error_page
std::map<int, std::pair<std::string, std::string> > defaultErrorPages();
bool isErrorPage(std::string const &value);
bool isHtmlAddress(std::string const &value);

// signals
void setupSignal();
void handleSignal(int signum);

// timestamp
const std::string timeStamp();
const std::string getHttpDate();
std::string generateCookieId();

// use colors without affecting linelength
size_t visLen(const std::string &str);

// others
std::string intToString(const int &num);
std::string upperKey(std::string const &key);
void replaceContent(std::string &htmlContent, const std::string &target,
					const std::string &newContent);
std::string getQueryValue(const std::string &query, const std::string &key);
std::string getCookieValue(const std::string &cookie, const std::string &key);

// check file extension
bool isBinary(std::string location);
bool isFolder(std::string location);

// cleanup
void deleteArray(char **array);

// reference
std::map<int, std::string> getStatusCodeMap();

#endif
