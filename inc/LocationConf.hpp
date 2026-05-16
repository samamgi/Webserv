#ifndef LOCATIONCONF_HPP
#define LOCATIONCONF_HPP

#include "ABlockBase.hpp"
#include "Utils.hpp"

class ServerData;

class LocationConf : public ABlockBase
{
	public:
		LocationConf();
		LocationConf(ServerData const &S);
		~LocationConf();
		LocationConf(LocationConf const &other);
		LocationConf &operator=(LocationConf const &other);

		virtual std::string const &getRoot() const;
		bool getAutoindex() const;
		std::string const &getKey() const;
		void setKey(std::string const value);
		size_t fillAutoIndex(std::vector<std::string> &buffer, size_t i);

	private:
		std::string _key;
		bool _autoindex;
};

#endif
