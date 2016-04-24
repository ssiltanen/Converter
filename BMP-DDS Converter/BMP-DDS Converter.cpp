//Sampo Siltanen 2016
//
//Software that converts 24 bits per pixel bmp file to DDS BC1(DXT1) compressed file and vice versa
//Implementation done with only standard libraries
//
//BMP-DDS Converter.cpp : Defines the entry point for the console application.

#include "Interfaces.h"
#include "Converter.h"

#include <memory>
#include <iostream>

int main()
{
	try {
		std::shared_ptr<IConverter> pConverter(new Converter);
		pConverter->VLoadFile("D:\\Code Projects\\Converter\\Debug\\kode_red.bmp");
		pConverter->VLoadFile("D:\\Code Projects\\Converter\\Debug\\dxt1_red.dds");
		std::string asd;
		std::cin >> asd;
	}
	catch (...) {
		std::cout << "Something went horribly wrong!" << std::endl;
	}
}

