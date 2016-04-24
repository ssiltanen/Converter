
#include "Converter.h"
#include "MyException.h"

#include <functional>
#include <iostream>

Converter::Converter()
{
}


Converter::~Converter()
{
}

void Converter::VLoadFile(const std::string & fileLocation)
{
	try {
		loadFiletype(fileLocation);
		std::cout << "File " + fileLocation + " loaded successfully" << std::endl;
	}
	catch (std::exception& e) {
		std::cout << "Error in loadFile(): " << e.what() << std::endl;
	}
}

void Converter::VConvert(const std::string& fromFormat, const std::string& targetFormat)
{
	std::shared_ptr<IFiletype> pFromFiletype;
	std::shared_ptr<IFiletype> pTargetFiletype;
	try {
		pFromFiletype = getFiletype(fromFormat);
		pTargetFiletype = getFiletype(targetFormat);
	}
	catch (std::exception& e) {
		std::cout << "Error in Convert(): " << e.what() << std::endl;
		return;
	}

	//DO CONVERSION HERE

	//CREATE FILE
}

void Converter::loadFiletype(const std::string & location)
{
	//Separate filetype from the filename
	std::size_t dot = location.find('.');
	if (dot == std::string::npos)  
		throw MyException("No filetype ending found");
	const std::string filetype = location.substr(dot + 1);
	if (filetype == "")
		throw MyException("No filetype ending found");
	
	//Create filetype
	std::shared_ptr<IFiletype> pFiletype = getUninitializedFiletype(filetype);

	//Initialize filetype with actual file
	try {
		pFiletype->VInitialize(location);
	}
	catch (MyException& e) {
		removeFiletype(filetype);
		throw;
	}
}

void Converter::removeFiletype(const std::string & filetype)
{
	auto it = m_filetypes.find(filetype);
	if (it != m_filetypes.end()) {
		m_filetypes.erase(it);
	}
}

std::shared_ptr<IFiletype> Converter::getFiletype(const std::string & filetype) const
{
	auto it = m_filetypes.find(filetype);
	if (it == m_filetypes.end()) {
		throw MyException("Filetype " + filetype + " has not been loaded yet");
	}
	return it->second;
}

std::shared_ptr<IFiletype> Converter::getUninitializedFiletype(const std::string & filetype)
{
	removeFiletype(filetype);

	std::shared_ptr<IFiletype> ptr;
	if (filetype == "bmp" || filetype == "BMP") 
		ptr = std::shared_ptr<IFiletype>(new BMPFile());
	else if (filetype == "dds" || filetype == "DDS") 
		ptr = std::shared_ptr<IFiletype>(new DDSFile());
	else 
		throw MyException("Filetype " + filetype + " not supported");

	m_filetypes.insert(std::make_pair(filetype, ptr));

	return ptr;
}
