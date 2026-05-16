#include <cstring>
#include <iostream>

bool isFolder(std::string location)
{
		char c = location[location.size() - 1];
		if (c == '/')
				return true;
		return false;
}

int main()
{
		std::cout << isFolder("") << "\n"
				  << isFolder("location/") << "\n"
				  << isFolder("location\test") << std::endl;

		return (0);
}
