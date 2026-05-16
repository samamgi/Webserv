#include "HeadRsp.hpp"
#include "Response.hpp"

HeadRsp::HeadRsp(Response &response) : _response(&response)
{
		setContentType();
		setProtocol();
		setRspStatusCode();
		setContentLength();
		setConnectionType();
		setCacheControl();
		setCookieString();
		buildHeader();
}

HeadRsp::~HeadRsp() {};

void HeadRsp::setContentType() { _contentType = "Content-Type: " + _response->getType() + HEADNL; }

void HeadRsp::setCookieString()
{
		std::string cookie = _response->getCookie();
		if (!cookie.empty())
				_cookieString = "Set-Cookie:" + cookie + std::string(HEADNL);
		else
				_cookieString = "";
}

void HeadRsp::setProtocol() { _protocol = "HTTP/1.1"; }

void HeadRsp::setRspStatusCode()
{
		int code = _response->getStatusCode();
		std::stringstream ss;
		ss << code;

		const std::map<int, std::string> &statusCodeMap = getStatusCodeMap();
		std::map<int, std::string>::const_iterator it = statusCodeMap.find(code);
		if (it != statusCodeMap.end())
		{
				_statusCode = ss.str() + " " + it->second;
		}
		else
		{
				_statusCode = ss.str() + " Unknown Status";
		}
}

void HeadRsp::setConnectionType() { _connectionType = "Connection: close" + std::string(HEADNL); }

void HeadRsp::setContentLength()
{
		_contentLength = "Content-Length: " + _response->getLength() + HEADNL;
}

void HeadRsp::setCacheControl()
{
		std::string cache;

		if (_contentType == "text/html")
				cache = "public, max-age=3600";
		else if (_contentType == "image/jpg" || _contentType == "image/png" ||
				 _contentType == "image/gif" || _contentType == "text/javascript")
				cache = "public, max-age=300, must-revalidate";
		else
				cache = "no-cache, no-store, must-revalidate";

		_cacheControl = "Cache-Control: " + cache + HEADNL;
}

void HeadRsp::buildHeader()
{
		_header = _protocol + " " + _statusCode + HEADNL;
		_header += "Server: webserv/1.0" + std::string(HEADNL);
		_header += "Date: " + getHttpDate() + std::string(HEADNL);
		_header += _connectionType;

		const std::map<std::string, std::string> &cgiHeaders = _response->getCgiHeaders();

		if (!cgiHeaders.empty())
		{
				for (std::map<std::string, std::string>::const_iterator it = cgiHeaders.begin();
					 it != cgiHeaders.end(); ++it)
				{
						if (it->first != "Status" && it->first != "Content-Type")
						{
								_header += it->first + ": " + it->second + HEADNL;
						}
				}
				_header += _contentType;
		}
		else
		{
				_header += _contentType;
				_header += _cacheControl;
		}
		if (!_response->getCookie().empty())
			_header += _cookieString;
		_header += _contentLength;
		_header += HEADNL;
}

std::string HeadRsp::getHeader() { return _header; }