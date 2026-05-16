#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>

#include <ctime>
#include <map>
#include <set>
#include <vector>

#include "HttpRequest.hpp"
#include "ParsingConf.hpp"
#include "Response.hpp"
#include "Utils.hpp"

class ServerManager;

#define REQ_LINE_TIMEOUT 5

struct ClientConnection
{
		int clientFd;
		struct sockaddr_in clientAddr;
		socklen_t clientLen;
		HttpRequest req;
		Response *resp;
		std::pair<int, std::string> incoming;
		bool requestComplete;
		time_t lastActivityTime;
		ssize_t bytesSent;

		ClientConnection(int fd, const struct sockaddr_in &addr, socklen_t len,
						 const std::pair<int, std::string> &inc, ServerManager &sm);
		~ClientConnection();
};

class ServerManager
{
	private:
		bool _running;
		int _reqCount;
		int _rspCount;
		std::vector<ServerData> _serverData;
		std::set<std::pair<int, std::string> > _uniqueListens;
		std::vector<int> _socketFd;
		std::vector<struct sockaddr_in> _servAddr;
		int _inputFd;
		Script _script;
		std::map<std::string, CookieData> _sessions;
		std::map<int, ClientConnection *> _clients;
		std::vector<struct pollfd> _polls;

	public:
		ServerManager(ParsingConf &parsData);
		~ServerManager();

		void servSetup();
		void servRun();
		void servQuit();

		std::vector<ServerData> getServersList() const;
		Script &getScript();
		int getReqCount() const;
		int getRspCount() const;
		std::set<std::pair<int, std::string> > getUniqueListens();
		CookieData *getSession(const std::string &sessionId);
		std::string createSession(const std::string &username);
		void servListen(std::pair<int, std::string> _listens);
		void servInput();
		std::pair<int, std::string> getSocketData(int socketFd);

		void handleNewConnection(int listeningSocket);
		void handleClient(struct pollfd &pfd);
		void handleRead(int clientFd);
		void handleWrite(int clientFd);
		void closeConnection(int clientFd);
		void checkErrors();
};

#endif