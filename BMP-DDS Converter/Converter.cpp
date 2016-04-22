
#include "Converter.h"

#include <functional>
#include <iostream>

Converter::Converter()
{
}


Converter::~Converter()
{
}

void Converter::convert(const std::string& location)
{
	try {
		std::shared_ptr<IFiletype> pFiletype = createFiletype(location);
	}
	catch (std::bad_function_call& e) {
		std::cout << "convert: " << e.what() << std::endl;
	}

}

std::shared_ptr<IFiletype> Converter::createFiletype(const std::string & location)
{
	//Separate filetype from the filename
	std::size_t dot = location.find('.');
	if (dot == std::string::npos) { 
		std::cerr << "Bad file type" << std::endl;
		throw std::bad_function_call(); }
	std::string filetype = location.substr(dot + 1);
	if (filetype == "") {
		std::cerr << "Bad file type" << std::endl;
		throw std::bad_function_call();
	}
	
	//Create correct type ptr
	std::shared_ptr<IFiletype> pFiletype;
	if (filetype == "bmp") pFiletype = std::shared_ptr<IFiletype>(new BMPFile(location));
	else if (filetype == "dds") pFiletype = std::shared_ptr<IFiletype>(new DDSFile(location));
	else throw std::bad_function_call();

	//check if previous filetype exists
	//if exists, erase
	auto it = m_filetypes.find(filetype);
	if (it != m_filetypes.end()) {
		m_filetypes.erase(it);
	}

	//insert
	m_filetypes.insert(std::make_pair(filetype, pFiletype));

	return pFiletype;
}
