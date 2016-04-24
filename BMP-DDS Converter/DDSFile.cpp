#include "DDSFile.h"
#include "MyException.h"

#include <fstream>
#include <iostream>
#include <algorithm>

DDSFile::DDSFile(const std::string& location)
{
	uint8_t* dataBuffer = nullptr;

	m_mainData = nullptr;
	m_additionalData = nullptr;
	m_pDdsHeader = nullptr;

	std::ifstream file(location, std::ios::binary);
	if (!file)
		builderFailed(nullptr, "Failure to open dds file from location: " + location);

	char dwMagic[4];
	file.read(dwMagic, 4);

	//Check if file is DDS
	if (strncmp(dwMagic, "DDS ", 4) != 0) 
		builderFailed(nullptr, "File " + location + " isn't a dds file");

	// Allocate byte memory that will hold the header
	dataBuffer = new uint8_t[sizeof(DDS_HEADER)];
	file.read((char*)dataBuffer, sizeof(DDS_HEADER));

	// Construct the values from the buffers
	m_pDdsHeader = (DDS_HEADER*)dataBuffer;
	
	//Check valid structure sizes
	if (m_pDdsHeader->dwSize != 124 || m_pDdsHeader->ddspf.dwsize != 32) 
		builderFailed(dataBuffer, "File " + location + " has invalid header(s)");

	//Check if file contains compressed RGB data
	if (m_pDdsHeader->ddspf.dwflags != DDPF_FOURCC) 
		builderFailed(dataBuffer, "File " + location + " texture does not contain compressed RGB data");

	//Check if file is DXT1 compressed
	if (m_pDdsHeader->ddspf.dwfourCC != DXT1_FOURCC) 
		builderFailed(dataBuffer, "File " + location + " is not DXT1 compressed DDS file");

	//Calculate main image size, (blocksize = 8 and packing = 4 for DXT1)
	unsigned int mainImageSize = std::max((unsigned int)1, (m_pDdsHeader->dwWidth + 3) / 4) * std::max((unsigned int)1, (m_pDdsHeader->dwHeight + 3) / 4) * 8;
	//unsigned int imageSizeWithMipmaps = 

	delete dataBuffer;
}


DDSFile::~DDSFile()
{
	delete m_pDdsHeader;
	delete m_mainData;
	delete m_additionalData;
}

void DDSFile::builderFailed(uint8_t * dataBuffer, std::string & cause) const
{
	delete dataBuffer;
	throw MyException(cause);
}
