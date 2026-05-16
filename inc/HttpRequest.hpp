#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#define SKIP 1
#define REQ_LINE 2
#define HEADERS 3
#define BODY 4
#define BOUNDARY 5
#define HEADERS2 6
#define BODY2 7
#define DONE 0
#define ERR -1
#define CRLF "\r\n"
#define TWO_CRLF "\r\n\r\n"

#include "Headers.hpp"
#include "HttpParser.hpp"
#include "RequestLine.hpp"
#include "Script.hpp"
#include "ServerData.hpp"
#include "Uri.hpp"
#include "Utils.hpp"

class RequestLine;
class Uri;
class ServerManager;
class Headers;
class Script;

struct MultiBody
{
		Headers bodyHeader;
		std::string bodyContent;

		MultiBody() {}

		MultiBody(const Headers &header, const std::string &content)
			: bodyHeader(header), bodyContent(content)
		{
		}
};

class HttpRequest
{
	private:
		std::vector<MultiBody> _bodies;
		RequestLine *req_line;
		Uri *uri;
		std::pair<std::string, std::string> host;
		std::pair<std::string, std::string> _fullPath;
		std::string location;
		Headers *headers;
		std::string body;
		std::size_t body_len;
		std::size_t max_body_size;
		std::string boundary;
		bool boundary_flag;
		int code;
		int state;
		int body_state;
		std::string fullRequest;
		std::string _rspType;
		std::string _cgiInterpreterPath;
		std::pair<int, std::string> incoming;
		ServerManager &server;
		LocationConf blockLoc;
		bool _makeNew;
		MultiBody _newBody;

		HttpRequest();

		void checkHost(std::map<std::string, std::vector<std::string> >::const_iterator it,
					   ServerData &serv);
		void finalHeadersParsingRoutine();
		void setFullPath(ServerData const &serv);
		void setLocation(std::map<std::string, LocationConf> const &location,
						 std::string const &path);
		void manyBodiesRoutine(std::size_t found);
		void runSimpleTest();

	public:
		HttpRequest(std::pair<int, std::string> incoming, ServerManager &server);
		HttpRequest(HttpRequest const &src);
		~HttpRequest();

		HttpRequest &operator=(HttpRequest const &rhs);

		void sendBuffer(char *buffer, ssize_t bytes);
		void setStatusCode(std::string error);
		void setRspType();
		void printBodies();
		std::string getHttpMethod() const;
		std::string getRequestUri() const;
		std::string getPath() const;
		std::string getQuery() const;
		std::string getHttpVersion() const;
		std::pair<std::string, std::string> getFullPath() const;
		int getStatusCode() const;
		int getParsingState() const;
		bool getAutoindex() const;
		std::string getRspType() const;
		std::string getQueryValue(const std::string &key) const;
		MultiBody fillBody(Headers const &header, std::string const &bodyContent);
		void fileUpload();
		Headers *getReqHeaders() const;
		ServerManager &getServ() const;
		std::pair<int, std::string> getAddrPort() const;
		std::pair<std::string, std::string> getHost() const;
		LocationConf findLocation(std::string path, std::map<std::string, LocationConf> const &loc);
		LocationConf getBlockLoc() const;
		std::string getRawBody() const;
		std::string getUriFirst() const;
		std::vector<ServerData> getServersList() const;
};

#endif
