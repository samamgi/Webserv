#ifndef	HTTPARSERTESTER_HPP
#define	HTTPARSERTESTER_HPP

#define GRE		"\033[32m"

#include "HttpRequest.hpp"

class	HttpRequest;

class	HttpParserTester	{
private:
	HttpParserTester();
	HttpParserTester( HttpParserTester const & src );
	~HttpParserTester();

	HttpParserTester &	operator=( HttpParserTester const & rhs );

	/*----------Delete Methods------------*/
	static void	deleteMethod( ServerManager & s );	

	/*---------------Http Message------------------*/
	static void	onlyASCII(ServerManager & s);
	static void	crWithoutLf( ServerManager & s );
	static void	emptyLinesBeforeReqLine( ServerManager & s );
	static void	isspaceBeforeHeader( ServerManager & s );
	static void shouldHaveOneHost( ServerManager & s );

	/*---------------Host Header-------------------*/
	static void validHostSyntaxis();

	/*--------------Request Line-------------------*/
	static void	shouldHaveTwoSpaces();
	static void shouldHaveThreeTokens();
	static void	uriTooLong();
	static void httpVersion();
	static void	implementedMethod();

	/*-------------------Uri-----------------------*/
	static void	invalidCharUri();
	static void	invalidForm();
	static void	validPercentEncoded();
	static void	uriReconstruction();

	/*------------------Headers--------------------*/
	static void	parseHeaderSyntaxis();
	static void	pushHeaderValues();
	static void	pushMoreValues();
	
	/*---------------Multipart Body----------------*/
	static void	boundaryExist(ServerManager & s);
	static void	checkBoundary( ServerManager & s );

public:
	
	static void	run(ServerManager & s);

	static void	parseHttpMessageTest( ServerManager & s);
	static void	parseRequestLineTest();
	static void parseUriTest();
	static void	parseHostTest();
	static void	parseHeadersTest();
	static void parseMultipartBodyTest( ServerManager & s );
};

#endif