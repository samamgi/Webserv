#include "HttpParserTester.hpp"
#include "HttpParser.hpp"
#include "RequestLine.hpp"
#include "Uri.hpp"
#include "Utils.hpp"
#include "Headers.hpp"
#include <iostream>
#include <string>


void	HttpParserTester::run( ServerManager & s ) {


	HttpRequest		req = HttpRequest( std::make_pair(8080, "127.0.0.1"), s); //for tests

	char 	buffer[] = "GET / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n";
	size_t	bytes = strlen( buffer );


	req.sendBuffer( buffer, bytes );
	if ( req.getStatusCode() == 200 )
		std::cout << GRE << "Valid request / Test OK" << RESET << std::endl;
	else 
		std::cout << RED << "Invalid request, error: " << req.getStatusCode() <<" / Test FAIL" << std::endl;

	parseHttpMessageTest( s );
	parseRequestLineTest();
	parseUriTest();
	parseHostTest();
	parseHeadersTest();
	parseMultipartBodyTest( s );
	// deleteMethod( s );
	
}

/*--------------------------Post And Delete------------------------------------*/

void	HttpParserTester::deleteMethod( ServerManager & s ) {
	std::cout << "_____________Delete tests_____________" << std::endl;

	HttpRequest		req = HttpRequest( std::make_pair(8080, "127.0.0.1"), s); //for tests

	char 	buffer[] = "DELETE /uploads/text.txt HTTP/1.1\r\nHost: 127.0.0.1 \r\nUser-Agent: curl/7.68.0\r\n\r\n";
	size_t	bytes = strlen( buffer );


	req.sendBuffer( buffer, bytes );
	if ( req.getStatusCode() == 204 )
		std::cout << GRE << "Valid request / Test OK" << RESET << std::endl;
	else 
		std::cout << RED << "Invalid request, error: " << req.getStatusCode() <<" / Test FAIL" << std::endl;





	std::cout << RESET << std::endl << "_____________End tests_____________" << std::endl << std::endl;
}


/*--------------------------Multipart Body------------------------------------*/

void	HttpParserTester::boundaryExist( ServerManager & s ) {
	HttpRequest		req = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer[] = "POST /uploads/ HTTP/1.1\r\nHost: localhost\r\nContent-Type: multipart/form-data\r\nContent-Length: 62\r\n\r\n----1234\r\nContent-Disposition: form-data; name=description\r\n\r\nfirst body content\r\n----1234\r\nContent-Disposition: form/data; name=description\r\nOther-Header:Random\r\n\r\nsecond body content\r\n----1234--\r\n";
	size_t	bytes = strlen( buffer );


	req.sendBuffer( buffer, bytes );
	if ( req.getStatusCode() == 200 )
		std::cout << RED << "Valid message:    Multipart Content-Type header without boundary accepted / Test FAIL";
	else 
		std::cout << GRE << "Invalid request, error: " << req.getStatusCode() <<" Multipart Content-Type header without boundary not accepted/ Test OK";
	std::cout << RESET << std::endl;
	/*------------------*/
	
	HttpRequest		req1 = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer1[] = "POST /uploads/ HTTP/1.1\r\nHost: localhost\r\nContent-Type: multipart/form-data; bb=-1234\r\nContent-Length: 62\r\n\r\n----1234\r\nContent-Disposition: form-data; name=description\r\n\r\nfirst body content\r\n----1234\r\nContent-Disposition: form/data; name=description\r\nOther-Header:Random\r\n\r\nsecond body content\r\n----1234--\r\n";
	size_t	bytes1 = strlen( buffer1 );


	req1.sendBuffer( buffer1, bytes1 );
	if ( req1.getStatusCode() == 200 )
		std::cout << RED << "Valid message:    Multipart Content-Type header without boundary accepted / Test FAIL";
	else 
		std::cout << GRE << "Invalid request, error: " << req.getStatusCode() <<" Multipart Content-Type header without boundary not accepted/ Test OK";
	std::cout << RESET << std::endl;

	/*-------------------*/
	HttpRequest		req2 = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer2[] = "POST /uploads/ HTTP/1.1\r\nHost: localhost\r\nContent-Type: multipart/form-data; boundary=--1 234\r\nContent-Length: 62\r\n\r\n----1 234\r\nContent-Disposition: form-data; name=description\r\n\r\nfirst body content\r\n----1 234\r\nContent-Disposition: form/data; name=description\r\nOther-Header:Random\r\n\r\nsecond body content\r\n----1 234--\r\n";
	size_t	bytes2 = strlen( buffer2 );


	req2.sendBuffer( buffer2, bytes2 );
	if ( req2.getStatusCode() == 200 )
		std::cout << RED << "Valid message:		boundary with spaces accepted / Test FAIL";
	else 
		std::cout << GRE << "Invalid request, error: " << req.getStatusCode() <<" boundary with spaces not accepted/ Test OK";
	std::cout << RESET << std::endl;
}


void	HttpParserTester::checkBoundary( ServerManager & s ) {
	HttpRequest		req = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer[] = "POST /uploads/ HTTP/1.1\r\nHost: localhost\r\nContent-Type: multipart/form-data; boundary=--1234\r\nContent-Length: 62\r\n\r\n----14\r\nContent-Disposition: form-data; name=description\r\n\r\nfirst body content\r\n----1234\r\nContent-Disposition: form/data; name=description\r\nOther-Header:Random\r\n\r\nsecond body content\r\n----1234--\r\n";
	size_t	bytes = strlen( buffer );


	req.sendBuffer( buffer, bytes );
	if ( req.getStatusCode() == 200 )
		std::cout << RED << "Valid message:    good first boundary in body / Test FAIL";
	else 
		std::cout << GRE << "Invalid request, error: " << req.getStatusCode() <<" wrong first boundary in body / Test OK";
	std::cout << RESET << std::endl;
	/*------------------*/
	
}


void	HttpParserTester::parseMultipartBodyTest( ServerManager & s ) {

	std::cout << "_____________Multipart Body tests_____________" << std::endl;

	boundaryExist( s );
	checkBoundary( s );

//	std::cout << RESET << std::endl << "_____________End tests_____________" << std::endl << std::endl;
}


/*--------------------------Request Message------------------------------------*/

void	HttpParserTester::onlyASCII( ServerManager & s ) {

	HttpRequest		req = HttpRequest( std::make_pair(8080, "127.0.0.1"), s); //for tests

	char 	buffer[] = "GET /こん HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n";
	size_t	bytes = strlen( buffer );


	req.sendBuffer( buffer, bytes );
	if ( req.getStatusCode() == 200 )
		std::cout << RED << "Valid message:	  'こん' char accepted / Test FAIL";
	else 
		std::cout << GRE << "Invalid request, error: " << req.getStatusCode() <<" 'こん' char not accepted / Test OK";
	std::cout << RESET << std::endl;
	/*------------------*/
	
	HttpRequest		req1 = HttpRequest( std::make_pair(8080, "127.0.0.1"), s); //for tests

	char 	buffer1[] = "GET / HTTP/1.1\r\nHost:france \r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n";
	size_t	bytes1 = strlen( buffer1 );


	req1.sendBuffer( buffer1, bytes1 );
	if ( req1.getStatusCode() == 200 )
		std::cout << RED << "Valid message:	  'ñ' char accepted / Test FAIL";
	else 
		std::cout << GRE << "Invalid request, error: " << req1.getStatusCode() <<" 'ñ' char not accepted / Test OK";
	std::cout << RESET << std::endl;
}


void	HttpParserTester::crWithoutLf( ServerManager & s ) {
	HttpRequest		req = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer[] = "GET / HT\rTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n";
	size_t	bytes = strlen( buffer );


	req.sendBuffer( buffer, bytes );
	if ( req.getStatusCode() == 200 )
		std::cout << RED << "Valid message:    CR without LF accepted / Test FAIL";
	else 
		std::cout << GRE << "Invalid request, error: " << req.getStatusCode() <<" CR without LF not accepted/ Test OK";
	std::cout << RESET << std::endl;
	/*------------------*/
	
	HttpRequest		req1 = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer1[] = "GET / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nContent-Length: 6\r\n\r\nhe\rllo";
	size_t	bytes1 = strlen( buffer1 );


	req1.sendBuffer( buffer1, bytes1 );
	if ( req1.getStatusCode() == 200 )
		std::cout << GRE << "Valid message:    CR without LF in body accepted / Test OK" << std::endl;
	else 
		std::cout << RED << "Invalid request, error: " << req1.getStatusCode() <<" CR without LF in body not accepted / Test FAIL";
	std::cout << RESET << std::endl;

	/*-------------------*/
	HttpRequest		req2 = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer2[] = "GET / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nContent-Length: 7\r\n\r\nhe\r\nllo";
	size_t	bytes2 = strlen( buffer2 );


	req2.sendBuffer( buffer2, bytes2 );
	if ( req2.getStatusCode() == 200 )
		std::cout << GRE << "Valid message:    CRLF in body accepted / Test OK" << std::endl;
	else 
		std::cout << RED << "Invalid request, error: " << req2.getStatusCode() <<" CRLF in body not accepted / Test FAIL";
	std::cout << RESET << std::endl;
}


void	HttpParserTester::emptyLinesBeforeReqLine( ServerManager & s ) {

	HttpRequest		req1 = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer1[] = "\r\n\r\n\r\nGET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 6\r\n\r\nhe\rllo";
	size_t	bytes1 = strlen( buffer1 );


	req1.sendBuffer( buffer1, bytes1 );
	if ( req1.getStatusCode() == 200 )
		std::cout << GRE << "Valid message:    three empty lines (CRLF) before Request-Line accepted / Test OK" << std::endl;
	else 
		std::cout << RED << "Invalid request, error: " << req1.getStatusCode() <<" three CRLF before req-line not accepted / Test FAIL";
	std::cout << RESET << std::endl;

	/*-------------------*/
	HttpRequest		req2 = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer2[] = "\r\nGET / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nContent-Length: 7\r\n\r\nhe\r\nllo";
	size_t	bytes2 = strlen( buffer2 );


	req2.sendBuffer( buffer2, bytes2 );
	if ( req2.getStatusCode() == 200 )
		std::cout << GRE << "Valid message:    one empty line (CRLF) before Request-Line accepted / Test OK" << std::endl;
	else 
		std::cout << RED << "Invalid request, error: " << req2.getStatusCode() <<" one CRLF before req-line not accepted / Test FAIL";
	std::cout << RESET << std::endl;
}


void	HttpParserTester::isspaceBeforeHeader( ServerManager & s ) {

	HttpRequest		req = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer[] = "GET / HTTP/1.1\r\n\tHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n";
	size_t	bytes = strlen( buffer );


	req.sendBuffer( buffer, bytes );
	if ( req.getStatusCode() == 200 )
		std::cout << RED << "Valid message:    \\tab before header accepted / Test FAIL" << std::endl;
	else 
		std::cout << GRE << "Invalid request, error: " << req.getStatusCode() <<" \\tab (isspace) before header not accepted / Test OK";
	std::cout << RESET << std::endl;
	/*------------------*/
	
	HttpRequest		req1 = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer1[] = "GET / HTTP/1.1\r\n   Host:localhost \r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n";
	size_t	bytes1 = strlen( buffer1 );


	req1.sendBuffer( buffer1, bytes1 );
	if ( req1.getStatusCode() == 200 )
		std::cout << RED << "Valid message:    spaces before header accepted / Test FAIL" << std::endl;
	else 
		std::cout << GRE << "Invalid request, error: " << req1.getStatusCode() <<"   spaces (isspace) before header not accepted / Test OK";
	std::cout << RESET << std::endl;
}

void	HttpParserTester::shouldHaveOneHost( ServerManager & s ) {

	HttpRequest		req = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer[] = "GET / HT\rTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\nHost: other\r\n\r\n";
	size_t	bytes = strlen( buffer );


	req.sendBuffer( buffer, bytes );
	if ( req.getStatusCode() == 200 )
		std::cout << RED  <<"Valid message:    two hosts accepted / Test FAIL" << std::endl;
	else 
		std::cout << GRE << "Invalid request, error: " << req.getStatusCode() <<" two hosts not accepted/ Test OK";
	std::cout << RESET << std::endl;
	/*------------------*/
	
	HttpRequest		req1 = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer1[] = "GET / HTTP/1.1\r\nUser-Agent: curl/7.68.0\r\nContent-Length: 6\r\n\r\nhe\rllo";
	size_t	bytes1 = strlen( buffer1 );


	req1.sendBuffer( buffer1, bytes1 );
	if ( req1.getStatusCode() == 200 )
		std::cout << RED <<"Valid message:     message without host accepted / Test FAIL" << RESET << std::endl;
	else 
		std::cout << GRE << "Invalid request, error: " << req1.getStatusCode() <<"  message without host not accepted / Test OK";
	std::cout << RESET << std::endl;
	/*-------------------*/

	HttpRequest		req2 = HttpRequest( std::make_pair(8080, "127.0.0.1"), s);

	char 	buffer2[] = "GET / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nHost:	localhost	\r\nContent-Length: 7\r\n\r\nhe\r\nllo";
	size_t	bytes2 = strlen( buffer2 );


	req2.sendBuffer( buffer2, bytes2 );
	if ( req2.getStatusCode() == 200 )
		std::cout << GRE << "Valid message:    two host with same content accepted / Test OK" << std::endl;
	else 
		std::cout << RED << "Invalid request, error: " << req2.getStatusCode() <<" two host with same content not accepted / Test FAIL";
	std::cout << RESET << std::endl;

}

void	HttpParserTester::parseHttpMessageTest( ServerManager & s ) {
	
	std::cout << "_____________Http message tests_____________" << std::endl;

	onlyASCII( s );
	crWithoutLf( s );
	emptyLinesBeforeReqLine( s );
	shouldHaveOneHost( s );
	isspaceBeforeHeader( s );

	std::cout << std::endl;

}



/*-----------------------------------Host Header-----------------------------------------*/


void	HttpParserTester::validHostSyntaxis() {
	
	std::string host( "www.example.com" );
	
	try {
		HttpParser::parseHost( host );
		std::cout << GRE << "valid host:	  " << host << " accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  " << host << " host not accepted/ Test FAIL" << std::endl;
	}
	
	host = "example.com";
	try {
		HttpParser::parseHost( host );
		std::cout << GRE << "valid host:	  " << host << " accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  " << host << " host not accepted/ Test FAIL" << std::endl;
	}

	host = "http://example.com";
	try {
		HttpParser::parseHost( host );
		std::cout << RED << "valid host:	  " << host << " accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << host << " host not accepted/ Test OK" << std::endl;
	}

	host = "example..com";
	try {
		HttpParser::parseHost( host );
		std::cout << RED << "valid host:	  " << host << " accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << host << " host not accepted/ Test OK" << std::endl;
	}

	host = "exam ple.com";
	try {
		HttpParser::parseHost( host );
		std::cout << RED << "valid host:	  " << host << " accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << host << " host not accepted/ Test OK" << std::endl;
	}

	host = "127.0.0.1";
	try {
		HttpParser::parseHost( host );
		std::cout << GRE << "valid host:	  " << host << " accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  " << host << " host not accepted/ Test FAIL" << std::endl;
	}

	host = "domain.com:443";
	try {
		HttpParser::parseHost( host );
		std::cout << GRE << "valid host:	  " << host << " accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  " << host << " host not accepted/ Test FAIL" << std::endl;
	}

	host = "domain.com:65536";
	try {
		HttpParser::parseHost( host );
		std::cout << RED << "valid host:	  " << host << " accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << host << " host not accepted/ Test OK" << std::endl;
	}

	host = "domain.com:";
	try {
		HttpParser::parseHost( host );
		std::cout << RED << "valid host:	  " << host << " accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << host << " host not accepted/ Test OK" << std::endl;
	}

	host = "domain.com:08";
	try {
		HttpParser::parseHost( host );
		std::cout << RED << "valid host:	  " << host << " accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << host << " host not accepted/ Test OK" << std::endl;
	}

	host = "domain.com:80.0";
	try {
		HttpParser::parseHost( host );
		std::cout << RED << "valid host:	  " << host << " accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << host << " host not accepted/ Test OK" << std::endl;
	}
	
	host = "domain.com:abc";
	try {
		HttpParser::parseHost( host );
		std::cout << RED << "valid host:	  " << host << " accepted / Test FAIL" << RESET << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << host << " host not accepted/ Test OK" << RESET << std::endl;
	}

	host = "#special%";
	try {
		HttpParser::parseHost( host );
		std::cout << RED << "valid host:	  " << host << " empty spaces host accepted / Test FAIL" << RESET << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << host << " empty spaces host not accepted/ Test OK" << RESET << std::endl;
	}

	host = "-domain.com";
	try {
		HttpParser::parseHost( host );
		std::cout << RED << "valid host:	  " << host << " empty host accepted / Test FAIL" << RESET << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << host << " empty host not accepted/ Test OK" << RESET << std::endl;
	}

	host = "domain-.com";
	try {
		HttpParser::parseHost( host );
		std::cout << RED << "valid host:	  " << host << " empty host accepted / Test FAIL" << RESET << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << host << " empty host not accepted/ Test OK" << RESET << std::endl;
	}

}

void	HttpParserTester::parseHostTest() {

	std::cout << "_______________Host syntaxis tester_____________" << std::endl;
	validHostSyntaxis();

	std::cout << std::endl;
}


/*-----------------------------------Request Line----------------------------------------*/

void	HttpParserTester::shouldHaveTwoSpaces() {
	try {
		std::string	req_line( "POST /form HTTP/1.1" );
		HttpParser::parseRequestLine( req_line );
		std::cout << GRE << "Valid reqline:    two spaces accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  two spaces not accepted / Test FAIL" << std::endl;
	}
	try {
		std::string	req_line( "POST /form  HTTP/1.1" );
		HttpParser::parseRequestLine( req_line );
		std::cout << RED << "Valid reqline:    three spaces accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  three spaces not accepted / Test OK" << std::endl;
	}
	try {
		std::string	req_line( "POST /formHTTP/1.1" );
		HttpParser::parseRequestLine( req_line );
		std::cout << RED << "Valid reqline:    one space accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  one space not accepted / Test OK" << std::endl;
	}
}

void	HttpParserTester::shouldHaveThreeTokens() {
	try {
		std::string	req_line( "POST /form HTTP/1.1" );
		HttpParser::parseRequestLine( req_line );
		std::cout << GRE << "Valid req-line:   three tokens accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  three tokens not accepted / Test FAIL" << std::endl;
	}
	try {
		std::string	req_line( "POST  /formHTTP/1.1" );
		HttpParser::parseRequestLine( req_line );
		std::cout << RED <<  "Valid req-line:   two tokens accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  two tokens not accepted / Test OK" << std::endl;
	}
}

void	HttpParserTester::uriTooLong() {
	try {
		std::string	uri( 8001, '/' );
		std::string req_line( "GET " + uri + " HTTP/1.1" );
		HttpParser::parseRequestLine( req_line );
		std::cout << RED << "Valid req-line:   8001 octects accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ": 8001 octets not accepted / Test OK" << std::endl;
	}

	try {
		std::string	uri( 8000, '/' );
		HttpParser::parseRequestLine( "GET " + uri + " HTTP/1.1" );
		std::cout << GRE << "Valid req-line:   8000 octects accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ": 8000 octets not accepted / Test FAIL" << std::endl;
	}
}

void	HttpParserTester::httpVersion() {
	try {
		std::string	req_line( "POST /form HTTP/1.1" );
		HttpParser::parseRequestLine( req_line );
		std::cout << GRE << "Valid http-vers:  HTTP/1.1 version accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  HTTP/1.1 version not accepted / Test FAIL" << std::endl;
	}
	try {
		std::string	req_line( "POST  /form HTTP//1.1" );
		HttpParser::parseRequestLine( req_line );
		std::cout << RED <<  "Valid http-vers:  HTTP//1.1 accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  HTTP//1.1 not accepted / Test OK" << std::endl;
	}
	try {
		std::string	req_line( "POST  /form HTTP/2.1" );
		HttpParser::parseRequestLine( req_line );
		std::cout << RED <<  "Valid http-vers:   HTTP/2.1 accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  HTTP/2.1 not accepted / Test OK" << std::endl;
	}
}

void	HttpParserTester::implementedMethod() {
	try {
		std::string	req_line( "GET /form HTTP/1.1" );
		HttpParser::parseRequestLine( req_line );
		std::cout << GRE << "Implemented method:  GET method accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  GET method not accepted / Test FAIL" << std::endl;
	}
	try {
		std::string	req_line( "FOOMETHOD /form HTTP/1.1" );
		HttpParser::parseRequestLine( req_line );
		std::cout << RED << "Implemented method:  FOOMETHOD accepted / Test FAIL" << RESET << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  FOOMETHOD not accepted / Test OK" << RESET << std::endl;
	}
}

void	HttpParserTester::parseRequestLineTest() {

	std::cout << "_____________Request-line tests_____________" << std::endl;

	shouldHaveTwoSpaces();
	shouldHaveThreeTokens();
	uriTooLong();
	httpVersion();
	implementedMethod();
 
	std::cout << RESET << std::endl;
}


void	HttpParserTester::invalidCharUri() {

	std::string uri( "/test(path)" );
	
	try {
		std::string	http_mess( "GET " + uri + " HTTP/1.1");
		HttpParser::parseRequestLine( http_mess );
		std::cout << GRE << "valid uri:    	  " << uri << " accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  " << uri << " '(' and ')' char not accepted/ Test FAIL" << std::endl;
	}
	
	uri = "/test{path}";
	try {
		std::string	http_mess( "POST " + uri + " HTTP/1.1");
		HttpParser::parseRequestLine( http_mess );
		std::cout << RED << "valid uri:	          " << uri << " accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << uri << " '{' and '}' char not accepted/ Test OK" << std::endl;
	}
} 

void	HttpParserTester::invalidForm() {

	std::string uri( "http://example.com/index.html" );
		
	try {
		std::string	http_mess( "POST " + uri + " HTTP/1.1");
		HttpParser::parseRequestLine( http_mess );
		std::cout << RED << "valid uri:	          " << uri << " absolut-form accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << uri << " absolut-form not accepted/ Test OK" << std::endl;
	}

	uri = "example.com:80";
	try {
		std::string	http_mess( "POST " + uri + " HTTP/1.1");
		HttpParser::parseRequestLine( http_mess );
		std::cout << RED << "valid uri:	          " << uri << " authority-form accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << uri << " authority-form not accepted/ Test OK" << std::endl;
	}

	uri = "*";
	try {
		std::string	http_mess( "POST " + uri + " HTTP/1.1");
		HttpParser::parseRequestLine( http_mess );
		std::cout << RED << "valid uri:	          " << uri << " asterisk-form accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << uri << " asterisk-form not accepted/ Test OK" << std::endl;
	}

} 


void	HttpParserTester::validPercentEncoded() {

	std::string uri( "/test%20path" );
	
	try {
		std::string	http_mess( "GET " + uri + " HTTP/1.1");
		HttpParser::parseRequestLine( http_mess );
		std::cout << GRE << "valid percent:	  " << uri << " accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  " << uri << " percent encoded not accepted/ Test FAIL" << std::endl;
	}

	uri = "/post%A";
	try {
		std::string	http_mess( "POST " + uri + " HTTP/1.1");
		HttpParser::parseRequestLine( http_mess );
		std::cout << RED << "valid percent:	  " << uri << " accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << uri << " percent encoded not accepted/ Test OK" << std::endl;
	}
	
	uri = "/post%RTlala";
	try {
		std::string	http_mess( "POST " + uri + " HTTP/1.1");
		HttpParser::parseRequestLine( http_mess );
		std::cout << RED << "valid percent:	  " << uri << " accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  " << uri << " percent encoded not accepted/ Test OK" << std::endl;
	}

	uri = "/post%4E%33%54lala%F5";
	try {
		std::string	http_mess( "POST " + uri + " HTTP/1.1");
		HttpParser::parseRequestLine( http_mess );
		std::cout << GRE << "valid percent:	  " << uri << " accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  " << uri << " percent encoded not accepted/ Test FAIL" << std::endl;
	}
}

void	HttpParserTester::uriReconstruction() {
	std::string	uri( "/test%20path?query#fragment" ), host( "example.com" );
	Uri			test = Uri( uri, host );
		
	std::cout << std::endl;
	std::cout << RESET << "Uri reconstruction: " << test.getUri() << std::endl;
	std::cout << "path: " << test.getPath() << ", query: " << test.getQuery();
	std::cout << ", fragment: " << test.getFragment() << std::endl;

	std::string	uri2( "/test%20path#fragment" ), host2( "example.com" );
	Uri			test2 = Uri( uri2, host2 );
		
	std::cout << std::endl;
	std::cout << RESET << "Uri reconstruction: " << test2.getUri() << std::endl;
	std::cout << "path: " << test2.getPath() << ", query: " << test2.getQuery();
	std::cout << ", fragment: " << test2.getFragment() << std::endl;

}

void	HttpParserTester::parseUriTest() {

	std::cout << "_________________Uri tests__________________" << std::endl;

	invalidCharUri();
	invalidForm();
	validPercentEncoded();
	uriReconstruction(); 

	std::cout << RESET << std::endl;
}

/*-------------------------------------------Headers--------------------------------------------*/

void	HttpParserTester::parseHeaderSyntaxis() {

	std::string name( "Content-Length " );
	std::string value( "45" );
	std::string arg( name + ":" + value );

	try {
		Headers	h;
		h.setHeader( arg );
		std::cout << RED << "valid:		 '" << name << "' header accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  '" << name << "' header name not accepted/ Test OK" << std::endl;
	}

	name = "Content-Length";
	value = "45";
	arg = name + ":" + value;

	try {
		Headers	h;
		h.setHeader( arg );
		std::cout << GRE << "valid:		  '" << name << "' header name accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  '" << name << "' header name not accepted/ Test FAIL" << std::endl;
	}

	name = "Content-Length";
	value = "	45 ";
	arg = name + ":" + value;

	try {
		Headers	h;
		h.setHeader( arg );
		std::cout << GRE << "valid:		  '" << value << "' header value accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  '" << value << "' header value not accepted/ Test FAIL" << std::endl;
	}

	name = "Accept";
	value = ":::";
	arg = name + ":" + value;

	try {
		Headers	h;
		h.setHeader( arg );
		std::cout << GRE << "valid:		  '" << arg << "'value: '" << h.getHeaderOnlyOneValue( "accept", 0 ) << "' header accepted / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  '" << arg << "' header not accepted/ Test FAIL" << std::endl;
	}


	name = "Content-Length@";
	value = "45";
	arg = name + ":" + value;

	try {
		Headers	h;
		h.setHeader( arg );
		std::cout << RED << "valid:		 '" << name << "' @ char in name accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  '" << name << "' @ char in name not accepted/ Test OK" << std::endl;
	}

	name = "Content-LengthA";
	value = "45";
	arg = name + ":" + value;

	try {
		Headers	h;
		h.setHeader( arg );
		std::cout << GRE << "valid:		 '" << name << "' header ignored / Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  '" << name << "' header not accepted/ Test FAIL" << std::endl;
	}

}

void	HttpParserTester::pushHeaderValues() {

	std::string name( "Content-Length" );
	std::string value( "45,44,34,56" );
	std::string arg( name + ":" + value );

	try {
		Headers	h;
		h.setHeader( arg );
		std::cout << GRE << "valid:		  '" << h.getHeaderOnlyOneValue( "content-length", 0 ) << "' only one value accepted by Content-Lenght/ Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  '" << arg << "' header not accepted/ Test FAIL" << std::endl;
	}
	
	name = "Accept";
	value = "a b,c	, d ";
	arg = name + ":" + value;

	try {
		Headers	h;
		h.setHeader( arg );
		std::cout << GRE << "valid:		  '" << h.getHeaderOnlyOneValue( "accept", 0 ) << "' '" << h.getHeaderOnlyOneValue( "accept", 1 ) << "' '" <<h.getHeaderOnlyOneValue( "accept", 2 ) << "' many values accepted by Accept/ Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  '" << arg << "' header not accepted/ Test FAIL" << std::endl;
	}
}

void	HttpParserTester::pushMoreValues() {

	std::string argC( "Content-Length:45" );
	std::string argC2( "Content-LENGTh:45" );
	std::string argc( "CoNtEnT-LeNgTh:46" );
	std::string argA( "Accept: text/html, image/png" );
	std::string arga( "Accept: image/jpeg, image/gif" );

	Headers h;
	h.setHeader( argC );
	
	h.setHeader( argA );


	try {
		h.setHeader( argc );
		std::cout << RED << "valid:		 '" << argc << "' two Content-Length headers with diferent values accepted / Test FAIL" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << GRE << e.what() << ":  '" << argC << "' '" << argc << "' two Content-Length headers with diferent values not accepted/ Test OK" << std::endl;
	}
	
	try {
		h.setHeader( argC2 );
		std::cout << GRE << "valid:		  '" << argC << "' '" << argC2  << "' second Content-Length header with same values ignored/ Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  '" << argC << "' two Content-Length headers with same values not accepted/ Test FAIL" << std::endl;
	}
	
	try {
		h.setHeader( arga );
		std::cout << GRE << "valid:		  '" << h.getHeaderOnlyOneValue( "accept", 0 ) << "' '" << h.getHeaderOnlyOneValue( "accept", 1 ) << "' '" << h.getHeaderOnlyOneValue( "accept", 2 ) << "' '" << h.getHeaderOnlyOneValue( "accept", 3 ) << "' four values for Accept header accepted/ Test OK" << std::endl;
	} catch (const std::invalid_argument& e) {
		std::cout << RED << e.what() << ":  '" << argA << "' two Accept header with two values not accepted/ Test FAIL" << std::endl;
	}



} 

void	HttpParserTester::parseHeadersTest() {

	std::cout << "_____________Headers tests_____________" << std::endl;

	parseHeaderSyntaxis();	
	pushHeaderValues();
	pushMoreValues();
	

	std::cout << RESET << std::endl;// << "_____________End tests_____________" << std::endl << std::endl;
}
