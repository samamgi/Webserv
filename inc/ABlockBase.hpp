#ifndef ABLOCKBASE_HPP
#define ABLOCKBASE_HPP

#include "Utils.hpp"

class ABlockBase
{
	public:
		ABlockBase();
		virtual ~ABlockBase();
		ABlockBase(ABlockBase const &other);
		ABlockBase &operator=(ABlockBase const &other);

		virtual std::string const &getRoot() const = 0;
		unsigned int const &getBodySize() const;
		std::vector<std::string> const &getReturnDirective() const;
		std::vector<std::string> const &getIndex() const;
		std::vector<std::string> const &getAllowedMethods() const;
		std::map<int, std::string> const &getErrorPage() const;
		std::map<std::string, std::string> const &getCgiPass() const;

		bool checkFlag(std::string const &value);
		size_t fillRoot(std::vector<std::string> &buffer, size_t i);
		size_t fillIndex(std::vector<std::string> &buffer, size_t i);
		size_t fillBodySize(std::vector<std::string> &buffer, size_t i);
		size_t fillReturnDirectives(std::vector<std::string> &buffer, size_t i);
		size_t fillErrorPage(std::vector<std::string> &buffer, size_t i);
		size_t fillAllowedMethods(std::vector<std::string> &buffer, size_t i);
		size_t fillCgiPass(std::vector<std::string> &buffer, size_t i);

	protected:
		std::string _root;
		unsigned int _bodySize;
		std::vector<std::string> _flag;
		std::vector<std::string> _index;
		std::vector<std::string> _allowedMethods;
		std::vector<std::string> _returnDirective;
		std::map<int, std::string> _errorPage;
		std::map<std::string, std::string> _cgiPass;
};

#endif
