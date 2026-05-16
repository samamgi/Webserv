#include "ServerManager.hpp"
#include "HttpRequest.hpp"
#include "Response.hpp"
#include <cerrno>
#include <fcntl.h>
#include <ctime>
#include <unistd.h>

ClientConnection::ClientConnection(int fd, const struct sockaddr_in &addr, socklen_t len,
								   const std::pair<int, std::string> &inc, ServerManager &sm)
	: clientFd(fd), clientAddr(addr), clientLen(len), req(inc, sm), resp(NULL), incoming(inc),
	  requestComplete(false), bytesSent(0)
{
		time(&lastActivityTime);
}

ClientConnection::~ClientConnection() { delete resp; }

ServerManager::ServerManager(ParsingConf &parsData) : _running(false), _reqCount(0), _rspCount(0)
{
		_serverData = parsData.servers;
}

ServerManager::~ServerManager()
{
		for (std::map<int, ClientConnection *>::iterator it = _clients.begin();
			 it != _clients.end(); ++it)
		{
				close(it->first);
				delete it->second;
		}
		for (size_t i = 0; i < _socketFd.size(); i++)
				close(_socketFd[i]);
}

void ServerManager::servSetup()
{
		for (size_t i = 0; i < _serverData.size(); i++)
		{
				for (size_t j = 0; j < _serverData[i].getListens().size(); j++)
						_uniqueListens.insert(_serverData[i].getListens()[j]);
		}

		graTopLine();
		graTime("Listening Sockets Setup");
		graEmptyLine();
		for (std::set<std::pair<int, std::string> >::iterator it = _uniqueListens.begin();
			 it != _uniqueListens.end(); ++it)
				servListen(*it);
		graBottomLine();
}

void ServerManager::servListen(std::pair<int, std::string> _listens)
{
		int newsocket = socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in newaddr;

		if (newsocket < 0)
		{
				graError("Socket creation error");
				return;
		}

		int opt = 1;
		if (setsockopt(newsocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
				graError("setsockopt failed");
				close(newsocket);
				return;
		}

		if (fcntl(newsocket, F_SETFL, O_NONBLOCK) < 0)
		{
				graError("Nonblocking setup error");
				close(newsocket);
				return;
		}

		std::memset(&newaddr, 0, sizeof(newaddr));
		newaddr.sin_family = AF_INET;
		newaddr.sin_port = htons(_listens.first);
		newaddr.sin_addr.s_addr = inet_addr(_listens.second.c_str());

		if (bind(newsocket, (struct sockaddr *)&newaddr, sizeof(newaddr)) < 0)
		{
				graError("Binding error for " + _listens.second + ":" +
						 intToString(_listens.first));
				close(newsocket);
				return;
		}

		if (listen(newsocket, 128) < 0)
		{
				graError("Listen error for " + _listens.second + ":" + intToString(_listens.first));
				close(newsocket);
				return;
		}

		_socketFd.push_back(newsocket);
		_servAddr.push_back(newaddr);
		graTextElement(_listens.second + ":" + intToString(_listens.first));
}
struct PollFdIsInvalid
{
		bool operator()(const struct pollfd &pfd) const { return pfd.fd == -1; }
};

void ServerManager::servRun()
{
		for (size_t i = 0; i < _socketFd.size(); ++i)
		{
				struct pollfd pfd;
				pfd.fd = _socketFd[i];
				pfd.events = POLLIN;
				pfd.revents = 0;
				_polls.push_back(pfd);
		}

		struct pollfd pfdStdin;
		pfdStdin.fd = STDIN_FILENO;
		pfdStdin.events = POLLIN;
		pfdStdin.revents = 0;
		_polls.push_back(pfdStdin);
		_inputFd = STDIN_FILENO;

		_running = true;
		printBoxMsg("Server Running");

		while (_running)
		{
				_polls.erase(std::remove_if(_polls.begin(), _polls.end(), PollFdIsInvalid()),
							 _polls.end());  // THERE 6

				int check = poll(_polls.data(), _polls.size(), 1000); // THERE 1
				if (check < 0)
				{
						if (errno == EINTR) // THERE 5
								continue;
						printBoxError("Poll error");
						break;
				}

				checkErrors();

				if (check == 0)
						continue;

				for (size_t i = 0; i < _polls.size(); ++i)
				{
						if (_polls[i].revents > 0)
						{
								bool socketListening = false;
								for (size_t j = 0; j < _socketFd.size(); ++j)
								{
										if (_polls[i].fd == _socketFd[j])
										{
												socketListening = true;
												break;
										}
								}

								if (socketListening)
										handleNewConnection(_polls[i].fd);
								else if (_polls[i].fd == _inputFd)
										servInput();
								else
										handleClient(_polls[i]);
						}
				}
		}

		for (size_t i = 0; i < _socketFd.size(); i++)
				close(_socketFd[i]);
		_socketFd.clear();
}

void ServerManager::handleNewConnection(int listeningSocket)
{
		struct sockaddr_in clientAddr;
		socklen_t clientLen = sizeof(clientAddr);
		int clientFd = accept(listeningSocket, (struct sockaddr *)&clientAddr, &clientLen);

		if (clientFd < 0)
		{
				printBoxError("Accept failed");
				return;
		}

		if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
		{
				printBoxError("Failed to set client socket to non-blocking");
				close(clientFd);
				return;
		}

		_reqCount++;
		std::pair<int, std::string> incoming = getSocketData(listeningSocket);
		try
		{
				_clients[clientFd] = new ClientConnection(clientFd, clientAddr, clientLen, incoming, *this);
		}
		catch (const std::exception &e)
		{
				close(clientFd);
				printBoxError("Failed to allocate client connection");
				return;
		}

		struct pollfd pfd;
		pfd.fd = clientFd;
		pfd.events = POLLIN;
		pfd.revents = 0;
		_polls.push_back(pfd);

		printBoxMsg("New connection accepted on fd " + intToString(clientFd));
}

void ServerManager::handleClient(struct pollfd &pfd)
{
		if (_clients.find(pfd.fd) == _clients.end())
				return;

		if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL))
		{
				closeConnection(pfd.fd);
				return;
		}

		if (pfd.revents & POLLIN)
		{
				handleRead(pfd.fd);
		}
		if (pfd.revents & POLLOUT)
		{
				handleWrite(pfd.fd);
		}
}

void ServerManager::handleRead(int clientFd)
{
		char buffer[4096];
		ssize_t bytes = recv(clientFd, buffer, sizeof(buffer), 0); // THERE 2

		ClientConnection *conn = _clients[clientFd];

		if (bytes > 0)
		{
				time(&conn->lastActivityTime);
				conn->req.sendBuffer(buffer, bytes);
				int parsingState = conn->req.getParsingState();
				if (parsingState == DONE || parsingState == ERR)
				{
						conn->requestComplete = true;
						if (conn->resp)
						{
								delete conn->resp;
								conn->resp = NULL;
						}
						conn->resp = new Response(conn->req);
						if (parsingState == DONE && conn->req.getStatusCode() < 400)
						{
								conn->resp->setContent(conn->req.getFullPath(),
													   conn->req.getHttpMethod());
						}
						conn->resp->setClientFd(clientFd);
						conn->resp->prepResponse(conn->incoming);
						for (size_t i = 0; i < _polls.size(); ++i)
						{
								if (_polls[i].fd == clientFd)
								{
										_polls[i].events = POLLOUT;
										break;
								}
						}
				}
		}
		else if (bytes == 0)
		{
				closeConnection(clientFd);
		}
		else // THERE 4
		{
				printBoxError("Recv error");
				closeConnection(clientFd); // THERE 3
		}
}

void ServerManager::handleWrite(int clientFd)
{
		ClientConnection *conn = _clients[clientFd];
		if (!conn || !conn->requestComplete || !conn->resp)
				return;

		const std::string &response = conn->resp->getResponse();
		ssize_t bytes_sent = send(clientFd, response.c_str() + conn->bytesSent,
								  response.length() - conn->bytesSent, 0);

		if (bytes_sent > 0)
		{
				conn->bytesSent += bytes_sent;
				if (conn->bytesSent >= (ssize_t)response.length())
				{
						_rspCount++;
						closeConnection(clientFd);
				}
		}
		else
		{
				printBoxError("Send error");
				closeConnection(clientFd);
		}
}

void ServerManager::closeConnection(int clientFd)
{
		close(clientFd);

		if (_clients.count(clientFd))
		{
				delete _clients[clientFd];
				_clients.erase(clientFd);
		}

		for (std::vector<struct pollfd>::iterator it = _polls.begin(); it != _polls.end(); ++it)
		{
				if (it->fd == clientFd)
				{
						it->fd = -1; // Mark for removal
						break;
				}
		}
		printBoxMsg("Connection closed on fd " + intToString(clientFd));
}

void ServerManager::checkErrors()
{
		time_t now;
		time(&now);

		for (std::map<int, ClientConnection *>::iterator it = _clients.begin();
			 it != _clients.end(); ++it)
		{
				ClientConnection *conn = it->second;
				if (conn->requestComplete)
						continue;

				double timeDiff = difftime(now, conn->lastActivityTime);

				int state = conn->req.getParsingState();
				bool error = false;

				if (state >= SKIP && state <= REQ_LINE && timeDiff > REQ_LINE_TIMEOUT)
				{
						conn->req.setStatusCode(E_408); //check here
						error = true;
				}
				else if (state == HEADERS && timeDiff > CLIENT_HEADER_TIMEOUT)
				{
						conn->req.setStatusCode(E_408);
						error = true;
				}
				else if (state == BODY && timeDiff > CLIENT_BODY_TIMEOUT)
				{
						conn->req.setStatusCode(E_408);
						error = true;
				}

				if (error)
				{
						conn->requestComplete = true;
						if (conn->resp)
						{
								delete conn->resp;
								conn->resp = NULL;
						}
						conn->resp = new Response(conn->req);
						conn->resp->setClientFd(conn->clientFd);
						conn->resp->prepResponse(conn->incoming);
						for (size_t i = 0; i < _polls.size(); ++i)
						{
								if (_polls[i].fd == conn->clientFd)
								{
										_polls[i].events = POLLOUT;
										break;
								}
						}
				}
		}
}

std::pair<int, std::string> ServerManager::getSocketData(int socketFd)
{
		for (size_t i = 0; i < _socketFd.size(); ++i)
		{
				if (_socketFd[i] == socketFd)
				{
						struct sockaddr_in socketIn = _servAddr[i];
						char ipStr[INET_ADDRSTRLEN];
						inet_ntop(AF_INET, &socketIn.sin_addr, ipStr, INET_ADDRSTRLEN);
						int portIn = ntohs(socketIn.sin_port);
						return std::make_pair(portIn, ipStr);
				}
		}
		return std::make_pair(-1, "");
}

void ServerManager::servQuit()
{
		if (_running)
				_running = false;
		printBoxMsg("Server quit");
}

void ServerManager::servInput()
{
		char buffer[256];
		ssize_t charsRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
		if (charsRead > 0)
		{
				buffer[charsRead] = '\0';
				std::string cmd(buffer);
				if (!cmd.empty() && cmd[cmd.length() - 1] == '\n')
						cmd.erase(cmd.length() - 1);
				if (cmd == "quit" || cmd == "q")
						servQuit();
				else if (cmd == "status" || cmd == "s")
						printServersStatus(*this);
				else
						printBoxError("Command unavailable - [q/s]");
		}
		else
	{
			for (size_t i = 0; i < _polls.size(); ++i)
			{
					if (_polls[i].fd == _inputFd)
					{
							_polls[i].fd = -1;
							break;
					}
			}
	}
}

std::vector<ServerData> ServerManager::getServersList() const { return _serverData; }

Script &ServerManager::getScript() { return _script; }

int ServerManager::getReqCount() const { return _reqCount; }

int ServerManager::getRspCount() const { return _rspCount; }

std::set<std::pair<int, std::string> > ServerManager::getUniqueListens() { return _uniqueListens; }

std::string ServerManager::createSession(const std::string &username)
{
		std::string sessionId = generateCookieId();
		CookieData newSession;
		newSession.username = username;
		newSession.isAuthenticated = true;
		newSession.lastAccessTime = std::time(NULL);
		this->_sessions[sessionId] = newSession;
		return sessionId;
}

CookieData *ServerManager::getSession(const std::string &sessionId)
{
		if (this->_sessions.find(sessionId) == this->_sessions.end())
				return NULL;
		const int SESSION_TIMEOUT = 3600;
		time_t now = std::time(NULL);
		CookieData &session = this->_sessions[sessionId];
		if (now - session.lastAccessTime > SESSION_TIMEOUT)
		{
				this->_sessions.erase(sessionId);
				return NULL;
		}
		session.lastAccessTime = now;
		return &session;
}