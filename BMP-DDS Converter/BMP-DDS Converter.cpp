// BMP-DDS Converter.cpp : Defines the entry point for the console application.
//
#include "Interfaces.h"
#include "Converter.h"

#include <memory>
#include <functional>
#include <iostream>

int main()
{
	try {
		std::shared_ptr<IConverter> pConverter(new Converter);
		pConverter->convert("D:\\Code Projects\\Converter\\Debug\\WHT.bmp");
		std::string asd;
		std::cin >> asd;
	}
	catch (...) {
		std::cout << "Something went horribly wrong!" << std::endl;
	}
}

