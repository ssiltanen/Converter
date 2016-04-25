#include "DDSFile.h"
#include "MyException.h"

#include <fstream>
#include <iostream>
#include <algorithm>

DDSFile::DDSFile()
{
	m_pDdsHeader = nullptr;
	m_mainData = nullptr;
}

DDSFile::~DDSFile()
{
	delete m_pDdsHeader;
	delete m_mainData;
}

void DDSFile::VInitializeFromFile(const std::string & location)
{
	uint8_t* dataBuffer = nullptr; //temp

	m_mainData = nullptr;
	m_pDdsHeader = nullptr;

	std::ifstream file(location, std::ios::binary);
	if (!file)
		initializeFailed(nullptr, "Failure to open dds file from location: " + location);

	char dwMagic[4];
	file.read(dwMagic, 4);

	//Check if file is DDS
	if (strncmp(dwMagic, "DDS ", 4) != 0)
		initializeFailed(nullptr, "File " + location + " isn't a dds file");

	// Allocate byte memory that will hold the header
	dataBuffer = new uint8_t[sizeof(DDS_HEADER)];
	file.read((char*)dataBuffer, sizeof(DDS_HEADER));

	// Construct the values from the buffers
	m_pDdsHeader = (DDS_HEADER*)dataBuffer;

	//Check valid structure sizes
	if (m_pDdsHeader->dwSize != 124 || m_pDdsHeader->ddspf.dwsize != 32)
		initializeFailed(dataBuffer, "File " + location + " has invalid header(s)");

	//Check if file contains compressed RGB data
	if (m_pDdsHeader->ddspf.dwflags != DDPF_FOURCC)
		initializeFailed(dataBuffer, "File " + location + " texture does not contain compressed RGB data");

	//Check if file is DXT1 compressed
	if (m_pDdsHeader->ddspf.dwfourCC != DXT1_FOURCC)
		initializeFailed(dataBuffer, "File " + location + " is not DXT1 compressed DDS file");

	//Calculate main image size, (blocksize = 8 and packing = 4 for DXT1)
	unsigned int mainImageSize = std::max((unsigned int)1, (m_pDdsHeader->dwWidth + 3) / 4) * std::max((unsigned int)1, (m_pDdsHeader->dwHeight + 3) / 4) * 8;
	if (mainImageSize != m_pDdsHeader->dwPitchOrLinearSize)
		std::cerr << "Warning! Calculated main image size differ from header value" << std::endl;

	//Read the main data
	m_mainData = new uint8_t[mainImageSize];
	file.read((char*)m_mainData, mainImageSize);

	delete dataBuffer;
}

void DDSFile::VConversionInitialize(uint8_t * uncompressedImageData, unsigned int width, unsigned int height)
{
	m_mainData = DXT1Compress(uncompressedImageData);
	delete uncompressedImageData;

	//Create header
	m_pDdsHeader = new DDS_HEADER();
	m_pDdsHeader->dwSize = HEADER_SIZE;
	m_pDdsHeader->dwFlags = FLAGS;
	m_pDdsHeader->dwHeight = height;
	m_pDdsHeader->dwWidth = width;
	m_pDdsHeader->dwPitchOrLinearSize = std::max((unsigned int)1, (m_pDdsHeader->dwWidth + 3) / 4) * std::max((unsigned int)1, (m_pDdsHeader->dwHeight + 3) / 4) * 8;
	m_pDdsHeader->dwDepth = 0;
	m_pDdsHeader->dwMipMapCount = 0;
	m_pDdsHeader->dwReserved1[11] = {};

	//Create pixel format
	DDS_PIXELFORMAT pixelFormat;
	pixelFormat.dwsize = INFO_SIZE;
	pixelFormat.dwflags = DDPF_FOURCC;
	pixelFormat.dwfourCC = DXT1_FOURCC;
	pixelFormat.dwRGBBitCount = 0;
	pixelFormat.dwRBitMask = 0;
	pixelFormat.dwGBitMask = 0;
	pixelFormat.dwBBitMask = 0;
	pixelFormat.dwABitMask = 0;

	m_pDdsHeader->ddspf = pixelFormat;
	m_pDdsHeader->dwCaps = DDSCAPS_TEXTURE;
	m_pDdsHeader->dwCaps2 = 0;
	m_pDdsHeader->dwCaps3 = 0;
	m_pDdsHeader->dwCaps4 = 0;
	m_pDdsHeader->dwReserved2 = 0;
}

void DDSFile::VCreateFile() const
{
	//Muista dwMagic alkuun
}

unsigned int DDSFile::VGetWidth() const
{
	if (m_pDdsHeader == nullptr)
		return 0;
	return m_pDdsHeader->dwWidth;
}

unsigned int DDSFile::VGetHeight() const
{
	if (m_pDdsHeader == nullptr)
		return 0;
	return m_pDdsHeader->dwHeight;
}

unsigned int DDSFile::VGetFilesize() const
{
	if (m_pDdsHeader == nullptr)
		return 0;
	return m_pDdsHeader->dwSize;
}

uint8_t * DDSFile::VGetUncompressedImageData() const
{
	return decompress();
}

void DDSFile::initializeFailed(uint8_t * dataBuffer, std::string & cause) const
{
	delete dataBuffer;
	throw MyException(cause);
}

uint8_t * DDSFile::DXT1Compress(const uint8_t* const uncompressedData) const
{
	return nullptr;
}

uint8_t * DDSFile::decompress() const
{
	if (m_mainData == nullptr)
		return nullptr;
	return nullptr;
}
