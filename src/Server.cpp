#include "Library.h"

#include <iostream>

#define log(x) std::cout << x

void print_welcome();

int main()
{

	LSMS::Library lib{};
	
	print_welcome();
	std::cin.get();
	system("cls");

	lib.run();

	return 0;
}

void print_welcome()
{
	std::fstream file;
	std::string line;

	try
	{
		file.open((fm::init_workingdir() + "Welcome.txt"), std::ios::in | std::ios::binary);
		if (file.is_open())
		{
			while (!file.eof())
			{
				fm::_getline(file, line);
				std::cout << line << std::endl;
			}
		}
		else
			log("Could not open Welcome Message\n");
	}
	catch (const std::ifstream::failure &e)
	{
	}
	file.close();
};
