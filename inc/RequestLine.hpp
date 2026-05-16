#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP

#include <string>
#include <vector>

class RequestLine
{
	private:
		std::string method;
		std::string req_target;
		std::string version;

		RequestLine();

	public:
		RequestLine(std::vector<std::string> const &line);
		RequestLine(RequestLine const &src);
		~RequestLine();

		RequestLine &operator=(RequestLine const &rhs);

		std::string getMethod() const;
		std::string getReqTarget() const;
		std::string getVersion() const;
};

#endif
