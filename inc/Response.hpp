#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "HeadRsp.hpp"
#include "LocationConf.hpp"
#include "Utils.hpp"

class HttpRequest;

class Response
{
	private:
		const HttpRequest *_request;
		int _clientFd;
		int _statusCode;
		std::string _response;
		std::string _location;
		std::string _method;
		std::ostringstream _output;
		std::string _contentType;
		std::string _contentLength;
		std::string _cookieToSet;
		std::map<std::string, std::string> _cgiHeaders;
		LocationConf _blockLoc;

		Response &operator=(Response const &copy);
		Response();

		void errorRoutine(std::string &content, std::pair<int, std::string> incoming);

	public:
		Response(HttpRequest const &request);
		Response(Response const &copy);
		~Response();

		std::string prepFile();
		void setClientFd(int _clientFd);
		void setResponse(std::string response);
		void setContent(std::pair<std::string, std::string> fullPath, std::string method);
		void prepResponse(std::pair<int, std::string> incoming);
		std::string getType();
		std::string getLength();
		std::string getResponse();
		void setCookie(const std::string &cookieValue);
		void printRawResponse();
		std::string doAutoindex(std::string str, DIR *dir);
		void doHtmlAutoindex(std::string &uri, std::ostringstream &html);
		std::map<std::string, std::string> getCgiHeaders() const;
		int getStatusCode() const;
		std::string getCookie() const;
};

#endif
