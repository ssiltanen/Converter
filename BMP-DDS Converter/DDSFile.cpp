#include "DDSFile.h"
#include "MyException.h"

#include <fstream>
#include <iostream>

DDSFile::DDSFile(const std::string& location)
{
	uint8_t* dataBuffer[2] = { nullptr, nullptr };

	m_mainData = nullptr;
	m_additionalData = nullptr;
	m_pDdsHeader = nullptr;

	std::ifstream file(location, std::ios::binary);
	if (!file)
		builderFailed(nullptr, NULL, "Failure to open dds file from location: " + location);

	//Check if file is DDS
	uint8_t* dwMagic = new uint8_t[4]; 
	file.read((char*)dwMagic, 4);
	if (*dwMagic != DDS_dwMagic) {
		delete dwMagic;
		builderFailed(nullptr, NULL, "File " + location + " isn't a dds file");
	}
	delete dwMagic;

	// Allocate byte memory that will hold the header
	dataBuffer[0] = new uint8_t[sizeof(DDS_HEADER)];
	file.read((char*)dataBuffer[0], sizeof(DDS_HEADER));

	// Construct the values from the buffers
	m_pDdsHeader = (DDS_HEADER*)dataBuffer[0];
	
	//Check valid structure sizes
	if (m_pDdsHeader->dwSize != 124 && m_pDdsHeader->ddspf.dwsize != 32) {
		delete m_pDdsHeader;
		builderFailed(dataBuffer, 1, "File " + location + " has invalid header(s)");
	}

	//Check if file is DXT1 compressed
	if (m_pDdsHeader->ddspf.dwfourCC != DXT1) {
		delete m_pDdsHeader;
		builderFailed(dataBuffer, 1, "File " + location + " is not DXT1 compressed DDS file");
	}

	//Check if DXT10 header is used and allocate memory for it if needed
	if (m_pDdsHeader->ddspf.dwflags == DDPF_FOURCC) {
		dataBuffer[1] = new uint8_t[sizeof(DDS_HEADER_DXT10)];
		file.read((char*)dataBuffer[1], sizeof(DDS_HEADER_DXT10));
		m_pDdsHeaderDxt10 = (DDS_HEADER_DXT10*)dataBuffer[1];
	}


}


DDSFile::~DDSFile()
{
	delete m_pDdsHeader;
	if (m_pDdsHeaderDxt10 != nullptr)
		delete m_pDdsHeaderDxt10;
	delete m_mainData;
	delete m_additionalData;
}

void DDSFile::builderFailed(uint8_t * dataBuffer[], int arraySize, std::string & cause) const
{
	if (dataBuffer != nullptr) {
		for (int i = 0; i < arraySize; ++i) {
			delete dataBuffer[i];
		}
	}
	throw MyException(cause);
}
