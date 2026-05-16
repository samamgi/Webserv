#include "Graphics.hpp"

void graTopLine()
{
		std::cout << TOP_LEFT;
		for (size_t i = 0; i < LINELENGTH; i++)
				std::cout << HORIZONTAL;
		std::cout << TOP_RIGHT << std::endl;
		graEmptyLine();
}

void graBottomLine()
{
		graEmptyLine();
		std::cout << BOT_LEFT;
		for (size_t i = 0; i < LINELENGTH; i++)
				std::cout << HORIZONTAL;
		std::cout << BOT_RIGHT << std::endl;
}

void graEmptyLine()
{
		std::string line(LINELENGTH, ' ');

		std::cout << VERTICAL << line << VERTICAL << std::endl;
}

void graSeparator()
{
		graEmptyLine();
		std::cout << MID_LEFT;
		for (size_t i = 0; i < LINELENGTH; i++)
				std::cout << HORIZONTAL;
		std::cout << MID_RIGHT << std::endl;
		graEmptyLine();
}

std::string graPad(std::string const &str)
{
		std::string padded;

		padded = std::string(PADDING, ' ') + str;
		return padded;
}

void graTextLine(std::string const &str)
{
		std::string padded = graPad(str);
		const size_t strLen = padded.length();
		const size_t restLen = LINELENGTH - strLen;

		std::cout << VERTICAL << padded << std::string(restLen, ' ') << VERTICAL << std::endl;
}

void graTextHeader(std::string const &str)
{
		graEmptyLine();
		std::string header = "[ " + str + " ]";
		std::string padded = graPad(header);
		const size_t strLen = padded.length();
		const size_t restLen = LINELENGTH - strLen;

		std::cout << VERTICAL << GREEN << padded << RESET << std::string(restLen, ' ') << VERTICAL
				  << std::endl;
		graEmptyLine();
}

void graTextElement(std::string const &str)
{
		std::string element = SYMBOL + str;
		std::string padded = graPad(element);
		const size_t strLen = padded.length();
		const size_t restLen = LINELENGTH - strLen;

		std::cout << VERTICAL << padded << std::string(restLen, ' ') << VERTICAL << std::endl;
}

void graTextElement(int const &num)
{
		std::string element = SYMBOL + intToString(num);
		std::string padded = graPad(element);
		const size_t strLen = padded.length();
		const size_t restLen = LINELENGTH - strLen;

		std::cout << VERTICAL << padded << std::string(restLen, ' ') << VERTICAL << std::endl;
}

void graTime(std::string const &str)
{
		std::string time = timeStamp();
		if (!str.empty())
				time = time + " " + str;
		std::string padded = graPad(time);
		const size_t strLen = visLen(padded);
		const size_t restLen = LINELENGTH - strLen;

		std::cout << VERTICAL << padded << std::string(restLen, ' ') << VERTICAL << std::endl;
}

void graTime()
{
		std::string time = timeStamp();
		std::string padded = graPad(time);
		const size_t strLen = visLen(padded);
		const size_t restLen = LINELENGTH - strLen;

		std::cout << VERTICAL << padded << std::string(restLen, ' ') << VERTICAL << std::endl;
}

void graError(std::string const &error)
{
		std::string str = ERROR_TAG + error;
		std::string padded = graPad(str);
		const size_t strLen = visLen(padded);
		const size_t restLen = LINELENGTH - strLen;

		std::cerr << VERTICAL << padded << std::string(restLen, ' ') << VERTICAL << std::endl;
}
