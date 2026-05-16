#ifndef URI_HPP
#define URI_HPP

#include <string>
#include <vector>

class Uri
{
	private:
		std::string uri;
		std::string scheme;
		std::string authority;
		std::string path;
		std::string query;
		std::string fragment;

		Uri();

	public:
		Uri(std::string const &req_target, std::string const &host);
		Uri(Uri const &src);
		~Uri();

		Uri &operator=(Uri const &rhs);

		std::string getUri() const;
		std::string getScheme() const;
		std::string getAuthority() const;
		std::string getPath() const;
		std::string getQuery() const;
		std::string getFragment() const;
};

#endif
