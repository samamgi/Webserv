#ifndef PARSINGCONF_HPP
#define PARSINGCONF_HPP

#include "ABlockBase.hpp"
#include "LocationConf.hpp"
#include "Utils.hpp"

#define DEFAULTCONF "configuration_files/test01.conf"
class ServerData;

class ParsingConf
{
	public:
		ParsingConf();
		~ParsingConf();

		std::vector<ServerData> servers;

		std::vector<ServerData> &getServers();

		void checkLocation(std::string &line);
		void checkSemicolon(std::string &line);
		void checkParenthesis(std::string &line);
		static void print_tokens(std::vector<std::string> &buffer);
		void print_structure();
		void checkStructure(std::vector<std::string> &buffer);
		void doParsing(std::string line, std::vector<std::string> &buffer);
		size_t fillServers(std::vector<std::string> &buffer, size_t &i,
						   std::vector<ABlockBase *> &blocks,
						   std::vector<ServerData>::iterator &itServer);
		size_t fillLocations(std::vector<std::string> &buffer, size_t &i,
							 std::vector<ABlockBase *> &blocks,
							 std::vector<ServerData>::iterator &itServer,
							 std::map<std::string, LocationConf>::iterator &itLocation);
		void fillStructs(std::vector<std::string> &buffer);

	private:
		ParsingConf(ParsingConf const &other);
		ParsingConf &operator=(ParsingConf const &other);
};

#endif
