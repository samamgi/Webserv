#include "HttpRequest.hpp"
#include "ServerManager.hpp"
#include <cstdlib>

HttpRequest::HttpRequest(std::pair<int, std::string> incoming, ServerManager &server)
	: req_line(NULL), uri(NULL), headers(NULL), body(""), body_len(0), max_body_size(0),
	  boundary(""), boundary_flag(false), code(200), state(SKIP), body_state(0),
	  incoming(incoming), server(server), _makeNew(true)
{
		headers = new Headers();
}

HttpRequest::HttpRequest(HttpRequest const &src) : req_line(NULL), uri(NULL), headers(NULL), server(src.server) { *this = src; }

HttpRequest::~HttpRequest()
{
		if (req_line)
				delete req_line;
		if (uri)
				delete uri;
		if (headers)
				delete headers;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &rhs)
{
		if (this != &rhs)
		{
				if (this->req_line)
						delete this->req_line;
				this->req_line = rhs.req_line ? new RequestLine(*rhs.req_line) : NULL;
				if (this->uri)
						delete this->uri;
				this->uri = rhs.uri ? new Uri(*rhs.uri) : NULL;
				if (this->headers)
						delete this->headers;
				this->headers = rhs.headers ? new Headers(*rhs.headers) : NULL;
				this->host = rhs.host;
				this->_fullPath = rhs._fullPath;
				this->body = rhs.body;
				this->body_len = rhs.body_len;
				this->max_body_size = rhs.max_body_size;
				this->boundary = rhs.boundary;
				this->boundary_flag = rhs.boundary_flag;
				this->code = rhs.code;
				this->state = rhs.state;
				this->body_state = rhs.body_state;
				this->fullRequest = rhs.fullRequest;
				this->incoming = rhs.incoming;
				this->location = rhs.location;
				this->_makeNew = rhs._makeNew;
				this->_newBody = rhs._newBody;
		}
		return *this;
}

void HttpRequest::sendBuffer(char *buffer, ssize_t bytes)
{

		for (int i = 0; i < bytes; i++)
				this->fullRequest.push_back(buffer[i]);

		std::size_t found = fullRequest.find(CRLF);
		// std::cout << GREEN << "[FULLREQUEST]\n" << fullRequest << RESET << std::endl; // PRINT request
		// request

		try
		{
				while (found != std::string::npos && this->state != BODY)
				{
						std::string tmp = this->fullRequest.substr(0, found);
						this->fullRequest.erase(0, found + 2);

						if (!HttpParser::isAsciiPrintable(tmp))
								throw std::invalid_argument(E_400);

						switch (this->state)
						{
						case SKIP:
								if (tmp.empty())
										break;
								else
										this->state = REQ_LINE;
										// fallthrough
						case REQ_LINE:
								delete this->req_line;
								this->req_line = new RequestLine(HttpParser::parseRequestLine(tmp));
								this->state = HEADERS;
								break;
						case HEADERS:
								if (tmp.empty())
								{
										finalHeadersParsingRoutine();
										break;
								}
								this->headers->setHeader(tmp);
								this->state = HEADERS;
								break;
						}
						found = fullRequest.find(CRLF);
				}

				if (this->state == BODY)
				{
						if (!this->boundary.empty() && this->getRspType() != "cgi-script")
								manyBodiesRoutine(found);
						else
						{
								this->body.append(this->fullRequest);
								this->fullRequest.clear();
								if (this->body.length() >= this->body_len)
								{
										if (this->body.length() > this->body_len)
												this->body.erase(this->body_len);

										this->state = DONE;
								}
						}
				}
		} catch (const std::invalid_argument& e)
		{
				this->setStatusCode(e.what());
		}
		if (this->state == DONE)
		{
				if (getRspType() == "cgi-script")
				{
						server.getScript().runScript(*this, _cgiInterpreterPath, server);
						this->code = server.getScript().getStatusCode();
				}
				else if (getHttpMethod() == "POST")
				{
						if (blockLoc.getBodySize() >= body.size())
								fileUpload();
						else
								this->setStatusCode(E_413);
				}
		}
}

void HttpRequest::setStatusCode(std::string error)
{
		char code_str[4];

		std::strncpy(code_str, error.c_str(), 3);

		code_str[3] = '\0';

		this->state = ERR;

		this->code = std::atoi(code_str);
}

void HttpRequest::printBodies() // PRINT bodies
{
		std::cout << "Bodies size: " << _bodies.size() << std::endl;

		for (std::vector<MultiBody>::const_iterator it = _bodies.begin(); it != _bodies.end(); ++it)
		{
				std::cout << GREEN << "Body content" << std::endl;
				it->bodyHeader.printHeader();

				std::map<std::string, std::vector<std::string> >::const_iterator cd_it =
					it->bodyHeader.getHeader("content-disposition");

				if (cd_it != it->bodyHeader.getHeaderEnd())
				{
						const std::string &contDisp = cd_it->second[0];
						size_t filename_pos = contDisp.find("filename=");

						if (filename_pos != std::string::npos)
						{
								size_t start_quote = contDisp.find("\"", filename_pos);
								size_t end_quote = contDisp.find("\"", start_quote + 1);

								if (start_quote != std::string::npos &&
									end_quote != std::string::npos)
								{
										std::string filename = contDisp.substr(
											start_quote + 1, end_quote - (start_quote + 1));
										std::cout << "[FILE UPLOAD] Filename: " << filename
												  << " (Size: " << it->bodyContent.size()
												  << " bytes)" << std::endl;
								}
								else
								{
										std::cout << "[FILE UPLOAD] Binary "
													 "content (Size: "
												  << it->bodyContent.size() << " bytes)"
												  << std::endl;
								}
						}
						else
								std::cout << it->bodyContent << std::endl;
				}
				else
						std::cout << it->bodyContent << std::endl;

				std::cout << RESET << std::endl;
		}
}

LocationConf const *HttpParser::findLocation(std::string path,
											 std::map<std::string, LocationConf> const &loc)
{
		std::map<std::string, LocationConf>::const_iterator it;
		std::map<std::string, LocationConf>::const_iterator tmp = loc.end();

		for (it = loc.begin(); it != loc.end(); it++)
		{
				if (path.find(it->first) == 0)
				{
						if (tmp == loc.end() || tmp->first.length() < it->first.length())
								tmp = it;
				}
		}
		if (tmp == loc.end())
				return NULL;
		return &tmp->second;
}

void HttpRequest::finalHeadersParsingRoutine()
{
		if (this->headers->getHeader("cookie") != this->headers->getHeaderEnd())
				this->headers->setManyValuesHeader("cookie");

		if (this->headers->getHeader("content-type") != this->headers->getHeaderEnd())
				this->headers->setManyValuesHeader("content-type");
		// this->headers->printHeader();

		ServerData serv =
			HttpParser::checkIfServerExist(this->server.getServersList(), this->incoming);

		checkHost(this->headers->getHeader("host"), serv);

		delete this->uri;
		this->uri = new Uri(req_line->getReqTarget(), this->host.first);

		setFullPath(serv);

		setLocation(serv.getLocations(), this->_fullPath.second);

		blockLoc = findLocation(this->_fullPath.second, serv.getLocations());
		this->max_body_size = blockLoc.getBodySize();
		HttpParser::checkIfPathExist(this->_fullPath, blockLoc, this->getHttpMethod());

		if (this->headers->getHeader("content-type") != this->headers->getHeaderEnd())

		{
				this->boundary = HttpParser::parseContentTypeBoundary(
					this->headers->getHeaderValue("content-type"));
				if (!this->boundary.empty())
				{
						this->state = BODY;
						this->body_state = BOUNDARY;
						this->boundary_flag = true;
				}
		}
		if (this->headers->getHeader("content-length") != this->headers->getHeaderEnd())
		{
				this->body_len = HttpParser::parseContentLengthHeader(
					this->headers->getHeaderOnlyOneValue("content-length", 0), this->max_body_size);
				this->state = BODY;
		}
		if (this->headers->getHeader("content-type") != this->headers->getHeaderEnd() ||
			this->headers->getHeader("content-length") != this->headers->getHeaderEnd())
				return;
		this->state = DONE;
}

void HttpRequest::manyBodiesRoutine(std::size_t found)
{
		while (found != std::string::npos && this->body_state != BODY2)
		{
				std::string tmp = this->fullRequest.substr(0, found);
				this->fullRequest.erase(0, found + 2);
				if (this->_makeNew)
				{
						this->_newBody = MultiBody();
						this->_makeNew = false;
				}
				switch (this->body_state)
				{
				case BOUNDARY:
						if (this->boundary_flag == true && tmp != "--" + this->boundary)
						{
								this->boundary_flag = false;
								throw std::invalid_argument(E_400);
						}
						this->boundary_flag = false;
						this->body_state = HEADERS2;
						break;
				case HEADERS2:
						if (tmp.empty())
						{
								this->body_state = BODY2;
								break;
						}
						this->_newBody.bodyHeader.setHeader(tmp);
						this->body_state = HEADERS2;
						break;
				}

				found = this->fullRequest.find(CRLF);
		}

		this->body = this->fullRequest;

		std::size_t f = this->body.find("\r\n--" + this->boundary + "--\r\n");
		if (f != std::string::npos)
		{
				this->body = this->body.erase(f, this->body.length());
				this->_newBody.bodyContent = this->body;
				this->_bodies.push_back(this->_newBody);
				this->_makeNew = true;
				this->state = DONE;
		}
		std::size_t f2 = this->body.find("\r\n--" + this->boundary + "\r\n");
		if (f2 != std::string::npos)
		{
				this->body = this->body.erase(f2, this->body.length());
				this->_newBody.bodyContent = this->body;
				this->_bodies.push_back(this->_newBody);
				this->_makeNew = true;
				this->fullRequest = this->fullRequest.erase(0, f2 + 6);
				this->body_state = BOUNDARY;
		}
}

void HttpRequest::setLocation(std::map<std::string, LocationConf> const &locations,
							  std::string const &path)
{
		std::map<std::string, LocationConf>::const_iterator best_match = locations.end();

		for (std::map<std::string, LocationConf>::const_iterator it = locations.begin();
			 it != locations.end(); ++it)
				if (path.find(it->first) == 0)
						if (best_match == locations.end() ||
							it->first.length() > best_match->first.length())
								best_match = it;

		if (best_match == locations.end())
				throw std::invalid_argument(E_404);

		this->location = best_match->first;
		setRspType();
		if (_rspType == "cgi-script")
		{
				std::string requestFile = _fullPath.second;
				size_t dotPos = requestFile.find_last_of('.');
				if (dotPos != std::string::npos)
				{
						std::string extension = requestFile.substr(dotPos);

						const std::map<std::string, std::string> &cgiPassMap =
							best_match->second.getCgiPass();
						std::map<std::string, std::string>::const_iterator cgi_it =
							cgiPassMap.find(extension);

						if (cgi_it != cgiPassMap.end())
						{
								this->_cgiInterpreterPath = cgi_it->second;
								std::cout << GREEN << "Found CGI interpreter for " << extension
										  << ": " << _cgiInterpreterPath << RESET << std::endl;
						}
						else
						{
								throw std::invalid_argument(E_501);
						}
				}
		}
}

void HttpRequest::setRspType()
{
		std::string location = _fullPath.first + _fullPath.second;

		if (isFolder(location))
		{
				_rspType = "text/html";
				return;
		}

		std::string extension;
		size_t dotPos = location.find_last_of('.');
		if (dotPos != std::string::npos)
				extension = location.substr(dotPos);

		if (extension == ".html" || extension == ".htm")
				_rspType = "text/html";
		else if (extension == ".jpg" || extension == ".jpeg")
				_rspType = "image/jpeg";
		else if (extension == ".png")
				_rspType = "image/png";
		else if (extension == ".gif")
				_rspType = "image/gif";
		else if (extension == ".css")
				_rspType = "text/css";
		else if (extension == ".py" || extension == ".php")
				_rspType = "cgi-script";
		else
				_rspType = "application/octet-stream";
}

void HttpRequest::checkHost(std::map<std::string, std::vector<std::string> >::const_iterator it,
							ServerData &serv)
{
		if (it == this->headers->getHeaderEnd())
				throw std::invalid_argument(E_400);

		if (this->headers->getHeaderOnlyOneValue("host", 0).empty())
				throw std::invalid_argument(E_400);

		this->host = HttpParser::parseHost(this->headers->getHeaderOnlyOneValue("host", 0));

		if (!HttpParser::checkIfHostNameExistInServer(this->host.first, serv.getServerName()))
				throw std::invalid_argument(E_400);
}

std::string HttpRequest::getHttpMethod() const { return this->req_line->getMethod(); }

std::string HttpRequest::getRequestUri() const { return this->uri->getUri(); }

std::string HttpRequest::getPath() const { return this->uri->getPath(); }

void HttpRequest::setFullPath(ServerData const &serv)
{
		_fullPath = std::make_pair<std::string, std::string>(serv.getRoot(), uri->getPath());
}
std::pair<std::string, std::string> HttpRequest::getFullPath() const { return _fullPath; }

std::string HttpRequest::getQuery() const { return this->uri->getQuery(); }

std::string HttpRequest::getHttpVersion() const { return this->req_line->getVersion(); }

int HttpRequest::getStatusCode() const { return this->code; }

int HttpRequest::getParsingState() const { return this->state; }

std::string HttpRequest::getRspType() const { return _rspType; }

ServerManager &HttpRequest::getServ() const { return this->server; }

void HttpRequest::fileUpload()
{

		// printBodies(); // PRINT bodies
		std::map<std::string, std::string> formData;

		for (std::vector<MultiBody>::const_iterator it = _bodies.begin(); it != _bodies.end(); ++it)
		{
				std::map<std::string, std::vector<std::string> >::const_iterator cd_it =
					it->bodyHeader.getHeader("content-disposition");
				if (cd_it == it->bodyHeader.getHeaderEnd())
				{
						continue;
				}

				const std::string &contDisp = cd_it->second[0];

				size_t filename_pos = contDisp.find("filename=");

				if (filename_pos != std::string::npos)
				{
						size_t start_quote = contDisp.find("\"", filename_pos);
						size_t end_quote = contDisp.find("\"", start_quote + 1);
						if (start_quote == std::string::npos || end_quote == std::string::npos)
								continue;

						std::string nameOriginal =
							contDisp.substr(start_quote + 1, end_quote - (start_quote + 1));
						size_t lastSlash = nameOriginal.find_last_of("/");
						std::string nameClean = (lastSlash == std::string::npos)
													? nameOriginal
													: nameOriginal.substr(lastSlash + 1);

						if (nameClean.empty())
								continue;

						std::string uploadPath = _fullPath.first + _fullPath.second + nameClean;

						std::ofstream fileUp(uploadPath.c_str(), std::ios::binary);
						if (fileUp.is_open())
						{
								fileUp << it->bodyContent;
								fileUp.close();
								std::cout << GREEN << "SUCCESS: Saved file to " << uploadPath
										  << RESET << std::endl;
						}
						else
								printBoxError("Error opening file");
				}
				else
				{
						size_t name_pos = contDisp.find("name=");
						if (name_pos != std::string::npos)
						{
								size_t start_quote = contDisp.find("\"", name_pos);
								size_t end_quote = contDisp.find("\"", start_quote + 1);
								if (start_quote != std::string::npos &&
									end_quote != std::string::npos)
								{
										std::string field_name = contDisp.substr(
											start_quote + 1, end_quote - (start_quote + 1));
										formData[field_name] = it->bodyContent;
								}
						}
				}
		}

		std::cout << RED << "[Form Data Received]" << RESET << std::endl;
		for (std::map<std::string, std::string>::const_iterator map_it = formData.begin();
			 map_it != formData.end(); ++map_it)
		{
				std::cout << map_it->first << ": " << map_it->second << std::endl;
		}
		this->code = 201;
}

Headers *HttpRequest::getReqHeaders() const { return this->headers; }

std::pair<int, std::string> HttpRequest::getAddrPort() const { return incoming; }

std::pair<std::string, std::string> HttpRequest::getHost() const { return host; }

std::string HttpRequest::getRawBody() const { return this->body; }

LocationConf HttpRequest::findLocation(std::string path,
									   std::map<std::string, LocationConf> const &loc)
{
		std::map<std::string, LocationConf>::const_iterator it;
		std::map<std::string, LocationConf>::const_iterator tmp = loc.end();

		if (path[path.size() - 1] != '/')
				path = path + '/';
		for (it = loc.begin(); it != loc.end(); it++)
		{
				if (path.find(it->first) == 0)
				{
						if (tmp == loc.end() || tmp->first.length() < it->first.length())
								tmp = it;
				}
		}
		if (tmp == loc.end())
				return LocationConf();
		return tmp->second;
}

LocationConf HttpRequest::getBlockLoc() const { return blockLoc; }

std::string HttpRequest::getUriFirst() const
{
		if (getHost().second.empty())
				return uri->getScheme() + "://" + uri->getAuthority();
		return uri->getScheme() + "://" + uri->getAuthority() + ":" + getHost().second;
}

std::vector<ServerData> HttpRequest::getServersList() const
{
		return this->server.getServersList();
}
