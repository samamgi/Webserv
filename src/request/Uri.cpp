#include "Uri.hpp"
#include "HttpParser.hpp"
#include <iostream>
#include <string>

Uri::Uri(std::string const &req_target, std::string const &host) : scheme("http"), authority(host)
{

		path = HttpParser::parsePath(req_target);
		query = HttpParser::parseQuery(req_target);
		fragment = HttpParser::parseFragment(req_target);

		uri = scheme + "://" + authority + path;
		if (!query.empty())
				uri += "?" + query;
		if (!fragment.empty())
				uri += "#" + fragment;
}

Uri::Uri(Uri const &src) { *this = src; }

Uri::~Uri() {}

Uri &Uri::operator=(Uri const &rhs)
{
		if (this != &rhs)
		{
				this->uri = rhs.uri;
				this->scheme = rhs.scheme;
				this->authority = rhs.authority;
				this->path = rhs.path;
				this->query = rhs.query;
				this->fragment = rhs.fragment;
		}
		return *this;
}

std::string Uri::getUri() const { return this->uri; }

std::string Uri::getScheme() const { return this->scheme; }

std::string Uri::getAuthority() const { return this->authority; }

std::string Uri::getPath() const { return this->path; }

std::string Uri::getQuery() const { return this->query; }

std::string Uri::getFragment() const { return this->fragment; }
