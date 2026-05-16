#ifndef HEADRSP_HPP
#define HEADRSP_HPP

#include "Utils.hpp"

#define HEADNL "\r\n"

class Response;

class HeadRsp
{
	private:
		Response *_response;
		std::string _header;
		std::string _statusCode;
		std::string _contentType;
		std::string _protocol;
		std::string _contentLength;
		std::string _connectionType;
		std::string _cacheControl;
		std::string _cookieString;

		HeadRsp &operator=(HeadRsp const &copy);
		HeadRsp();
		HeadRsp(HeadRsp const &copy);

	public:
		HeadRsp(Response &response);
		~HeadRsp();

		void setContentType();
		void setProtocol();
		void setRspStatusCode();
		void setContentLength();
		void setConnectionType();
		void setCacheControl();
		void buildHeader();
		void setCookieString();
		std::string getHeader();
};

#endif
