// BMP-DDS Converter.cpp : Defines the entry point for the console application.
//
#include "Interfaces.h"
#include "Converter.h"

#include <memory>
#include <functional>
#include <iostream>

int main()
{
	std::shared_ptr<IConverter> pConverter(new Converter);
	pConverter->convert("D:\\Code Projects\\BMP-DDS Converter\\Debug\\example.bmp");
	std::string asd;
	std::cin >> asd;
}

