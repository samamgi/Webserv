#include "HttpParser.hpp"
#include "RequestLine.hpp"
#include "ServerData.hpp"
#include "Utils.hpp"

std::vector<std::string> HttpParser::split(std::string const &str, char const delimiter)
{

		std::vector<std::string> tokens;
		std::string t;

		int i = 0;
		int init = 0;
		int count = 0;

		std::string::const_iterator it;
		std::string::const_iterator ite = str.end();

		for (it = str.begin(); it != ite; ++it)
		{
				i++;
				count++;
				if (*it == delimiter || it == ite - 1)
				{
						init = count - i;
						if (it == ite - 1 && *it != delimiter)
								i++;
						t = str.substr(init, i - 1);
						if (!t.empty())
								tokens.push_back(t);
						i = 0;
				}
		}
		return tokens;
}

std::vector<std::string> HttpParser::isspaceSplit(std::string const &str)
{

		std::vector<std::string> tokens;
		std::string t;

		int i = 0;
		int init = 0;
		int count = 0;

		std::string::const_iterator it;
		std::string::const_iterator ite = str.end();

		for (it = str.begin(); it != ite; ++it)
		{
				i++;
				count++;
				if (std::isspace(*it) || it == ite - 1)
				{
						init = count - i;
						if (it == ite - 1 && *it != std::isspace(*it))
								i++;
						t = str.substr(init, i - 1);
						if (!t.empty())
								tokens.push_back(t);
						i = 0;
				}
		}
		return tokens;
}

bool HttpParser::isAsciiPrintable(std::string const &str)
{
		std::string::const_iterator it, ite = str.end();

		for (it = str.begin(); it != ite; ++it)
		{
				if (*it == '\t')
						continue;
				if (*it > 126 || *it < 32)
						return false;
		}
		return true;
}

bool HttpParser::isUnreservedForUri(char c)
{
		if (c == '-' || c == '_' || c == '.' || c == '~' || isalnum(c))
				return true;
		return false;
}

bool HttpParser::isReservedForUri(char c)
{
		if (c == ':' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']' || c == '@')
				return true;

		if (c == '!' || c == '$' || c == '&' || c == '\\' || c == '(' || c == ')' || c == '*' ||
			c == '+' || c == ',' || c == ';' || c == '=')
				return true;

		return false;
}

bool HttpParser::isTokenChar(char c)
{
		const std::string specials = "!#$%&'*+-.^_`|~";

		return std::isalnum(static_cast<unsigned char>(c)) || specials.find(c) != std::string::npos;
}

bool HttpParser::isHexChar(char c)
{
		return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

bool HttpParser::isDNS(std::string s)
{

		std::string::const_iterator it, ite = s.end();

		for (it = s.begin(); it != ite; ++it)
				if (!isalnum(*it) && *it != '-' && *it != '.')
						return false;

		return true;
}

std::string HttpParser::toLower(std::string const &str)
{

		std::string result = str;
		std::string::iterator it, ite = result.end();

		for (it = result.begin(); it != ite; ++it)
				*it = std::tolower(static_cast<unsigned char>(*it));

		return result;
}

std::string HttpParser::trimSpaceAndTab(std::string &str)
{
		std::string::reverse_iterator rit, rite = str.rend();

		for (rit = str.rbegin(); rit != rite; ++rit)
		{
				if (*rit == ' ' || *rit == '\t')
						str.erase(str.size() - 1);
				else
						break;
		}

		while (!str.empty() && (str[0] == ' ' || str[0] == '\t'))
				str.erase(0, 1);

		return str;
}

RequestLine HttpParser::parseRequestLine(std::string const &line)
{

		std::string::const_iterator s_it, s_ite = line.end();
		int spaces = 0;

		for (s_it = line.begin(); s_it != s_ite; ++s_it)
				if (*s_it == ' ')
						spaces++;
		if (spaces != 2)
				throw std::invalid_argument(E_400);

		std::vector<std::string> tokens = split(line, ' ');

		if (tokens.size() != 3)
				throw std::invalid_argument(E_400);

		if (tokens[1].length() > 8000)
				throw std::invalid_argument(E_414);

		if (tokens[2] != "HTTP/1.1")
				throw std::invalid_argument(E_400);

		return RequestLine(tokens);
}

void HttpParser::parseReqTarget(std::string &uri)
{
		std::string::iterator it, ite = uri.end();

		if (uri.at(0) && uri.at(0) != '/')
				throw std::invalid_argument(E_400);

		for (it = uri.begin(); it != ite; ++it)
		{
				if (!isUnreservedForUri(*it) && !isReservedForUri(*it) && *it != '%')
						throw std::invalid_argument(E_400);
				if (*it == '%')
				{
						if (it + 1 == ite || it + 2 == ite)
								throw std::invalid_argument(E_400);
						if (!isHexChar(*(it + 1)) || !isHexChar(*(it + 2)))
								throw std::invalid_argument(E_400);
						it++;
				}
		}
}

std::string HttpParser::parsePath(std::string const &uri)
{
		std::size_t foundQ = uri.find("?");
		std::size_t foundF = uri.find("#");

		if (foundQ != std::string::npos)
				return uri.substr(0, foundQ);
		if (foundF != std::string::npos)
				return uri.substr(0, foundF);

		return uri;
}

std::string HttpParser::parseQuery(std::string const &uri)
{
		std::size_t foundQ = uri.find("?");
		std::string tmp;

		if (foundQ != std::string::npos)
				tmp = uri.substr(foundQ + 1, uri.length());
		else
				return "";

		std::size_t foundF = tmp.find("#");

		if (foundF != std::string::npos)
				return tmp.substr(0, foundF);
		else
				return tmp;
}

std::string HttpParser::parseFragment(std::string const &uri)
{

		std::size_t found = uri.find("#");

		if (found != std::string::npos)
				return uri.substr(found + 1, uri.length());
		else
				return "";
}

const std::string HttpParser::valid_method[] = {"GET",	"POST",	   "PUT",	"DELETE",
												"HEAD", "OPTIONS", "TRACE", "CONNECT"};
const int HttpParser::valid_method_count = 8;

bool HttpParser::notImplementedMethod(std::string const &method)
{
		for (int i = 0; i < valid_method_count; i++)
				if (method == valid_method[i])
						return true;
		return false;
}

ServerData const &HttpParser::checkIfServerExist(std::vector<ServerData> const &servers,
												 std::pair<int, std::string> incoming)
{
		std::vector<ServerData>::const_iterator it, ite = servers.end();
		std::vector<std::pair<int, std::string> >::const_iterator it_listen, ite_listen;

		for (it = servers.begin(); it != ite; ++it)
		{
				const std::vector<std::pair<int, std::string> > &listens = it->getListens();
				ite_listen = listens.end();
				for (it_listen = listens.begin(); it_listen != ite_listen; ++it_listen)
				{
						if (*it_listen == incoming)
								return *it;
				}
		}
		throw std::invalid_argument(E_421);
}

void HttpParser::checkIfPathExist(std::pair<std::string, std::string> const &path,
								  LocationConf &blockLoc, std::string const &method)
{

		std::string full(path.first + path.second);

		if (!blockLoc.getReturnDirective().empty())
		{
				if (blockLoc.getReturnDirective()[0] == "301" ||
					blockLoc.getReturnDirective()[0] == "302" ||
					blockLoc.getReturnDirective()[0] == "307" ||
					blockLoc.getReturnDirective()[0] == "308")
				{
						throw std::invalid_argument(blockLoc.getReturnDirective()[0]);
				}
				else
						throw std::invalid_argument(E_500);
		}

		if (path.second.empty())
				full = path.first;

		if (access(full.c_str(), F_OK) == -1)
				throw std::invalid_argument(E_404);

		const std::vector<std::string> &allowedMethods = blockLoc.getAllowedMethods();
		if (std::find(allowedMethods.begin(), allowedMethods.end(), method) == allowedMethods.end())
				throw std::invalid_argument(E_405);

		if (isBinary(full) && method != "DELETE")
		{
				std::ifstream file(full.c_str(), std::ios::binary);
				if (!file.is_open())
						throw std::invalid_argument(E_403);
				file.close();
		}
		else if (isFolder(full))
		{
				if (method == "POST")
				{
						if (access(full.c_str(), W_OK) != 0)
								throw std::invalid_argument(E_403);
						return;
				}
				if (method == "DELETE")
						throw std::invalid_argument(E_403);

				DIR *dir = opendir(full.c_str());
				if (!dir)
						throw std::invalid_argument(E_403);
				if (path.second[path.second.size() - 1] != '/')
						full = full + '/';
				std::string index = full + "index.html";
				if (access(index.c_str(), F_OK) == 0)
				{
						if (access(index.c_str(), R_OK) != 0)
						{
								closedir(dir);
								throw std::invalid_argument(E_403);
						}
				}
				else
				{
						if (access(full.c_str(), R_OK) != 0 || blockLoc.getAutoindex() == false)
						{
								std::cout << ERROR << blockLoc.getAutoindex() << std::endl;
								closedir(dir);
								throw std::invalid_argument(E_403);
						}
				}
				closedir(dir);
		}
		else
		{
				if (method == "DELETE")
				{
						if (std::remove(full.c_str()) != 0)
								throw std::invalid_argument(E_403);
						throw std::invalid_argument(E_204);
				}
				std::ifstream page(full.c_str());
				if (!page.is_open())
						throw std::invalid_argument(E_403);
				page.close();
		}
}

void HttpParser::notAllowedMethod(std::map<std::string, LocationConf>::iterator location,
								  std::vector<std::string> const &allowed_serv,
								  std::string const &method)
{

		std::vector<std::string> const &allowed_loc = location->second.getAllowedMethods();

		if (!allowed_loc.empty())
		{
				for (std::size_t i = 0; i < allowed_loc.size(); i++)
						if (allowed_loc[i] == method)
								return;
				throw std::invalid_argument(E_405);
		}

		if (!allowed_serv.empty())
		{
				for (std::size_t i = 0; i < allowed_serv.size(); i++)
						if (allowed_serv[i] == method)
								return;
				throw std::invalid_argument(E_405);
		}
}

std::pair<std::string, std::string> HttpParser::parseHost(std::string const &str)
{
		std::string tmp(str);
		std::string port = "";
		std::string name = trimSpaceAndTab(tmp);
		std::size_t found = tmp.find(':');

		if (found != std::string::npos)
		{
				name = tmp.substr(0, found);
				port = tmp.substr(found + 1, tmp.size() - found);
				if (port.empty())
						throw std::invalid_argument(E_400);
				if (!isInt(port))
						throw std::invalid_argument(E_400);
				if (port[0] == '0')
						throw std::invalid_argument(E_400);
		}

		if (!isDNS(name))
				throw std::invalid_argument(E_400);
		if (!checkLabel(name))
				throw std::invalid_argument(E_400);

		return std::make_pair(name, port);
}

bool HttpParser::checkIfHostNameExistInServer(std::string &host_name,
											  std::vector<std::string> const &serv_name)
{

		if (host_name == "localhost" || host_name == "127.0.0.1")
				return true;

		std::vector<std::string>::const_iterator it, ite = serv_name.end();

		for (it = serv_name.begin(); it != ite; ++it)
				if (host_name == *it)
						return true;
		return false;
}

const std::string HttpParser::one_header[] = {"host", "content-type", "content-length",
											  "content-disposition", "cookie"};
const int HttpParser::one_h_count = 5;
const std::string HttpParser::many_header[] = {"accept", "accept-encoding", "accept-lenguage",
											   "connection", "cache-control"};
const int HttpParser::many_h_count = 5;

std::pair<std::string, std::string> HttpParser::parseHeaderSyntaxis(std::string h)
{

		std::size_t found = h.find(":");

		if (found == std::string::npos)
				throw std::invalid_argument(E_400);
		std::string name = h.substr(0, found);
		std::string::iterator it_n, ite_n = name.end();

		for (it_n = name.begin(); it_n != ite_n; ++it_n)
				if (!HttpParser::isTokenChar(*it_n))
						throw std::invalid_argument(E_400);

		return make_pair(toLower(name), h.substr(found + 1, h.length() - found + 1));
}

bool HttpParser::oneValueHeader(std::string name)
{
		for (int i = 0; i < one_h_count; i++)
				if (name == one_header[i])
						return true;
		return false;
}

bool HttpParser::manyValuesHeader(std::string name)
{
		for (int i = 0; i < many_h_count; i++)
				if (name == many_header[i])
						return true;
		return false;
}

bool HttpParser::recognizeHeaderName(std::string name)
{
		if (oneValueHeader(name))
				return true;
		if (manyValuesHeader(name))
				return true;
		return false;
}

std::vector<std::string> HttpParser::pushValues(std::string n, std::string v)
{
		std::vector<std::string> values;

		if (oneValueHeader(n))
		{
				values.push_back(trimSpaceAndTab(v));
				return values;
		}

		values = split(v, ',');

		std::vector<std::string>::iterator it, ite = values.end();

		for (it = values.begin(); it != ite; ++it)
				trimSpaceAndTab(*it);

		return values;
}

void HttpParser::pushMoreValues(std::map<std::string, std::vector<std::string> >::iterator h,
								std::string v)
{

		if (oneValueHeader(h->first))
		{
				if (h->second.at(0) == trimSpaceAndTab(v))
						return;
				throw std::invalid_argument(E_400);
		}

		std::vector<std::string> more_values = split(v, ',');
		std::vector<std::string>::iterator it, ite = more_values.end();

		for (it = more_values.begin(); it != ite; ++it)
				trimSpaceAndTab(*it);

		h->second.insert(h->second.end(), more_values.begin(), more_values.end());
}

int HttpParser::parseContentLengthHeader(std::string const &v, std::size_t body_max)
{
		if (v.empty())
				throw std::invalid_argument(E_400);
		if (v.at(0) == 0 && v.length() == 1)
				return 0;
		std::size_t tmp = atoi(v.c_str());
		if (tmp <= 0)
				throw std::invalid_argument(E_400);
		if (tmp > body_max)
				throw std::invalid_argument(E_413);
		return tmp;
}

std::string HttpParser::parseContentTypeBoundary(std::vector<std::string> const &v)
{
		if (v.at(0) != "multipart/form-data")
				return "";

		if (v.size() < 2)
				throw std::invalid_argument(E_400);

		std::size_t found = v.at(1).find("boundary=");

		if (found == std::string::npos)
				throw std::invalid_argument(E_400);

		std::string boundary = v[1].substr(9, v[1].length());

		if (boundary.length() > 70)
				throw std::invalid_argument(E_400);

		std::string::const_iterator it, ite = boundary.end();

		for (it = boundary.begin(); it != ite; ++it)
				if (*it > 126 || *it < 33)
						throw std::invalid_argument(E_400);

		return boundary;
}
