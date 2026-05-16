#include "Headers.hpp"
#include "HttpParser.hpp"
#include <iostream>
#include <string>

Headers::Headers() {}

Headers::Headers(Headers const &src) { *this = src; }

Headers::~Headers() {}

Headers &Headers::operator=(Headers const &rhs)
{
		if (this != &rhs)
				this->header = rhs.header;
		return *this;
}

void Headers::setHeader(std::string &str)
{
		std::pair<std::string, std::string> tmp = HttpParser::parseHeaderSyntaxis(str);

		if (!HttpParser::recognizeHeaderName(tmp.first))
				return;

		std::map<std::string, std::vector<std::string> >::iterator found =
			this->header.find(tmp.first);

		if (found != this->header.end())
		{
				HttpParser::pushMoreValues(found, tmp.second);
				return;
		}

		this->header[tmp.first] = HttpParser::pushValues(tmp.first, tmp.second);
}

std::map<std::string, std::vector<std::string> >::const_iterator
Headers::getHeader(std::string const &name) const
{
		std::map<std::string, std::vector<std::string> >::const_iterator it =
			this->header.find(name);

		return it;
}

std::map<std::string, std::vector<std::string> >::const_iterator Headers::getHeaderBegin() const
{
		return this->header.begin();
}

std::map<std::string, std::vector<std::string> >::const_iterator Headers::getHeaderEnd() const
{
		return this->header.end();
}

std::vector<std::string> &Headers::getHeaderValue(std::string const &name)
{
		return this->header[name];
}

std::vector<std::string> Headers::getHeaderValue(std::string const &name) const
{
		std::map<std::string, std::vector<std::string> >::const_iterator it =
			this->header.find(name);

		if (it == this->header.end())
				throw std::invalid_argument("Error: header dont exist!");

		return it->second;
}

std::string Headers::getHeaderOnlyOneValue(std::string const &name, int index) const
{
		std::map<std::string, std::vector<std::string> >::const_iterator it =
			this->header.find(name);

		if (it == this->header.end())
				throw std::invalid_argument("Error: header dont exist!");
		return this->header.at(name).at(index);
}

void Headers::printHeader() const
{
		std::map<std::string, std::vector<std::string> >::const_iterator it,
			ite = this->header.end();
		std::vector<std::string>::const_iterator it_v, ite_v;

		for (it = this->header.begin(); it != ite; ++it)
		{
				std::cout << "key: " << it->first << "\nvalues: ";
				ite_v = it->second.end();
				for (it_v = it->second.begin(); it_v != ite_v; ++it_v)
						std::cout << *it_v << " ";
				std::cout << std::endl;
		}
}

void Headers::setManyValuesHeader(std::string name)
{
		if (getHeader(name) == this->header.end())
				return;

		std::vector<std::string> more_values =
			HttpParser::split(getHeaderOnlyOneValue(name, 0), ';');
		std::vector<std::string>::iterator it, ite = more_values.end();

		for (it = more_values.begin(); it != ite; ++it)
				HttpParser::trimSpaceAndTab(*it);

		std::vector<std::string> &cookie = this->getHeaderValue(name);

		cookie.insert(cookie.begin(), more_values.begin(), more_values.end());
		cookie.pop_back();
}

size_t Headers::getHeaderSize() const { return header.size(); }