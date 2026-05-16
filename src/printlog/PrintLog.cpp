#include "PrintLog.hpp"
#include "Graphics.hpp"
#include "LocationConf.hpp"
#include "ServerData.hpp"
#include "ServerManager.hpp"
#include "Utils.hpp"

void printServerManager(ServerManager const &servMan)
{
		std::cout << BLUE << WEBSERV_BANNER << RESET << std::endl;
		graTopLine();
		graTime("Server Started");
		graTextHeader("# of servers");
		graTextElement(servMan.getServersList().size());

		for (size_t i = 0; i < servMan.getServersList().size(); i++)
				printServersData(servMan.getServersList()[i], i);
		graBottomLine();
}

void printServersData(ServerData const &serv, size_t i)
{
		graSeparator();
		graTextHeader("Server number");
		graTextElement(intToString(i + 1));
		printServerNames(serv.getServerName());
		printServerListens(serv.getListens());
		printServerLocations(serv);
}

void printServerNames(std::vector<std::string> names)
{
		graTextHeader("Server Names");

		std::vector<std::string>::iterator it;
		std::vector<std::string>::iterator ite = names.end();

		for (it = names.begin(); it != ite; it++)
				graTextElement(*it);
}

void printServerListens(std::vector<std::pair<int, std::string> > listens)
{
		graTextHeader("Listening Sockets");

		std::vector<std::pair<int, std::string> >::iterator it;
		std::vector<std::pair<int, std::string> >::iterator ite = listens.end();

		for (it = listens.begin(); it != ite; it++)
		{
				std::ostringstream num;
				num << it->first;
				graTextElement(it->second + ":" + num.str());
		}
}

void printServerLocations(ServerData const &serv)
{
		graTextHeader("Root");
		graTextElement(serv.getRoot());
		graTextHeader("Locations");

		std::map<std::string, LocationConf>::const_iterator it;
		std::map<std::string, LocationConf>::const_iterator ite = serv.getLocations().end();

		for (it = serv.getLocations().begin(); it != ite; it++)
		{
				graTextElement(it->second.getKey());
		}
}

void printServersStatus(ServerManager &serv)
{
		graTopLine();
		graTime("Waiting for connection");
		graEmptyLine();
		graTextHeader("Requests received");
		graTextElement(intToString(serv.getReqCount()));
		graTextHeader("Responses sent");
		graTextElement(intToString(serv.getRspCount()));
		graTextHeader("Listening Sockets");

		const std::set<std::pair<int, std::string> > &listens = serv.getUniqueListens();
		std::set<std::pair<int, std::string> >::const_iterator it;
		std::set<std::pair<int, std::string> >::const_iterator ite = listens.end();

		for (it = listens.begin(); it != ite; it++)
				graTextElement(it->second + ":" + intToString(it->first));

		graBottomLine();
}

void printRequest(ServerManager &serv, int socketFd, std::string request, std::string fullPath,
				  std::string method)
{
		std::pair<int, std::string> incoming = serv.getSocketData(socketFd);
		graTopLine();
		graTime("Request received");
		graTextHeader("Request number");
		graTextElement(serv.getReqCount());
		graTextHeader("Received on");
		graTextElement(incoming.second + ":" + intToString(incoming.first));
		graTextHeader("Request method");
		graTextElement(method);
		graTextHeader("Path for request");
		graTextElement(fullPath);
		graEmptyLine();
		graTextLine("Request content below.");
		graBottomLine();

		printRaw(request);

		graTopLine();
		graTextLine("End of request");
		graBottomLine();
}

void printRaw(std::string const &text)
{
		if (text.size() > 10 &&
			((text.substr(0, 4) == "\xFF\xD8\xFF") ||							// JPEG
			 (text.substr(0, 8) == "\x89PNG\r\n\x1A\n") ||						// PNG
			 (text.substr(0, 6) == "GIF87a" || text.substr(0, 6) == "GIF89a"))) // GIF
		{
				std::cout << GREEN << "[Image content detected - " << text.size()
						  << " bytes - content not displayed]" << RESET << std::endl;
				return;
		}

		bool bits = false;
		std::cout << GREEN;
		for (size_t i = 0; i < text.size(); i++)
		{
				if (text[i] == '\r')
						std::cout << "\\r";
				else if (text[i] == '\n')
						std::cout << "\\n\n";
				else if (text[i] >= 32 && text[i] <= 126)
						std::cout << text[i];
				else
				{
						if (bits == false)
						{
								std::cout << "bit content" << std::endl;
								bits = true;
						}
						continue;
				}
		}
		std::cout << RESET << std::endl;
}

void printResponse(ServerManager &serv, std::pair<int, std::string> incoming,
				   std::string fullResponse, std::string fullPath)
{
		graTopLine();
		graTime("Response sent");
		graEmptyLine();
		graTextHeader("Response Number");
		graTextElement(intToString(serv.getRspCount()));
		graTextHeader("Request Number");
		graTextElement(intToString(serv.getReqCount()));
		graTextHeader("Socket received");
		graTextElement(incoming.second + ":" + intToString(incoming.first));
		graTextHeader("Response location");
		graTextElement(fullPath);
		graEmptyLine();
		graTextLine("Raw response below");
		graBottomLine();

		if (isBinary(fullPath))
				std::cout << fullPath << std::endl;
		else
				printRaw(fullResponse);

		graTopLine();
		graTime(intToString(fullResponse.size()) + " bytes sent");
		graBottomLine();
}

void printBoxMsg(std::string const &str)
{
		graTopLine();
		graTime(str);
		graBottomLine();
}

void printBoxError(std::string const &str)
{
		graTopLine();
		graError(str);
		graBottomLine();
}
